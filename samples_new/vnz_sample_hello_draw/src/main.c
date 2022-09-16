#include <vnz/common.h>
#include <vnz/vthread.h>
#include <vnz/memory.h>
#include <vnz/debug.h>
#include <vnz/errno.h>

#include <string.h>
#include <string_cop.h>

#include "common.h"
#include "sce_font.h"

#define RGBA8(r,g,b,a) ((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))

//const SceCtrlData s_ctlData;

int s_fontX = 16;
int s_fontY = 16;

int drawFont(void *addr, int x, int y, const char *string, unsigned int textColor, unsigned int bgColor)
{
	char ch;
	unsigned int *base;

	ch = *string++;
	while (0 != ch) {
		if (x + 32 >= FB_WIDTH) {
			return 0; // frame overflow
		}

		base = (unsigned int *)addr + x + y * FB_WIDTH;

		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 8; j++) {
				if ((((char *)GET_SYMBOL_ADDR(sce_font_data))[(unsigned int)ch * 16 + i] & (1 << j))) {
					base[j] = textColor;
				}
				else {
					base[j] = bgColor;
				}
			}

			base += FB_WIDTH;
		}

		x += 8;

		ch = *string++;
	}

	return 0;
}

int renderThread(int a1, int a2, int a3, int a4)
{
	VnzMemory fbMem;
	void *fbMemPtr;

	VnzRenderArg *callArg = (VnzRenderArg *)a1;
	if (!callArg) {
		return 0;
	}

	if (!callArg->fbmem[callArg->fbidx]) {
		return 0;
	}

	vnzMemoryCreate(&fbMem, callArg->fbmem[callArg->fbidx], FB_WIDTH * FB_HEIGHT * 4);
	fbMemPtr = vnzMemoryOpenPrivate(&fbMem);

	memzero32(fbMemPtr, (FB_WIDTH * FB_HEIGHT * 4) / 32);

	unsigned int button = callArg->ctrl[0].buttons;
	unsigned int bgColor = RGBA8(0, 0, 0, 255);

	if ((button & SCE_CTRL_RIGHT) == SCE_CTRL_RIGHT)
		*(int *)GET_SYMBOL_ADDR(s_fontX) += 1;
	else if ((button & SCE_CTRL_LEFT) == SCE_CTRL_LEFT)
		*(int *)GET_SYMBOL_ADDR(s_fontX) -= 1;
	else if ((button & SCE_CTRL_UP) == SCE_CTRL_UP)
		*(int *)GET_SYMBOL_ADDR(s_fontY) -= 1;
	else if ((button & SCE_CTRL_DOWN) == SCE_CTRL_DOWN)
		*(int *)GET_SYMBOL_ADDR(s_fontY) += 1;

	if ((button & SCE_CTRL_CROSS) == SCE_CTRL_CROSS)
		bgColor = RGBA8(255, 0, 0, 255);

	drawFont(
		fbMemPtr,
		*(int *)GET_SYMBOL_ADDR(s_fontX),
		*(int *)GET_SYMBOL_ADDR(s_fontY),
		GET_SYMBOL_ADDR("Use DPAD to move me. Hold X to make my background red."),
		RGBA8(255, 255, 255, 255),
		bgColor);

	vnzMemoryClosePrivate(&fbMem);

	//callArg->fbidx ^= 1;

	return 0;
}

int main(void *pVThreadProcessingResource, void *pUserArg, unsigned int userArgSize)
{
	if (!pUserArg)
		return SCE_ERROR_ERRNO_EINVAL;

	int cmd = *(int *)pUserArg;

	if (cmd == VNZ_CMD_RENDER) {
		VnzVThreadContext thrdCtx = 0;

		thrdCtx = vnzVThreadCreateContext(1, pVThreadProcessingResource);

		if (thrdCtx != 0) {
			vnzVThreadExecute(thrdCtx, 0, GET_SYMBOL_ADDR(renderThread), (int)pUserArg, (int)userArgSize, 0, 0);
			vnzVThreadDeleteContext(thrdCtx);
		}
		else
			return SCE_ERROR_ERRNO_EBUSY;

		return 0;
	}

	return SCE_ERROR_ERRNO_EINVAL;
}