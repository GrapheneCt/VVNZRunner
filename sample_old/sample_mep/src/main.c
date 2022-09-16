#include <vnz/common.h>
#include <vnz/vthread.h>
#include <vnz/memory.h>
#include <vnz/debug.h>
#include <vnz/errno.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <uart.h>
#include <mempool.h>

#include "common.h"
#include "sce_font.h"

#define RGBA8(r,g,b,a) ((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))

int drawFont(void *addr, int x, int y, const char *string) {

	char ch;
	unsigned int *base;

	ch = *string++;
	while (0 != ch) {
		if (x + 32 >= 960) {
			return 0; // frame overflow
		}

		base = (unsigned int *)addr + x + y * 960;

		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 8; j++) {
				if ((((char *)GET_SYMBOL_ADDR(sce_font_data))[(unsigned int)ch * 16 + i] & (1 << j))) {
					base[j] = RGBA8(255, 255, 255, 255);
				}
				//else {
				//	base[j] = RGBA8(0, 0, 0, 255);
				//}
			}

			base += 960;
		}

		x += 8;

		ch = *string++;
	}

	return 0;
}

int execEntry(int a1, int a2, int a3, int a4)
{
	VnzMemory dispMem;

	VnzCallArg *callArg = (VnzCallArg *)a1;
	if (!callArg) {
		callArg->lastError = SCE_ERROR_ERRNO_EINVAL;
		return 0;
	}

	if (!callArg->addr) {
		callArg->lastError = SCE_ERROR_ERRNO_EINVAL;
		return 0;
	}

	vnzMemoryCreate(&dispMem, callArg->addr, callArg->width * callArg->height * 4);

	void *mappedMem = vnzMemoryOpenPrivate(&dispMem);
	if (!mappedMem) {
		callArg->lastError = SCE_ERROR_ERRNO_EINVAL;
		return 0;
	}

	drawFont(mappedMem, callArg->x, callArg->y, callArg->text);

	vnzMemoryClosePrivate(&dispMem);

	callArg->lastError = 0;

	return 0;
}

int main(void *pVThreadProcessingResource, void *pUserArg, unsigned int userArgSize)
{
	VnzVThreadContext thrdCtx = 0;

	thrdCtx = vnzVThreadCreateContext(1, pVThreadProcessingResource);

	if (thrdCtx != 0) {
		vnzVThreadExecute(thrdCtx, 0, GET_SYMBOL_ADDR(execEntry), (int)pUserArg, (int)userArgSize, 0, 0);
		vnzVThreadDeleteContext(thrdCtx);
	}
	else
		return SCE_ERROR_ERRNO_EBUSY;

	printf(GET_SYMBOL_ADDR("\n\n\n\nprintf from vnz %c\n\n\n\n\n"), 'm');

	return 0;
}