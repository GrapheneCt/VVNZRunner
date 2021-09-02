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

#define DXT_FLAGS (STB_DXT_DITHER | STB_DXT_HIGHQUAL)
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define STB_DXT_IMPLEMENTATION
#include "stb_dxt.h"

typedef enum DDSFormat {
	FORMAT_DXT1,
	FORMAT_DXT2,
	FORMAT_DXT3,
	FORMAT_DXT4,
	FORMAT_DXT5
} DDSFormat;

//#define TEST_ARM

#define TEST_IMAGE_WIDTH		960
#define TEST_IMAGE_HEIGHT		544
#define DXT_FORMAT				FORMAT_DXT5
#define SRC_IMAGE_PATH			"app0:test.rgba"				

#define VTHREAD_NUM				1
#define PERF_PMON_GRAIN_USEC	1000
#define PERF_PRINTF_GRAIN_USEC	200000

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

static DxtCompressionArg s_dxtArg;

void myEntryArm(void *arg)
{
	DxtCompressionArg *dxtArg = &s_dxtArg;

	unsigned char* input = dxtArg->src;
	void *output = dxtArg->dst;

	for (int y = 0; y < dxtArg->height; y += 4) {
		for (int x = 0; x < dxtArg->width; x += 4) {
			unsigned char block[64];
			unsigned char alpha[16];
			for (int by = 0; by < 4; ++by) {
				for (int bx = 0; bx < 4; ++bx) {
					int ai = (by * 4) + bx;
					int bi = ai * 4;
					int xx = MIN(x + bx, dxtArg->width - 1);
					int yy = MIN(y + by, dxtArg->height - 1);
					int i = ((yy * dxtArg->width) + xx) * 4;
					block[bi + 0] = input[i + 0];
					block[bi + 1] = input[i + 1];
					block[bi + 2] = input[i + 2];
					block[bi + 3] = 0xFF;
					alpha[ai] = input[i + 3];
					if (dxtArg->format == FORMAT_DXT2 || dxtArg->format == FORMAT_DXT4) {
						float am = (float)alpha[ai] / 0xFF;
						block[bi + 0] *= am;
						block[bi + 1] *= am;
						block[bi + 2] *= am;
					}
				}
			}

			unsigned char chunk[16];
			int chunkSize = 16;
			switch (dxtArg->format) {
			/*case FORMAT_DXT1:
				stb_compress_dxt_block(chunk, block, 0, DXT_FLAGS);
				chunkSize = 8;
				break;
			case FORMAT_DXT2:
			case FORMAT_DXT3:
				for (int i = 0; i < 8; ++i) {
					unsigned char a0 = alpha[i * 2 + 0] / 17;
					unsigned char a1 = alpha[i * 2 + 1] / 17;
					chunk[i] = (a1 << 4) | a0;
				}
				stb_compress_dxt_block(chunk + 8, block, 0, DXT_FLAGS);
				break;
			case FORMAT_DXT4:*/
			case FORMAT_DXT5:
				stb_compress_bc4_block(chunk, alpha);
				stb_compress_dxt_block(chunk + 8, block, 0, DXT_FLAGS);
				break;
			}
			memcpy(output, chunk, chunkSize);
			output += chunkSize;
		}
	}

	return;
}

int VThreadInterface(SceSize args, void *argp)
{
#ifdef TEST_ARM
	myEntryArm(0);
#else
	int err = vnzBridgeExec(&s_dxtArg, sizeof(DxtCompressionArg));
	sceClibPrintf("vnzBridgeExec: 0x%X\n", err);
#endif

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
	dstSize = ALIGN(dstSize, 1 * 1024 * 1024);
	unsigned int srcSize = TEST_IMAGE_WIDTH * TEST_IMAGE_HEIGHT * 4;
	srcSize = ALIGN(srcSize, 1 * 1024 * 1024);
	unsigned int stbSize = 4 * 1024;

	// Allocate memory block
	sceClibPrintf("Allocate memory block for destination\n");

	dstMb = sceKernelAllocMemBlock("SceVeneziaTestDst", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, dstSize, NULL);
	if (dstMb <= 0) {
		sceClibPrintf("sceKernelAllocMemBlock for dstMb: 0x%X\n", dstMb);
		abort();
	}
	sceKernelGetMemBlockBase(dstMb, &dstMem);

	// Map for direct access from Venezia
	sceClibPrintf("Map for direct access from Venezia\n");

#ifndef TEST_ARM
	err = vnzBridgeMapMemory(dstMem, dstSize, &dstPhysmem, 1);
	if (err < 0) {
		sceClibPrintf("vnzBridgeMapMemory for dstMem: 0x%X\n", err);
		abort();
	}
#endif
	sceClibPrintf("Memory paddr: 0x%X\n", dstPhysmem);

	// Allocate memory block
	sceClibPrintf("Allocate memory block for source\n");

	srcMb = sceKernelAllocMemBlock("SceVeneziaTestSrc", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, srcSize, NULL);
	if (srcMb <= 0) {
		sceClibPrintf("sceKernelAllocMemBlock for srcMb: 0x%X\n", srcMb);
		abort();
	}
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

#ifndef TEST_ARM
	err = vnzBridgeMapMemory(srcMem, srcSize, &srcPhysmem, 0);
	if (err < 0) {
		sceClibPrintf("vnzBridgeMapMemory for srcMem: 0x%X\n", err);
		abort();
	}
#endif
	sceClibPrintf("Memory paddr: 0x%X\n", srcPhysmem);

	// Allocate memory block
	sceClibPrintf("Allocate memory block for stb_dxt constants\n");

	stbMb = sceKernelAllocMemBlock("SceVeneziaTestStb", SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_NC_RW, stbSize, NULL);
	if (stbMb <= 0) {
		sceClibPrintf("sceKernelAllocMemBlock for stbMb: 0x%X\n", stbMb);
		abort();
	}
	sceKernelGetMemBlockBase(stbMb, &stbMem);

	StbDxtConstData *cst = (StbDxtConstData *)stbMem;
	sceClibMemcpy(cst->stb__OMatch5, stb__OMatch5, sizeof(stb__OMatch5));
	sceClibMemcpy(cst->stb__OMatch6, stb__OMatch6, sizeof(stb__OMatch6));
	sceClibMemcpy(cst->stb__midpoints5, stb__midpoints5, sizeof(stb__midpoints5));
	sceClibMemcpy(cst->stb__midpoints6, stb__midpoints6, sizeof(stb__midpoints6));

	// Map for direct access from Venezia
	sceClibPrintf("Map for direct access from Venezia\n");

	err = vnzBridgeMapMemory(stbMem, stbSize, &stbPhysmem, 0);
	if (err < 0) {
		sceClibPrintf("vnzBridgeMapMemory for stbMem: 0x%X\n", err);
		abort();
	}
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

#ifndef TEST_ARM
	vnzBridgeSetVeneziaExecClockFrequency(333);
	sceCodecEngineChangeNumWorkerCoresMax();
#endif

#ifdef TEST_ARM
	s_dxtArg.src = srcMem;
	s_dxtArg.dst = dstMem;
#else
	s_dxtArg.src = srcPhysmem;
	s_dxtArg.dst = dstPhysmem;
#endif

	s_dxtArg.stbDxtData = stbPhysmem;
	s_dxtArg.srcMemSize = srcSize;
	s_dxtArg.dstMemSize = dstSize;
	s_dxtArg.format = DXT_FORMAT;
	s_dxtArg.width = TEST_IMAGE_WIDTH;
	s_dxtArg.height = TEST_IMAGE_HEIGHT;

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

#ifndef TEST_ARM
	vnzBridgeUnmapMemory(dstMem, dstSize, 1);
	vnzBridgeUnmapMemory(srcMem, srcSize, 0);
#endif
	vnzBridgeUnmapMemory(stbMem, stbSize, 0);

	fwrite(dstMem, dstSize, 1, output);
	fclose(output);

	vnzBridgeRestore();

	return 0;
}