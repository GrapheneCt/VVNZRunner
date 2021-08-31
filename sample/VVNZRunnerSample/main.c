#include <kernel.h>
#include <codecengine.h>
#include <audiodec.h>

#include "mep_code.h"
#include "../../VVNZRunner/include/vvnzrunner.h"

#define VTHREAD_NUM				7
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

int VThreadInterface(SceSize args, void *argp)
{
	int err = vnzBridgeExec(NULL, 0);
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

int main()
{
	int err = 0;
	int zero = 0;
	int one = 1;

	sceClibPrintf("-------------------- START: memory mapping test --------------------\n");

	void *testmem;
	unsigned int physmem;
	unsigned int size = 256 * 1024;

	// Allocate memory block
	sceClibPrintf("Allocate memory block\n");

	SceUID testmb = sceKernelAllocMemBlock("SceVeneziaTest", SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_NC_RW, size, NULL);
	sceClibPrintf("sceKernelAllocMemBlock: 0x%X\n", testmb);
	sceKernelGetMemBlockBase(testmb, &testmem);

	*(int *)testmem = 0x2134;

	// Map for direct access from Venezia
	sceClibPrintf("Map for direct access from Venezia\n");

	err = vnzBridgeMapMemory(testmem, size, &physmem);
	sceClibPrintf("vnzBridgeMapMemory: 0x%X\n", err);
	sceClibPrintf("Memory paddr: 0x%X\n", physmem);

	// Unmap from direct access from Venezia
	sceClibPrintf("Unmap from direct access from Venezia\n");

	err = vnzBridgeUnmapMemory(testmem, size);
	sceClibPrintf("vnzBridgeUnmapMemory: 0x%X\n", err);

	sceClibPrintf("-------------------- END: memory mapping test --------------------\n\n");

	sceClibPrintf("-------------------- START: Venezia test --------------------\n");

	char blank[sizeof(int) * (VTHREAD_NUM + 1)];
	int state = 0;
	int status = 0;
	SceUInt64 waitStartUsec = 0;
	SceUInt64 currentTimeMeasureUsec = 0;
	SceUInt64 oldTimeMeasureUsec = 0;
	SceUInt64 waitEndUsec = 0;
	SceCodecEnginePmonProcessorLoad perf;
	perf.size = sizeof(SceCodecEnginePmonProcessorLoad);

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

	sceCodecEngineChangeNumWorkerCoresMax();

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
	sceCodecEnginePmonGetProcessorLoad(&perf);
	sceCodecEnginePmonReset();
	sceCodecEnginePmonStart();
	currentTimeMeasureUsec = sceKernelGetProcessTimeWide();
	if (currentTimeMeasureUsec - oldTimeMeasureUsec > 2000000) {
		sceClibPrintf("Venezia CPU average load: %u %%\n", perf.average);
		oldTimeMeasureUsec = currentTimeMeasureUsec;
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

	vnzBridgeRestore();

	return 0;
}

// Impl from math_neon
const float __powf_rng[2] = {
	1.442695041f,
	0.693147180f
};

const float __powf_lut[16] = {
	-2.295614848256274, 	//p0	log
	-2.470711633419806, 	//p4
	-5.686926051100417, 	//p2
	-0.165253547131978, 	//p6
	+5.175912446351073, 	//p1
	+0.844006986174912, 	//p5
	+4.584458825456749, 	//p3
	+0.014127821926000,		//p7
	0.9999999916728642,		//p0	exp
	0.04165989275009526, 	//p4
	0.5000006143673624, 	//p2
	0.0014122663401803872, 	//p6
	1.000000059694879, 		//p1
	0.008336936973260111, 	//p5
	0.16666570253074878, 	//p3
	0.00019578093328483123	//p7
};

float powf_c(float x, float n)
{
	float a, b, c, d, xx;
	int m;

	union {
		float   f;
		int 	i;
	} r;

	//extract exponent
	r.f = x;
	m = (r.i >> 23);
	m = m - 127;
	r.i = r.i - (m << 23);

	//Taylor Polynomial (Estrins)
	xx = r.f * r.f;
	a = (__powf_lut[4] * r.f) + (__powf_lut[0]);
	b = (__powf_lut[6] * r.f) + (__powf_lut[2]);
	c = (__powf_lut[5] * r.f) + (__powf_lut[1]);
	d = (__powf_lut[7] * r.f) + (__powf_lut[3]);
	a = a + b * xx;
	c = c + d * xx;
	xx = xx * xx;
	r.f = a + c * xx;

	//add exponent
	r.f = r.f + ((float)m) * __powf_rng[1];

	r.f = r.f * n;


	//Range Reduction:
	m = (int)(r.f * __powf_rng[0]);
	r.f = r.f - ((float)m) * __powf_rng[1];

	//Taylor Polynomial (Estrins)
	a = (__powf_lut[12] * r.f) + (__powf_lut[8]);
	b = (__powf_lut[14] * r.f) + (__powf_lut[10]);
	c = (__powf_lut[13] * r.f) + (__powf_lut[9]);
	d = (__powf_lut[15] * r.f) + (__powf_lut[11]);
	xx = r.f * r.f;
	a = a + b * xx;
	c = c + d * xx;
	xx = xx * xx;
	r.f = a + c * xx;

	//multiply by 2 ^ m 
	m = m << 23;
	r.i = r.i + m;

	return r.f;
}
