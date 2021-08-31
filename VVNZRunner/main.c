#include <stdlib.h>
#include <stddef.h>
#include <scetypes.h>
#include <stddef.h>
#include <kernel.h>
#include <kernel/sysmem.h>
#include <kernel/proc_event.h>
#include <taihen.h>

//#define TEST_BUILD

int(*_sceVeneziaEventHandler)(int resume, int eventid, void *args, void *opt);
int(*_sceAvcodecMapMemoryToVenezia)(void **vnzPaddr, const void *vaddr, unsigned int size, SceKernelMemoryRefPerm perm, unsigned int mode, unsigned int plsAllowVnz);
int(*_sceAvcodecJpegEncoderThunkBegin)();
int(*_sceAvcodecJpegEncoderThunkEnd)();

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))

#define SCE_SYSMEM_VENEZIA_PARAM_IMAGE 8
#define SCE_SYSMEM_VENEZIA_PARAM_SPRAM 1

#define VADDR_PERVASIVE2_REG	0x280E2000
#define VADDR_VENEZIA_SPRAM		0x28100000
#define VADDR_VENEZIA_IMAGE		0x28200000
#define VADDR_VENEZIA_VIP		0x28600000

#define PADDR_VENEZIA_SPRAM		0x1F840000
#define SPRAM_MAP_SIZE			0x20000

#define VIP_MEMSIZE				0x100000
#define IMAGE_MEMSIZE			0x400000
#define SPRAM_MEMSIZE			0x2500000

#define INJECT_CODE_SIZE_LIMIT	0x1000
#define INJECT_CODE_BASE_OFFSET	0x1F988

#define scePUIDOpenByGUID			sceKernelCreateUserUid
#define scePUIDClose				sceKernelDeleteUserUid
#define	scePUIDtoGUID				sceKernelKernelUidForUserUid
#define sceKernelVARangeToPAVector	sceKernelGetPaddrList

typedef struct SceCodecEngineWrapperThunkArg {
	void *pVThreadProcessingResource;
	void *userArg2;
	void *userArg3;
	void *userArg4;
} SceCodecEngineWrapperThunkArg;

typedef struct SceSysmemVeneziaImageParam {
	unsigned int size; // 0xc
	unsigned int paddr;
	unsigned int memsize;
} SceSysmemVeneziaImageParam;

typedef struct SceKernelVARange {
	uint32_t addr;
	SceSize size;
} SceKernelVARange;

typedef struct SceKernelPARange {
	uint32_t addr;
	SceSize size;
} SceKernelPARange;

typedef struct SceKernelPAVector { // size is 0x14 on FW 0.990
	SceSize size;			// Size of this structure
	uint32_t pRanges_size;		// Ex: 8
	uint32_t nDataInVector;		// Must be <= 8
	uint32_t count;
	SceKernelPARange *pRanges;
} SceKernelPAVector;

int sceCodecEngineWrapperCallGenericThunk(unsigned int id, SceCodecEngineWrapperThunkArg *arg, void *beginCallback, void *endCallback);
void *sceCodecEngineWrapperGetVThreadProcessingResource(unsigned int key);
int sceCodecEngineWrapperLockProcessSuspendCore();
int sceCodecEngineWrapperUnlockProcessSuspendCore();

static unsigned char s_injectCodeJumpDefault[4] = {
	0x88, 0xDC, 0x37, 0x8E
};

static unsigned char s_origCodeJump[4];
static unsigned char s_injectCodeJump[4];

static unsigned int s_requestedCodeOffset = 0;
static unsigned int s_requestedCodeSize = 0;
static SceUID s_injectMemblock = SCE_UID_INVALID_UID;
static void *s_injectMem = NULL;
static SceUID s_registeredPid = SCE_UID_INVALID_UID;
static SceUID s_userSpramMap = SCE_UID_INVALID_UID;

void _vnzDoRestore()
{
	memcpy(VADDR_VENEZIA_IMAGE + INJECT_CODE_BASE_OFFSET, s_origCodeJump, 4);
	memcpy(VADDR_VENEZIA_IMAGE + s_requestedCodeOffset, s_injectMem, s_requestedCodeSize);
}

void _vnzDoInject()
{
#define INJECTION_BASE_INFO	(VADDR_VENEZIA_SPRAM + 0x1400)
	*(unsigned int *)(INJECTION_BASE_INFO) = s_requestedCodeOffset | 0x800000;
#undef INJECTION_BASE_INFO
	memcpy(VADDR_VENEZIA_IMAGE + INJECT_CODE_BASE_OFFSET, s_injectCodeJump, 4);
	memcpy(VADDR_VENEZIA_IMAGE + s_requestedCodeOffset, s_injectMem + s_requestedCodeSize, s_requestedCodeSize);
}

int vnzBridgeMapMemory(void *vaddr, unsigned int size, void **vnzPaddr)
{
	void *res = 0;

	int ret = _sceAvcodecMapMemoryToVenezia(&res, vaddr, size, SCE_KERNEL_MEMORY_REF_PERM_KERN_R | SCE_KERNEL_MEMORY_REF_PERM_KERN_W, 2, 1);

	sceKernelMemcpyKernelToUser(vnzPaddr, &res, 4);

	return ret;
}

int vnzBridgeUnmapMemory(void *vaddr, unsigned int size)
{
	unsigned int vi = vaddr;
	return sceKernelMemRangeReleaseWithPerm(SCE_KERNEL_MEMORY_REF_PERM_KERN_R | SCE_KERNEL_MEMORY_REF_PERM_KERN_W, vi & 0xffffff00, size);
}

void _vnzDoReset()
{
	_sceVeneziaEventHandler(0, 0x400e, NULL, NULL);
	_sceVeneziaEventHandler(1, 0x1000e, NULL, NULL);
}

int vnzBridgeRestore()
{
	if (s_registeredPid == SCE_UID_INVALID_UID)
		return -4;

	_vnzDoRestore();

	sceKernelFreeMemBlock(s_injectMemblock);

	s_injectMem = NULL;
	s_injectMemblock = SCE_UID_INVALID_UID;
	s_registeredPid = SCE_UID_INVALID_UID;

	unsigned int state = __builtin_mrc(15, 0, 13, 0, 3);
	__builtin_mcr(15, 0, 13, 0, 3, state << 16);
	_vnzDoReset();
	__builtin_mcr(15, 0, 13, 0, 3, state);

	return SCE_OK;
}

int vnzBridgeInject(void *mepCodeJump, unsigned int mepCodeOffset, void *mepCode, unsigned int mepCodeSize)
{
	if (s_registeredPid != SCE_UID_INVALID_UID)
		vnzBridgeRestore();

	if (mepCodeSize > INJECT_CODE_SIZE_LIMIT)
		return -1;

	if (!mepCode)
		return -2;

	if (mepCodeOffset > 0xFFFF0)
		return -3;

	if (!mepCodeJump)
		memcpy(s_injectCodeJump, s_injectCodeJumpDefault, 4);
	else
		sceKernelMemcpyUserToKernel(s_injectCodeJump, mepCodeJump, 4);

	if (!mepCodeOffset)
		s_requestedCodeOffset = 0xE3790; // as in s_injectCodeJumpDefault
	else
		s_requestedCodeOffset = mepCodeOffset;

	s_requestedCodeSize = mepCodeSize;

	unsigned int memSize = ALIGN(s_requestedCodeSize * 2, 4096);

	s_injectMemblock = sceKernelAllocMemBlock("SceVeneziaInjectImage", SCE_KERNEL_MEMBLOCK_TYPE_KERNEL_RW_GAME, memSize, NULL);
	sceKernelGetMemBlockBase(s_injectMemblock, &s_injectMem);

	memcpy(s_injectMem, VADDR_VENEZIA_IMAGE + s_requestedCodeOffset, s_requestedCodeSize);
	sceKernelMemcpyUserToKernel(s_injectMem + s_requestedCodeSize, mepCode, s_requestedCodeSize);

	s_registeredPid = sceKernelGetProcessId();

	_vnzDoInject();

	unsigned int state = __builtin_mrc(15, 0, 13, 0, 3);
	__builtin_mcr(15, 0, 13, 0, 3, state << 16);
	_vnzDoReset();
	__builtin_mcr(15, 0, 13, 0, 3, state);

	return SCE_OK;
}

int vnzBridgeExec(void *pUserArg, unsigned int userArgSize)
{
	SceCodecEngineWrapperThunkArg thunkArg;
	int ret = 0;
	unsigned char hasUserArg = 0;
	char userArg[0x180];
	void *cbBegin = NULL;
	void *cbEnd = NULL;

	memset(&thunkArg, 0, sizeof(SceCodecEngineWrapperThunkArg));

	if (pUserArg) {
		if (userArgSize > 0x180)
			return -1;

		memset(userArg, 0, sizeof(userArg));
		sceKernelMemcpyUserToKernel(userArg, pUserArg, userArgSize);
		cbBegin = _sceAvcodecJpegEncoderThunkBegin;
		cbEnd = _sceAvcodecJpegEncoderThunkEnd;
		thunkArg.userArg2 = userArg;
	}

	ret = sceCodecEngineWrapperLockProcessSuspendCore();
	if (ret < 0)
		return ret;

	thunkArg.pVThreadProcessingResource = sceCodecEngineWrapperGetVThreadProcessingResource(0);

	ret = sceCodecEngineWrapperCallGenericThunk(0x1C5, &thunkArg, cbBegin, cbEnd);

	sceCodecEngineWrapperUnlockProcessSuspendCore();

	return ret;
}

int vnzBridgeMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset)
{
	if (spramOffset > SPRAM_MEMSIZE)
		return -3;

	return sceKernelMemcpyUserToKernel(VADDR_VENEZIA_SPRAM + spramOffset, src, size);
}

int vnzBridgeMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset)
{
	if (spramOffset > SPRAM_MEMSIZE)
		return -3;

	return sceKernelMemcpyKernelToUser(dst, VADDR_VENEZIA_SPRAM + spramOffset, size);
}

int vnzBridgeGetSpramValue(unsigned int offset)
{
	if (offset > SPRAM_MEMSIZE)
		return -3;

	int value = *(int *)(VADDR_VENEZIA_SPRAM + offset);

	return value;
}

int vnzBridgeGetVeneziaClockFrequency()
{
	return scePowerGetVeneziaClockFrequency();
}

int vnzBridgeSetVeneziaClockFrequency(int clock)
{
	return scePowerSetVeneziaClockFrequency(clock);
}

int procEvHandler(SceUID pid, int event_type, SceProcEventInvokeParam1 *a3, int a4)
{
	if (event_type == 0x1000 && s_registeredPid == pid) {
		_vnzDoRestore();
		_vnzDoReset();
	}
	else if (event_type == 0x40000 && s_registeredPid == pid) {
		_vnzDoInject();
		_vnzDoReset();
	}

	return SCE_OK;
}

int procEvExitHandler(SceUID pid, SceProcEventInvokeParam1 *a2, int a3)
{
	if (s_registeredPid == pid) {
		vnzBridgeRestore();
	}

	return SCE_OK;
}

int module_start(SceSize args, const void * argp)
{
	tai_module_info_t info;
	memset(&info, 0, sizeof(tai_module_info_t));
	info.size = sizeof(tai_module_info_t);
	if (taiGetModuleInfoForKernel(KERNEL_PID, "SceCodecEngineWrapper", &info) < 0)
		return SCE_KERNEL_START_SUCCESS;

	module_get_offset(KERNEL_PID, info.modid, 0, 0x8C | 1, (uintptr_t *)&_sceVeneziaEventHandler);

	memset(&info, 0, sizeof(tai_module_info_t));
	info.size = sizeof(tai_module_info_t);
	if (taiGetModuleInfoForKernel(KERNEL_PID, "SceAvcodec", &info) < 0)
		return SCE_KERNEL_START_SUCCESS;

	module_get_offset(KERNEL_PID, info.modid, 0, 0x89C0 | 1, (uintptr_t *)&_sceAvcodecMapMemoryToVenezia);
	module_get_offset(KERNEL_PID, info.modid, 0, 0xAA64 | 1, (uintptr_t *)&_sceAvcodecJpegEncoderThunkBegin);
	module_get_offset(KERNEL_PID, info.modid, 0, 0xAA4C | 1, (uintptr_t *)&_sceAvcodecJpegEncoderThunkEnd);

	SceProcEventHandler handler;
	memset(&handler, 0, sizeof(SceProcEventHandler));
	handler.size = sizeof(SceProcEventHandler);
	handler.stop = procEvHandler;
	handler.start = procEvHandler;
	handler.exit = procEvExitHandler;
	handler.kill = procEvExitHandler;

	sceKernelRegisterProcEventHandler("SceVeneziaProcEvHandler", &handler, 0);

	memcpy(s_origCodeJump, VADDR_VENEZIA_IMAGE + INJECT_CODE_BASE_OFFSET, 4);

	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, const void * argp)
{
	return SCE_KERNEL_STOP_SUCCESS;
}

#ifdef TEST_BUILD
int vnz_test_gate_enable()
{
	return scePervasiveVeneziaClockGateEnable();
}

int vnz_test_gate_disable()
{
	return scePervasiveVeneziaClockGateDisable();
}

int vnz_test_dump_image()
{
	SceInt32 sz;
	SceUID fd;

	/*fd = sceIoOpen("uma0:vnz_spram_dump.bin", SCE_O_WRONLY | SCE_O_CREAT, 0777);
	sz = sceIoWrite(fd, VADDR_VENEZIA_SPRAM, 0x1400);
	sceIoClose(fd);*/

	fd = sceIoOpen("uma0:vnz_vip_dump.bin", SCE_O_WRONLY | SCE_O_CREAT, 0777);
	sz = sceIoWrite(fd, VADDR_VENEZIA_VIP, VIP_MEMSIZE);
	sceIoClose(fd);

	fd = sceIoOpen("uma0:vnz_image_dump.bin", SCE_O_WRONLY | SCE_O_CREAT, 0777);
	sz = sceIoWrite(fd, VADDR_VENEZIA_IMAGE, IMAGE_MEMSIZE);
	sceIoClose(fd);

	return sz;
}
#else
int vnz_test_gate_enable()
{
	return SCE_OK;
}

int vnz_test_gate_disable()
{
	return SCE_OK;
}

int vnz_test_dump_image()
{
	return SCE_OK;
}
#endif