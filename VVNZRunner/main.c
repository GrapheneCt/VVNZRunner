#include <stdlib.h>
#include <stddef.h>
#include <scetypes.h>
#include <stddef.h>
#include <kernel.h>
#include <kernel/sysmem.h>
#include <kernel/sysmem/memblock.h>
#include <kernel/proc_event.h>
#include <taihen.h>

#include "common.h"
#include "elf.h"
#include "perf.h"
#include "memory.h"
#include "vnz_wrapper.h"
#include "include/vvnzrunner.h"

#define ENABLE_GPO_PROXY

int(*_sceVeneziaEventHandler)(int resume, int eventid, void *args, void *opt);

unsigned char s_injectCode[36] = {
	0xC0, 0x6F, 0x1A, 0x7B, 0x21, 0xC3, 0x84, 0xF1, 0x21, 0xC9, 0x84, 0xF1,
	0x06, 0x4B, 0x94, 0xC9, 0x04, 0x14, 0x34, 0xC3, 0x00, 0x14, 0x3E, 0x03,
	0x9E, 0x00, 0x00, 0x93, 0x0F, 0x10, 0x07, 0x4B, 0x40, 0x6F, 0xBE, 0x10
};


static unsigned int s_requestedCodeOffset = 0;
static unsigned int s_requestedCodeSize = 0;
static void* s_requestedCodeBase = NULL;
static void* s_requestedCodeBaseVaddr = NULL;

static SceUID s_registeredPid = SCE_UID_INVALID_UID;
static int *_vnzExecClockFrequency = NULL;
static int targetExecClockFrequency = 166;

void _vnzDoReset()
{
	//Reset VENEZIA
	_sceVeneziaEventHandler(0, 0x400e, NULL, NULL);
	_sceVeneziaEventHandler(1, 0x1000e, NULL, NULL);
}

void _vnzCommBegin(SceCodecEngineWrapperRpcMemoryContext context, SceCodecEngineWrapperThunkArg *arg, void *memoryRegion)
{
	SceCodecEngineWrapperConvertOpt opt;
	opt.flags = 1;
	opt.size = COMM_ARG_SIZE;

	*(void **)memoryRegion = arg->pVThreadProcessingResource;
	*(void **)(memoryRegion + 0x8) = arg->userArg3;
	*(void **)(memoryRegion + 0xC) = arg->userArg4;

	sceCodecEngineWrapperConvertVirtualToPhysical(context, arg->userArg2, COMM_ARG_SIZE, &opt);
	sceCodecEngineWrapperInitRpcMemory(context);
	sceCodecEngineWrapperMemcpyChain(context);

	*(void **)(memoryRegion + 0x4) = *(void **)((void*)context + 0x1C);
}

void _vnzCommEnd(SceCodecEngineWrapperRpcMemoryContext context)
{
	sceCodecEngineWrapperTermRpcMemory(context);
	sceCodecEngineWrapperMemcpyChain(context);
	sceCodecEngineWrapperConvertPhysicalToVirtual(context);
}

int vnzBridgeRestore()
{
	int ret = 0;

	if (s_registeredPid == SCE_UID_INVALID_UID)
		return SCE_ERROR_ERRNO_EALREADY;

	ret = vnzBridgeUnmapMemory(s_requestedCodeBaseVaddr, s_requestedCodeSize, 0);
	if (ret < 0)
		return ret;

	unsigned int state = __builtin_mrc(15, 0, 13, 0, 3);
	__builtin_mcr(15, 0, 13, 0, 3, state << 16);
	_vnzDoReset();
	__builtin_mcr(15, 0, 13, 0, 3, state);

	s_registeredPid = SCE_UID_INVALID_UID;

	return ret;
}

int vnzBridgeInject(const void *mepElfMemblockAddr, unsigned int mepElfMemblockSize)
{
	int ret = 0;
	SceUInt32 mepCodeFileSize = 0;
	Elf32_Ehdr hdr;

	if (!mepElfMemblockAddr || !mepElfMemblockSize || !IS_SIZE_ALIGNED(mepElfMemblockSize, SCE_KERNEL_256KiB))
		return SCE_ERROR_ERRNO_EINVAL;

	if (s_registeredPid != SCE_UID_INVALID_UID)
		return SCE_ERROR_ERRNO_EALREADY;

	ret = sceKernelCopyFromUser(&hdr, mepElfMemblockAddr, sizeof(Elf32_Ehdr));
	if (ret < 0)
		return ret;

	if (memcmp(hdr.e_ident, "\x7F\x45\x4C\x46\x01\x01\x01", 7)) {
		return SCE_ERROR_ERRNO_ENOEXEC;
	}

	s_requestedCodeOffset = hdr.e_entry;
	s_requestedCodeSize = mepElfMemblockSize;
	s_requestedCodeBaseVaddr = mepElfMemblockAddr;

	ret = _sceAvcodecMapMemoryToVenezia(&s_requestedCodeBase, s_requestedCodeBaseVaddr, s_requestedCodeSize, SCE_KERNEL_MEMORY_REF_PERM_KERN_R, 1, 1);
	if (ret < 0) {
		return ret;
	}

	s_registeredPid = sceKernelGetProcessId();

	return ret;
}

int vnzBridgeExec(void *pUserArg, unsigned int userArgSize)
{
	SceCodecEngineWrapperThunkArg thunkArg;
	int ret = 0;
	unsigned char hasUserArg = 0;
	char userArg[COMM_ARG_SIZE];
	SceCodecEngineWrapperRpcMemoryCommBegin cbBegin = NULL;
	SceCodecEngineWrapperRpcMemoryCommEnd cbEnd = NULL;

	if (s_registeredPid == SCE_UID_INVALID_UID)
		return SCE_ERROR_ERRNO_EPERM;

	memset(&thunkArg, 0, sizeof(SceCodecEngineWrapperThunkArg));

	if (pUserArg) {
		if (userArgSize > COMM_ARG_SIZE)
			return SCE_ERROR_ERRNO_E2BIG;

		memset(userArg, 0, sizeof(userArg));
		sceKernelCopyFromUser(userArg, pUserArg, userArgSize);
		cbBegin = _vnzCommBegin;
		cbEnd = _vnzCommEnd;
		thunkArg.userArg2 = userArg;
	}

	ret = sceCodecEngineWrapperLockProcessSuspendCore();
	if (ret < 0)
		return ret;

	unsigned int oldSpram0 = *(unsigned int *)(VADDR_VENEZIA_SPRAM + SPRAM_USE_OFFSET);
	unsigned int oldSpram4 = *(unsigned int *)(VADDR_VENEZIA_SPRAM + SPRAM_USE_OFFSET + 4);
		
	*(unsigned int *)(VADDR_VENEZIA_SPRAM + SPRAM_USE_OFFSET) = s_requestedCodeBase;
	*(unsigned int *)(VADDR_VENEZIA_SPRAM + SPRAM_USE_OFFSET + 4) = s_requestedCodeOffset;

	thunkArg.pVThreadProcessingResource = sceCodecEngineWrapperGetVThreadProcessingResource(0);

	ret = sceCodecEngineWrapperCallGenericThunk(THUNK_CMD_ID, &thunkArg, cbBegin, cbEnd);

	*(unsigned int *)(VADDR_VENEZIA_SPRAM + SPRAM_USE_OFFSET) = oldSpram0;
	*(unsigned int *)(VADDR_VENEZIA_SPRAM + SPRAM_USE_OFFSET + 4) = oldSpram4;

	sceCodecEngineWrapperUnlockProcessSuspendCore();

	return ret;
}

int vnzBridgeGetVeneziaExecClockFrequency()
{
	return *_vnzExecClockFrequency;
}

int vnzBridgeSetVeneziaExecClockFrequency(int clock)
{
	if (clock == 0x6f || clock == 0x37 || clock == 0x53 || clock == 0x29 || clock == 0xde || clock == 0x14d || clock == 0xa6) {
		targetExecClockFrequency = clock;
		*_vnzExecClockFrequency = clock;
		return SCE_OK;
	}
	else {
		return SCE_ERROR_ERRNO_EINVAL;
	}
}

int procEvHandler(SceUID pid, int event_type, SceProcEventInvokeParam1 *a3, int a4)
{
	if (event_type == 0x1000) {
		if (s_registeredPid == pid && targetExecClockFrequency != 166) {
			*_vnzExecClockFrequency = 166;
		}
	}
	else if (event_type == 0x40000) {
		if (s_registeredPid == pid && targetExecClockFrequency != 166) {
			*_vnzExecClockFrequency = targetExecClockFrequency;
		}
	}

	return SCE_OK;
}

int procEvExitHandler(SceUID pid, SceProcEventInvokeParam1 *a2, int a3)
{
	if (s_registeredPid == pid) {

		if (targetExecClockFrequency != 166) {
			*_vnzExecClockFrequency = 166;
			targetExecClockFrequency = 166;
		}

		vnzBridgeRestore();
	}

	return SCE_OK;
}

#ifdef ENABLE_GPO_PROXY

static unsigned int s_oldGPO = 0;

int _vnzDebugProxyThread(SceSize args, void *argp)
{
	unsigned int currGPO = 0;

	while (1) {

		currGPO = *(unsigned int *)(VADDR_VENEZIA_SPRAM + SPRAM_USE_OFFSET + 8);

		if (currGPO != s_oldGPO) {
			sceKernelSetGPO(currGPO);
		}

		s_oldGPO = currGPO;

		sceKernelDelayThread(100);
	}

	return sceKernelExitDeleteThread(0);
}
#endif

int module_start(SceSize args, const void * argp)
{
	tai_module_info_t info;
	memset(&info, 0, sizeof(tai_module_info_t));
	info.size = sizeof(tai_module_info_t);
	if (taiGetModuleInfoForKernel(KERNEL_PID, "SceCodecEngineWrapper", &info) < 0)
		return SCE_KERNEL_START_SUCCESS;

	module_get_offset(KERNEL_PID, info.modid, 0, 0x8C | 1, (uintptr_t *)&_sceVeneziaEventHandler);
	module_get_offset(KERNEL_PID, info.modid, 0, 0x36EC | 1, (uintptr_t *)&_sceVeneziaGetProcessorLoad);
	module_get_offset(KERNEL_PID, info.modid, 1, 0xBC, (uintptr_t *)&_vnzExecClockFrequency);

	memset(&info, 0, sizeof(tai_module_info_t));
	info.size = sizeof(tai_module_info_t);
	if (taiGetModuleInfoForKernel(KERNEL_PID, "SceAvcodec", &info) < 0)
		return SCE_KERNEL_START_SUCCESS;

	module_get_offset(KERNEL_PID, info.modid, 0, 0x89C0 | 1, (uintptr_t *)&_sceAvcodecMapMemoryToVenezia);

	SceProcEventHandler handler;
	memset(&handler, 0, sizeof(SceProcEventHandler));
	handler.size = sizeof(SceProcEventHandler);
	handler.stop = procEvHandler;
	handler.start = procEvHandler;
	handler.exit = procEvExitHandler;
	handler.kill = procEvExitHandler;

	sceKernelRegisterProcEventHandler("SceVeneziaProcEvHandler", &handler, 0);

	//Inject code loader
	*(unsigned int *)(VADDR_VENEZIA_IMAGE + THUNK_PTR_INJECT_ADDR) = THUNK_PTR_INJECT_DST;
	memcpy(VADDR_VENEZIA_IMAGE + INJECT_CODE_BASE_OFFSET, s_injectCode, sizeof(s_injectCode));

	_vnzDoReset();

#ifdef ENABLE_GPO_PROXY
	*(unsigned int *)(VADDR_VENEZIA_SPRAM + SPRAM_USE_OFFSET + 8) = 0;
	SceUID thid = sceKernelCreateThread("SceVeneziaDebugProxy", _vnzDebugProxyThread, SCE_KERNEL_DEFAULT_PRIORITY, SCE_KERNEL_4KiB, 0, 0, NULL);
	sceKernelStartThread(thid, 0, NULL);
#endif

	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, const void * argp)
{
	return SCE_KERNEL_STOP_SUCCESS;
}