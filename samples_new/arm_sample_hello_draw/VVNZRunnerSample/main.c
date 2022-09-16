#include <kernel.h>
#include <codecengine.h>
#include <audiodec.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctrl.h>
#include <display.h>

#include "../../vvnzrunner.h"
#include "util.h"

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))

#define IS_SIZE_ALIGNED( sizeToTest, PowerOfTwo )  \
		(((sizeToTest) & ((PowerOfTwo) - 1)) == 0)

#define FB_WIDTH			960
#define FB_HEIGHT			544
#define FB_COUNT			2

#define VNZ_CMD_RENDER			1

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

typedef struct VnzRenderArg {
	int cmd;
	int fbidx;
	void *fbmem[FB_COUNT];
	SceCtrlData ctrl[2];
} VnzRenderArg;

static void *s_codeMem = NULL;
static void *s_fbMemory[FB_COUNT];

int init()
{
	int err = 0;
	unsigned int codeMemSize;

	//vnzBridgeSetVeneziaExecClockFrequency(222);

	err = loadMepElf("app0:mep_code.elf", &s_codeMem, &codeMemSize);
	if (err != SCE_OK) {
		sceClibPrintf("loadMepElf: 0x%X\n", err);
		return err;
	}

	err = vnzBridgeInject(s_codeMem, codeMemSize);
	if (err != SCE_OK) {
		sceClibPrintf("vnzBridgeInject: 0x%X\n", err);
		return err;
	}

	return err;
}

int term()
{
	int err = 0;

	err = vnzBridgeRestore();
	if (err != SCE_OK) {
		sceClibPrintf("vnzBridgeRestore: 0x%X\n", err);
		return err;
	}

	err = unloadMepElf(s_codeMem);
	if (err != SCE_OK) {
		sceClibPrintf("unloadMepElf: 0x%X\n", err);
		return err;
	}

	return err;
}

void render(void *fb1, void *fb2)
{
	VnzRenderArg arg;
	SceDisplayFrameBuf fb;
	int err = 0;

	memset(&fb, 0, sizeof(SceDisplayFrameBuf));
	fb.size = sizeof(SceDisplayFrameBuf);
	fb.pixelformat = SCE_DISPLAY_PIXEL_A8B8G8R8;
	fb.width = FB_WIDTH;
	fb.pitch = FB_WIDTH;
	fb.height = FB_HEIGHT;

	memset(&arg, 0, sizeof(VnzRenderArg));
	arg.cmd = VNZ_CMD_RENDER;
	arg.fbmem[0] = fb1;
	arg.fbmem[1] = fb2;

	//render first frame before main loop
	{
		vnzBridgeExec(&arg, sizeof(VnzRenderArg));
	}

	//main loop
	while (1) {

		sceCtrlPeekBufferPositive(0, &arg.ctrl[0], 1);

		vnzBridgeExec(&arg, sizeof(VnzRenderArg));

		fb.base = s_fbMemory[arg.fbidx];
		sceDisplaySetFrameBuf(&fb, SCE_DISPLAY_UPDATETIMING_NEXTVSYNC);

		sceDisplayWaitVblankStart();
	}
}

int main()
{
	int err = 0;
	void *vnzMem[FB_COUNT] = NULL;

	//load MeP elf and inject it
	init();

	//allocate framebuffer memory and map it to VNZ
	for (int i = 0; i < FB_COUNT; i++) {
		s_fbMemory[i] = allocCdramMemblock(FB_WIDTH * FB_HEIGHT * 4);
		err = vnzBridgeMapMemory(s_fbMemory[i], FB_WIDTH * FB_HEIGHT * 4, &vnzMem[i], 1);
		if (err != SCE_OK) {
			sceClibPrintf("vnzBridgeMapMemory: 0x%X\n", err);
		}
	}

	//render frames
	render(vnzMem[0], vnzMem[1]);

	//deallocate framebuffer memory
	for (int i = 0; i < FB_COUNT; i++) {
		err = vnzBridgeUnmapMemory(s_fbMemory[i], FB_WIDTH * FB_HEIGHT * 4, 1);
		if (err != SCE_OK) {
			sceClibPrintf("vnzBridgeUnmapMemory: 0x%X\n", err);
		}
		freeCdramMemblock(s_fbMemory[i]);
	}

	//term MeP injection
	term();
	
	return 0;
}