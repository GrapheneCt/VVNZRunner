#include <stdlib.h>
#include <stddef.h>
#include <scetypes.h>
#include <kernel.h>
#include <kernel/sysmem.h>
#include <taihen.h>

//#define TEST_BUILD

int(*sceVeneziaEventHandler)(int resume, int eventid, void *args, void *opt);

#define SCE_SYSMEM_VENEZIA_PARAM_IMAGE 8
#define SCE_SYSMEM_VENEZIA_PARAM_SPRAM 1

#define VADDR_PERVASIVE2_REG	0x280E2000
#define VADDR_VENEZIA_SPRAM		0x28100000
#define VADDR_VENEZIA_IMAGE		0x28200000

#define PADDR_VENEZIA_IMAGE		0x40800000
#define PADDR_SPRAM_BASE		0x40300000

#define IMAGE_MEMSIZE			0x400000
#define SPRAM_MEMSIZE			0x2500000

#define INJECT_CODE_SIZE_LIMIT	0x624

typedef struct SceCodecEngineWrapperThunkArg {
	int processingResource;
	int a2;
	int a3;
	int a4;
} SceCodecEngineWrapperThunkArg;

typedef struct SceSysmemVeneziaImageParam {
	unsigned int size; // 0xc
	unsigned int paddr;
	unsigned int memsize;
} SceSysmemVeneziaImageParam;

static unsigned char s_injectCodeJumpDefault[4] = {
	0x98, 0xDA, 0x37, 0x80
};

static unsigned char s_origCodeJump[4];
static unsigned char s_origCode[INJECT_CODE_SIZE_LIMIT];
static unsigned char s_injectCode[INJECT_CODE_SIZE_LIMIT];
static unsigned char s_injectCodeJump[4];

static unsigned int s_requestedCodeOffset = 0;
static unsigned int s_requestedCodeSize = 0;
static SceUID s_vnzCodeRunSema = SCE_UID_INVALID_UID;
static int s_keepAlive = 1;

int vnzBridgeInject(void *mepCodeJump, unsigned int mepCodeOffset, void *mepCode, unsigned int mepCodeSize)
{
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
		s_requestedCodeOffset = 0x3752; // as in s_injectCodeJumpDefault
	else
		s_requestedCodeOffset = mepCodeOffset;

	s_requestedCodeSize = mepCodeSize;

	memcpy(s_origCode, VADDR_VENEZIA_IMAGE + s_requestedCodeOffset, s_requestedCodeSize);
	sceKernelMemcpyUserToKernel(s_injectCode, mepCode, s_requestedCodeSize);

	return SCE_OK;
}

int vnzBridgeExec()
{
	return sceKernelSignalSema(s_vnzCodeRunSema, 1);
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

	unsigned int value = *(unsigned int *)(VADDR_VENEZIA_SPRAM + offset);

	return value;
}

int vnzRunnerThread(SceSize args, void *argp)
{
	while (s_keepAlive) {
		sceKernelWaitSema(s_vnzCodeRunSema, 1, SCE_NULL);
		sceDebugPrintf("begin\n");
		sceVeneziaEventHandler(0, 0x400e, NULL, NULL);

		memcpy(VADDR_VENEZIA_IMAGE, s_injectCodeJump, 4);
		memcpy(VADDR_VENEZIA_IMAGE + s_requestedCodeOffset, s_injectCode, s_requestedCodeSize);
		sceDebugPrintf("mid\n");
		sceVeneziaEventHandler(1, 0x1000e, NULL, NULL);

		memcpy(VADDR_VENEZIA_IMAGE, s_origCodeJump, 4);
		memcpy(VADDR_VENEZIA_IMAGE + s_requestedCodeOffset, s_origCode, s_requestedCodeSize);
		sceDebugPrintf("end\n");
	}

	return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, const void * argp)
{
	tai_module_info_t info;
	info.size = sizeof(tai_module_info_t);
	if (taiGetModuleInfoForKernel(KERNEL_PID, "SceCodecEngineWrapper", &info) < 0)
		return SCE_KERNEL_START_SUCCESS;

	module_get_offset(KERNEL_PID, info.modid, 0, 0x8C | 1, (uintptr_t *)&sceVeneziaEventHandler);

	s_vnzCodeRunSema = sceKernelCreateSema("SceVeneziaCodeRunSema", 0, 0, 1, SCE_NULL);
	if (s_vnzCodeRunSema <= 0)
		return SCE_KERNEL_START_SUCCESS;

	SceUID thid = sceKernelCreateThread("SceVeneziaRunnerThread", vnzRunnerThread, 0x10000100, 0x1000, 0, 0, NULL);
	if (thid <= 0)
		return SCE_KERNEL_START_SUCCESS;
	sceKernelStartThread(thid, 0, NULL);

	memcpy(s_origCodeJump, VADDR_VENEZIA_IMAGE, 4);

	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, const void * argp)
{
	return SCE_KERNEL_STOP_SUCCESS;
}

#ifdef TEST_BUILD
int vnz_test_get_clock()
{
	return scePowerGetVeneziaClockFrequency();
}

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

	fd = sceIoOpen("uma0:vnz_image_dump.bin", SCE_O_WRONLY | SCE_O_CREAT, 0777);
	sz = sceIoWrite(fd, VADDR_VENEZIA_IMAGE, IMAGE_MEMSIZE);
	sceIoClose(fd);

	return sz;
}
#else
int vnz_test_get_clock()
{
	return 333;
}

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