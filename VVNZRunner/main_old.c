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
#include "codec_engine_bridge.h"
#include "memory.h"
#include "vnz_wrapper.h"
#include "include/vvnzrunner.h"

#define ENABLE_GPO_PROXY

#define sceKernelDcacheCleanRange_1 sceKernelCpuDcacheAndL2WritebackRange

int(*_sceVeneziaEventHandler)(int resume, int eventid, void *args, void *opt);

typedef struct SceVeneziaCommParam {
	unsigned int requestedCodeBase;
	unsigned int requestedCodeOffset;
} SceVeneziaCommParam;

static unsigned char s_injectCode[24] = {
	0xC0, 0x6F, 0x1A, 0x7B, 0x4E, 0xC9, 0x04, 0x00, 0x4E, 0x00, 0x06, 0x4B,
	0x00, 0x99, 0x0F, 0x10, 0x07, 0x4B, 0x40, 0x6F, 0xBE, 0x10, 0x00, 0x00
};

static unsigned int s_requestedCodeSize = 0;
static void* s_requestedCodeBaseVaddr = NULL;

static void *s_commHeapVaddr = NULL;
static void *s_commHeapPaddr = NULL;

static SceUID s_registeredPid = SCE_UID_INVALID_UID;
static int *_vnzExecClockFrequency = NULL;
static int targetExecClockFrequency = 166;

void _vnzDoReset()
{
	//Reset VENEZIA
	_sceVeneziaEventHandler(0, 0x400e, NULL, NULL);
	_sceVeneziaEventHandler(1, 0x1000e, NULL, NULL);
}

int vnzBridgeRestore()
{
	int ret = 0;

	if (s_registeredPid == SCE_UID_INVALID_UID)
		return SCE_ERROR_ERRNO_EALREADY;

	vnzBridgeUnmapMemory(s_requestedCodeBaseVaddr, s_requestedCodeSize, 0);

	sceVeneziaHeapFree(SCE_VENEZIA_HEAP_IMAGE, s_commHeapVaddr, COMM_HEAP_SIZE);
	s_commHeapVaddr = NULL;
	s_commHeapPaddr = NULL;

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
	void* requestedCodeBase = NULL;
	unsigned int requestedCodeOffset = 0;
	Elf32_Ehdr *hdr = NULL;
	Elf32_Shdr *shdr = NULL;
	SceUID blockMapId = SCE_UID_INVALID_UID;
	void *kernelPage = SCE_NULL;
	SceSize kernelSize = 0;
	unsigned int kernelOffset = 0;

	if (!mepElfMemblockAddr || !mepElfMemblockSize || !IS_SIZE_ALIGNED(mepElfMemblockSize, SCE_KERNEL_256KiB))
		return SCE_ERROR_ERRNO_EINVAL;

	if (s_registeredPid != SCE_UID_INVALID_UID)
		return SCE_ERROR_ERRNO_EALREADY;

	s_commHeapVaddr = sceVeneziaHeapAlloc(SCE_VENEZIA_HEAP_IMAGE, COMM_HEAP_SIZE);
	s_commHeapPaddr = sceVeneziaConvertVirtualToPhysicalForVenezia(s_commHeapVaddr);
	if (!s_commHeapVaddr || !s_commHeapPaddr)
		return SCE_ERROR_ERRNO_ENOMEM;

	sceKernelDcacheCleanRange_1(s_commHeapVaddr, COMM_HEAP_SIZE);

	blockMapId = sceKernelUserMap("SceVeneziaMepExec", 3, mepElfMemblockAddr, mepElfMemblockSize, &kernelPage, &kernelSize, &kernelOffset);
	if (blockMapId <= 0)
		return blockMapId;

	kernelPage = (void*)(((uintptr_t)kernelPage) + kernelOffset);

	hdr = (Elf32_Ehdr *)kernelPage;

	if (memcmp(hdr->e_ident, "\x7F\x45\x4C\x46\x01\x01\x01", 7)) {
		ret = SCE_ERROR_ERRNO_ENOEXEC;
		goto return_with_unmap;
	}

	for (int i = 0; i < hdr->e_shnum; i++) {
		shdr = kernelPage + hdr->e_shoff + (hdr->e_shentsize * i);
		if (shdr->sh_flags == SHF_ALLOC | SHF_EXECINSTR && shdr->sh_type == SHT_PROGBITS) {
			break;
		}
		shdr = NULL;
	}

	if (!shdr) {
		ret = SCE_ERROR_ERRNO_ENOEXEC;
		goto return_with_unmap;
	}

	requestedCodeOffset = hdr->e_entry;
	s_requestedCodeSize = mepElfMemblockSize;
	s_requestedCodeBaseVaddr = mepElfMemblockAddr;

	ret = _sceAvcodecMapMemoryToVenezia(&requestedCodeBase, s_requestedCodeBaseVaddr, s_requestedCodeSize, SCE_KERNEL_MEMORY_REF_PERM_KERN_R, 1, 1);
	if (ret < 0) {
		goto return_with_unmap;
	}

	if (!IS_SIZE_ALIGNED((unsigned int)requestedCodeBase, SCE_KERNEL_64KiB)) {
		ret = SCE_ERROR_ERRNO_EINVAL;
		vnzBridgeUnmapMemory(s_requestedCodeBaseVaddr, s_requestedCodeSize, 0);
		goto return_with_unmap;
	}

	/*
	_vnzPatcherSetOffset((unsigned short)((unsigned int)s_requestedCodeBase >> 16));
	_vnzPatcherPatchMepCode(kernelPage + shdr->sh_offset, shdr->sh_size);
	*/

	sceKernelUserUnmap(blockMapId);
	if (ret < 0) {
		return ret;
	}

	SceVeneziaCommParam *commParam = (SceVeneziaCommParam *)s_commHeapVaddr;
	commParam->requestedCodeBase = requestedCodeBase;
	commParam->requestedCodeOffset = requestedCodeOffset;

	s_registeredPid = sceKernelGetProcessId();

	return ret;

return_with_unmap:

	sceKernelUserUnmap(blockMapId);

	return ret;
}

int vnzBridgeExec(void *pUserArg, unsigned int userArgSize)
{
	int ret = 0;
	SceVeneziaThunkArg thunkArg;

	if (s_registeredPid == SCE_UID_INVALID_UID)
		return SCE_ERROR_ERRNO_EPERM;

	if (pUserArg) {
		if (userArgSize > COMM_HEAP_SIZE - sizeof(SceVeneziaCommParam))
			return SCE_ERROR_ERRNO_E2BIG;

		sceKernelCopyFromUser(s_commHeapVaddr + sizeof(SceVeneziaCommParam), pUserArg, userArgSize);
		thunkArg.userArg2 = s_commHeapPaddr + sizeof(SceVeneziaCommParam);
	}
	else
		thunkArg.userArg2 = 0;

	ret = sceVeneziaLockProcessSuspend();
	if (ret < 0)
		return ret;

	thunkArg.pVThreadProcessingResource = sceVeneziaGetVThreadProcessingResource(0);
	thunkArg.userArg3 = userArgSize;
	thunkArg.userArg4 = s_commHeapPaddr;

	sceKernelDcacheCleanRange_1(s_commHeapVaddr, COMM_HEAP_SIZE);

	ret = sceVeneziaRpcCallGenericThunk(THUNK_CMD_ID, &thunkArg, NULL, NULL);

	sceVeneziaUnlockProcessSuspend();

	if (pUserArg) {
		sceKernelCopyToUser(pUserArg, s_commHeapVaddr + sizeof(SceVeneziaCommParam), userArgSize);
	}

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

		int ret = vnzBridgeRestore();
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
	module_get_offset(KERNEL_PID, info.modid, 0, 0x161FC | 1, (uintptr_t *)&_sceCodecEngineAllocMemoryFromUnmapMemBlock);
	module_get_offset(KERNEL_PID, info.modid, 0, 0x162FC | 1, (uintptr_t *)&_sceCodecEngineFreeMemoryFromUnmapMemBlock);

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