#include <kernel.h>
#include <codecengine.h>
#include <audiodec.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mep_code.h"
#include "../../VVNZRunner/include/vvnzrunner.h"

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))

#define IS_SIZE_ALIGNED( sizeToTest, PowerOfTwo )  \
        (((sizeToTest) & ((PowerOfTwo) - 1)) == 0)

#define STB_DXT_IMPLEMENTATION
#include "stb_dxt.h"

typedef enum DDSFormat {
	FORMAT_DXT1,
	FORMAT_DXT2,
	FORMAT_DXT3,
	FORMAT_DXT4,
	FORMAT_DXT5
} DDSFormat;

#define TEST_IMAGE_WIDTH		256
#define TEST_IMAGE_HEIGHT		256
#define DXT_FORMAT				FORMAT_DXT1
#define SRC_IMAGE_PATH			"app0:test.rgba"				

#define VTHREAD_NUM				1
#define PERF_PMON_GRAIN_USEC	1000

#define SPRAM_SAFE_OFFSET		0x1404
#define SPRAM_VTHREAD_MAILBOX_OFFSET	0x1408

#define VTHREAD_STATE_READY		1
#define VTHREAD_STATE_FINISHED	2

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

float powf_c(float x, float n);

static SceUID vthreadWrapId[VTHREAD_NUM];

typedef struct DxtCompressionArg {
	void *src;
	void *dst;
	unsigned int srcMemSize;
	unsigned int dstMemSize;
	unsigned int format;
	unsigned int mode;
	unsigned int width;
	unsigned int height;
	void *stbDxtData;
} DxtCompressionArg;

static DxtCompressionArg dxtArg;

int VThreadInterface(SceSize args, void *argp)
{
	int err = vnzBridgeExec(&dxtArg, sizeof(DxtCompressionArg));
	sceClibPrintf("vnzBridgeExec: 0x%X\n", err);

	return sceKernelExitDeleteThread(0);
}

SceUID spawnVThread()
{
	int err = 0;

	SceUID thid = sceKernelCreateThread("VThreadInterface", VThreadInterface, 160, 0x1000, 0, 0, NULL);
	sceKernelStartThread(thid, 0, NULL);

	return thid;
}

static void fileWriteString(FILE* file, const char* str) {
	fwrite(str, strlen(str), 1, file);
}

static void fileWrite32(FILE* file, int32_t value) {
	fwrite(&value, 4, 1, file);
}

static void fileWritePadding(FILE* file, int size) {
	void *padding = malloc(size);
	memset(padding, 0, size * 4);
	fwrite(padding, 4, size, file);
	free(padding);
}

int main()
{
	int err = 0;
	int zero = 0;
	int one = 1;

	sceClibPrintf("-------------------- START: memory mapping test --------------------\n");

	SceUID dstMb, srcMb, stbMb;
	SceUID fd;
	void *dstMem, *srcMem, *stbMem;
	unsigned int dstPhysmem, srcPhysmem, stbPhysmem;
	unsigned int dstSize = TEST_IMAGE_WIDTH * TEST_IMAGE_HEIGHT * 4;
	dstSize = ALIGN(dstSize, 4 * 1024);
	unsigned int srcSize = TEST_IMAGE_WIDTH * TEST_IMAGE_HEIGHT * 4;
	srcSize = ALIGN(srcSize, 4 * 1024);
	unsigned int stbSize = 4 * 1024;

	// Allocate memory block
	sceClibPrintf("Allocate memory block for destination\n");

	dstMb = sceKernelAllocMemBlock("SceVeneziaTestDst", SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_NC_RW, dstSize, NULL);
	sceClibPrintf("sceKernelAllocMemBlock: 0x%X\n", dstMb);
	sceKernelGetMemBlockBase(dstMb, &dstMem);
	*(int *)dstMem = 0x911;

	// Map for direct access from Venezia
	sceClibPrintf("Map for direct access from Venezia\n");

	err = vnzBridgeMapMemory(dstMem, dstSize, &dstPhysmem, 1);
	sceClibPrintf("vnzBridgeMapMemory: 0x%X\n", err);
	sceClibPrintf("Memory paddr: 0x%X\n", dstPhysmem);

	// Allocate memory block
	sceClibPrintf("Allocate memory block for source\n");

	srcMb = sceKernelAllocMemBlock("SceVeneziaTestSrc", SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_NC_RW, srcSize, NULL);
	sceClibPrintf("sceKernelAllocMemBlock: 0x%X\n", srcMb);
	sceKernelGetMemBlockBase(srcMb, &srcMem);

	// Read source image
	sceClibPrintf("Read source image\n");

	fd = sceIoOpen(SRC_IMAGE_PATH, SCE_O_RDONLY, 0);
	if (fd < 0) {
		sceClibPrintf("Source image not dound, path %s\n", SRC_IMAGE_PATH);
		abort();
	}
	sceIoRead(fd, srcMem, TEST_IMAGE_WIDTH * TEST_IMAGE_HEIGHT * 4);
	sceIoClose(fd);

	// Map for direct access from Venezia
	sceClibPrintf("Map for direct access from Venezia\n");

	err = vnzBridgeMapMemory(srcMem, srcSize, &srcPhysmem, 0);
	sceClibPrintf("vnzBridgeMapMemory: 0x%X\n", err);
	sceClibPrintf("Memory paddr: 0x%X\n", srcPhysmem);

	// Allocate memory block
	sceClibPrintf("Allocate memory block for stb_dxt constants\n");

	stbMb = sceKernelAllocMemBlock("SceVeneziaTestStb", SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_NC_RW, stbSize, NULL);
	sceClibPrintf("sceKernelAllocMemBlock: 0x%X\n", stbMb);
	sceKernelGetMemBlockBase(stbMb, &stbMem);

	StbDxtConstData *cst = (StbDxtConstData *)stbMem;
	sceClibMemcpy(cst->stb__OMatch5, stb__OMatch5, sizeof(stb__OMatch5));
	sceClibMemcpy(cst->stb__OMatch6, stb__OMatch6, sizeof(stb__OMatch6));
	sceClibMemcpy(cst->stb__midpoints5, stb__midpoints5, sizeof(stb__midpoints5));
	sceClibMemcpy(cst->stb__midpoints6, stb__midpoints6, sizeof(stb__midpoints6));

	// Map for direct access from Venezia
	sceClibPrintf("Map for direct access from Venezia\n");

	err = vnzBridgeMapMemory(stbMem, stbSize, &stbPhysmem, 0);
	sceClibPrintf("vnzBridgeMapMemory: 0x%X\n", err);
	sceClibPrintf("Memory paddr: 0x%X\n", stbPhysmem);

	sceClibPrintf("-------------------- END: memory mapping test --------------------\n\n");

	sceClibPrintf("-------------------- START: Write DDS info --------------------\n");

	char* fourcc[5];
	FILE* output;

	if (!IS_SIZE_ALIGNED(TEST_IMAGE_WIDTH, 4)) {
		sceClibPrintf("Image width is not aligned by 4!\n");
		abort();
	}

	if (!IS_SIZE_ALIGNED(TEST_IMAGE_HEIGHT, 4)) {
		sceClibPrintf("Image height is not aligned by 4!\n");
		abort();
	}

	output = fopen("ux0:data/VNZ_TEST.dds", "wb");

	switch (DXT_FORMAT) {
	case FORMAT_DXT1:
		sceClibStrncpy(fourcc, "DXT1", 5);
		break;
	case FORMAT_DXT2:
		sceClibStrncpy(fourcc, "DXT2", 5);
		break;
	case FORMAT_DXT3:
		sceClibStrncpy(fourcc, "DXT3", 5);
		break;
	case FORMAT_DXT4:
		sceClibStrncpy(fourcc, "DXT4", 5);
		break;
	case FORMAT_DXT5:
		sceClibStrncpy(fourcc, "DXT5", 5);
		break;
	}

	fileWriteString(output, "DDS ");
	fileWrite32(output, 124);
	fileWrite32(output, 0x1007);
	fileWrite32(output, TEST_IMAGE_HEIGHT);
	fileWrite32(output, TEST_IMAGE_WIDTH);
	fileWritePadding(output, 14);
	fileWrite32(output, 32);
	fileWrite32(output, 0x04);
	fileWriteString(output, fourcc);
	fileWritePadding(output, 5);
	fileWrite32(output, 0x1000);
	fileWritePadding(output, 4);

	sceClibPrintf("-------------------- END: Write DDS info --------------------\n\n");

	sceClibPrintf("-------------------- START: Venezia test --------------------\n");

	char blank[sizeof(int) * (VTHREAD_NUM + 1)];
	int state = 0;
	int status = 0;
	SceUInt64 waitStartUsec = 0;
	SceUInt64 currentTimeMeasureUsec = 0;
	SceUInt64 oldTimeMeasureUsec = 0;
	SceUInt64 waitEndUsec = 0;
	SceCodecEnginePmonProcessorLoadExt perf;

	//Reset spram area that we will use to communicate with Venezia
	sceClibPrintf("Reset spram area that we will use to communicate with Venezia\n");

	sceClibMemset(blank, 0, sizeof(blank));
	err = vnzBridgeMemcpyToSpram(blank, sizeof(blank), SPRAM_SAFE_OFFSET);
	sceClibPrintf("vnzBridgeMemcpyToSpram: 0x%X\n", err);

	// Inject custom MeP code
	sceClibPrintf("Inject custom MeP code\n");

	err = vnzBridgeInject(NULL, 0, mep_code, sizeof(mep_code));
	sceClibPrintf("vnzBridgeInject: 0x%X\n", err);

	// Spawn V-Threads
	sceClibPrintf("Spawn V-Threads, total num: %d\n", VTHREAD_NUM);

	vnzBridgeSetVeneziaExecClockFrequency(333);
	sceCodecEngineChangeNumWorkerCoresMax();

	dxtArg.src = srcPhysmem;
	dxtArg.dst = dstPhysmem;
	dxtArg.stbDxtData = stbPhysmem;
	dxtArg.srcMemSize = srcSize;
	dxtArg.dstMemSize = dstSize;
	dxtArg.format = DXT_FORMAT;
	dxtArg.width = TEST_IMAGE_WIDTH;
	dxtArg.height = TEST_IMAGE_HEIGHT;

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
	if (currentTimeMeasureUsec - oldTimeMeasureUsec > 2000000) {
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

	vnzBridgeUnmapMemory(dstMem, dstSize, 1);
	vnzBridgeUnmapMemory(srcMem, srcSize, 0);
	vnzBridgeUnmapMemory(stbMem, stbSize, 0);

	fwrite(dstMem, dstSize, 1, output);
	fclose(output);

	vnzBridgeRestore();

	return 0;
}