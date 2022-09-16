#include <kernel.h>
#include <codecengine.h>
#include <audiodec.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <display.h>

#include "../../VVNZRunner/include/vvnzrunner.h"

//#define TEST_ARM

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))

#define IS_SIZE_ALIGNED( sizeToTest, PowerOfTwo )  \
		(((sizeToTest) & ((PowerOfTwo) - 1)) == 0)

#define VTHREAD_NUM				1
#define PERF_PMON_GRAIN_USEC	1000
#define PERF_PRINTF_GRAIN_USEC	200000

#define WORKMEM_SIZE			ALIGN(960 * 544 * 4, SCE_KERNEL_256KiB)

#define sceCodecEnginePmonStop _sceCodecEnginePmonStop
#define sceCodecEnginePmonReset _sceCodecEnginePmonReset
#define sceCodecEnginePmonStart _sceCodecEnginePmonStart
int _sceCodecEnginePmonGetProcessorLoad(SceCodecEnginePmonProcessorLoad *res, SceCodecEnginePmonProcessorLoad *scheck);
int sceCodecEnginePmonGetProcessorLoad(SceCodecEnginePmonProcessorLoad *res)
{
	SceCodecEnginePmonProcessorLoad scheck;
	scheck.size = sizeof(SceCodecEnginePmonProcessorLoad);
	return _sceCodecEnginePmonGetProcessorLoad(res, &scheck);
}

#define VNZ_CMD_NONE			0
#define VNZ_CMD_DEBUG_FONT_DRAW	1

typedef struct VnzCallArg {
	unsigned int cmd;
	unsigned int width;
	unsigned int height;
	void *addr;
	int x;
	int y;
	int lastError;
	char text[400];
} VnzCallArg;

static SceUID vthreadWrapId[VTHREAD_NUM];
static void *vnzWorkMem = NULL;

void myEntryArm(void *arg)
{
	return;
}

static int targY = 0;

int VThreadInterface(SceSize args, void *argp)
{
	VnzCallArg arg;
	int err = 0;

	arg.cmd = VNZ_CMD_DEBUG_FONT_DRAW;
	arg.addr = vnzWorkMem;
	arg.width = 960;
	arg.height = 544;
	arg.x = 0;
	arg.y = targY += 64;
	snprintf(arg.text, sizeof(arg.text), "Testing SceBSOD font on VNZ: %d", targY);

#ifdef TEST_ARM
	SceUInt32 waitbeg = sceKernelGetProcessTimeLow();
	myEntryArm(0);
	sceClibPrintf("myEntryArm time: %u\n", sceKernelGetProcessTimeLow() - waitbeg);
#else
	SceUInt32 waitbeg = sceKernelGetProcessTimeLow();
	//err = vnzBridgeExec(&arg, sizeof(VnzCallArg));
	test();
	sceClibPrintf("vnzBridgeExec time: %u\n", sceKernelGetProcessTimeLow() - waitbeg);
	//sceClibPrintf("vnzBridgeExec: 0x%X\n", err);
	//sceClibPrintf("last error: 0x%X\n", arg.lastError);
#endif
	return sceKernelExitDeleteThread(0);
}

void test()
{
	VnzCallArg arg;

	arg.cmd = VNZ_CMD_DEBUG_FONT_DRAW;
	arg.addr = vnzWorkMem;
	arg.width = 960;
	arg.height = 544;
	arg.x = 0;
	arg.y = 0;
	snprintf(arg.text, sizeof(arg.text), "Start VENEZIA print test");

	int err = vnzBridgeExec(&arg, sizeof(VnzCallArg));

	sceClibPrintf("vnzBridgeExec: 0x%X\n", err);
	sceClibPrintf("last error: 0x%X\n", arg.lastError);

	/*arg.y = 32;
	snprintf(arg.text, sizeof(arg.text), "In progress...");
	vnzBridgeExec(&arg, sizeof(VnzCallArg));

	arg.y = 64;
	snprintf(arg.text, sizeof(arg.text), "Done!");
	vnzBridgeExec(&arg, sizeof(VnzCallArg));*/
}

SceUID spawnVThread()
{
	int err = 0;

	SceUID thid = sceKernelCreateThread("VThreadInterface", VThreadInterface, 160, 0x1000, 0, 0, NULL);
	sceKernelStartThread(thid, 0, NULL);

	return thid;
}

int loadMepElf(const char *path, void **vaddr, unsigned int *mbSize)
{
	SceUInt32 mepCodeFileSize = 0;
	SceUID fd;
	int ret = SCE_OK;

	if (!path || !vaddr || !mbSize)
		return SCE_ERROR_ERRNO_EINVAL;

	fd = sceIoOpen(path, SCE_O_RDONLY, 0);
	if (fd <= 0)
		return fd;

	mepCodeFileSize = (SceUInt32)sceIoLseek(fd, 0, SCE_SEEK_END);
	sceIoLseek(fd, 0, SCE_SEEK_SET);

	*mbSize = ALIGN(mepCodeFileSize, SCE_KERNEL_1MiB);

	SceUID codeMemUID = sceKernelAllocMemBlock("SceVeneziaElfExec", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, *mbSize, NULL);
	if (codeMemUID <= 0) {
		sceIoClose(fd);
		return codeMemUID;
	}

	ret = sceKernelGetMemBlockBase(codeMemUID, vaddr);
	if (ret < 0) {
		sceIoClose(fd);
		return ret;
	}

	sceClibMemset(*vaddr, 0, *mbSize);
	sceIoRead(fd, *vaddr, mepCodeFileSize);
	sceIoClose(fd);

	return ret;
}

int unloadMepElf(void *vaddr)
{
	if (!vaddr)
		return SCE_ERROR_ERRNO_EINVAL;

	SceUID mbId = sceKernelFindMemBlockByAddr(vaddr, 0);
	sceKernelFreeMemBlock(mbId);
}

int main()
{
	void *codeMem;
	unsigned int codeMemSize;

	int err = 0;
	int zero = 0;
	int one = 1;

	sceClibPrintf("-------------------- START: Venezia test --------------------\n");

	char blank[32];
	int state = 0;
	int status = 0;
	SceUInt64 waitStartUsec = 0;
	SceUInt64 currentTimeMeasureUsec = 0;
	SceUInt64 oldTimeMeasureUsec = 0;
	SceUInt64 waitEndUsec = 0;
	SceCodecEnginePmonProcessorLoadExt perf;

	// Inject custom MeP code
	sceClibPrintf("Inject custom MeP code\n");

	err = loadMepElf("app0:mep_code.elf", &codeMem, &codeMemSize);
	sceClibPrintf("loadMepElf: 0x%X\n", err);

	err = vnzBridgeInject(codeMem, codeMemSize);
	sceClibPrintf("vnzBridgeInject: 0x%X\n", err);
	
	void *workMem;
	SceUID workMemUID = sceKernelAllocMemBlock("SceVeneziaElfExecTest", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, WORKMEM_SIZE, NULL);
	err = sceKernelGetMemBlockBase(workMemUID, &workMem);
	sceClibPrintf("sceKernelGetMemBlockBase: 0x%X\n", err);

	//memset(workMem, 0, WORKMEM_SIZE);

	err = vnzBridgeMapMemory(workMem, WORKMEM_SIZE, &vnzWorkMem, 1);
	sceClibPrintf("vnzBridgeMapMemory: 0x%X\n", err);

	/*SceUID vaid = sceCodecEngineOpenUnmapMemBlock(workMem, WORKMEM_SIZE);
	sceClibPrintf("sceCodecEngineOpenUnmapMemBlock: 0x%X\n", vaid);

	err = vnzBridgeAllocUnmapMemory(vaid, SCE_KERNEL_1MiB, 0x100, &vnzWorkMem);
	sceClibPrintf("vnzBridgeAllocUnmapMemory: 0x%X\n", err);
	sceClibPrintf("vnz mem: 0x%X\n", vnzWorkMem);

	err = vnzBridgeFreeUnmapMemory(vaid, vnzWorkMem, SCE_KERNEL_1MiB);
	sceClibPrintf("vnzBridgeFreeUnmapMemory: 0x%X\n", err);*/
	

	// Spawn V-Threads
	sceClibPrintf("Spawn V-Threads, total num: %d\n", VTHREAD_NUM);

#ifndef TEST_ARM
	//vnzBridgeSetVeneziaExecClockFrequency(333);
	//sceCodecEngineChangeNumWorkerCoresMax();
#endif

	for (int i = 0; i < VTHREAD_NUM; i++) {
		sceClibPrintf("Spawning V-Thread %d\n", i);
		vthreadWrapId[i] = spawnVThread();

		// Wait until V-Thread is ready on Venezia side
		/*sceClibPrintf("Waiting for V-Thread to start\n");
		do {
			sceKernelDelayThread(10);
			vnzBridgeMemcpyFromSpram(&state, sizeof(int), SPRAM_VTHREAD_MAILBOX_OFFSET + (sizeof(int) * i));
			sceClibPrintf("V-Thread state: %d\n", state);
		} while (!state);*/
	}

	// Wait for all V-Threads to finish
	sceClibPrintf("Wait for all V-Threads to finish\n");

	// Get wait start time
	waitStartUsec = sceKernelGetProcessTimeWide();
	oldTimeMeasureUsec = waitStartUsec;

repeat_wait:
	sceKernelDelayThread(PERF_PMON_GRAIN_USEC);

	// Get average Venezia CPU load while waiting
	sceCodecEnginePmonStop();
	sceCodecEnginePmonGetProcessorLoadExt(&perf);
	sceCodecEnginePmonReset();
	sceCodecEnginePmonStart();
	currentTimeMeasureUsec = sceKernelGetProcessTimeWide();
	if (currentTimeMeasureUsec - oldTimeMeasureUsec > PERF_PRINTF_GRAIN_USEC) {
		sceClibPrintf("\nPer-core Venezia CPU load:");
		sceClibPrintf("\n-----------------------------------------------------------------\n");
		sceClibPrintf("| %03d %% | %03d %% | %03d %% | %03d %% | %03d %% | %03d %% | %03d %% | %03d %% |", perf.core0, perf.core1, perf.core2, perf.core3, perf.core4, perf.core5, perf.core6, perf.core7);
		sceClibPrintf("\n-----------------------------------------------------------------\n");
		oldTimeMeasureUsec = currentTimeMeasureUsec;
		sceClibPrintf("clock: %d\n", vnzBridgeGetVeneziaExecClockFrequency());
	}

	for (int i = 0; i < VTHREAD_NUM; i++) {
		state = sceKernelGetThreadExitStatus(vthreadWrapId[i], &status);
		if (state == SCE_KERNEL_ERROR_NOT_DORMANT) {
			goto repeat_wait;
		}
	}

	// Get wait end time
	waitEndUsec = sceKernelGetProcessTimeWide();

	sceClibPrintf("All V-Threads have finished their jobs, total time: %llu usec\n", waitEndUsec - waitStartUsec);

	sceClibPrintf("-------------------- END: Venezia test --------------------\n\n");

	vnzBridgeUnmapMemory(workMem, WORKMEM_SIZE, 1);

	/*
	SceUID file = sceIoOpen("ux0:data/dump.bin", SCE_O_WRONLY | SCE_O_CREAT, 0666);
	sceIoWrite(file, workMem, 0x10000);
	sceIoClose(file);
	*/
	
	SceDisplayFrameBuf fb;
	fb.size = sizeof(SceDisplayFrameBuf);
	fb.base = workMem;
	fb.width = 960;
	fb.height = 544;
	fb.pitch = 960;
	fb.pixelformat = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;

	sceDisplaySetFrameBuf(&fb, SCE_DISPLAY_UPDATETIMING_NEXTVSYNC);

	vnzBridgeRestore();

	while (1) {
		sceKernelDelayThread(1000);
	}

	return 0;
}