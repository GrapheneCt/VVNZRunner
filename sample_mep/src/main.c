#include "vthread.h"
#include "memory.h"

#define VENEZIA_SPRAM_ADDR		0xF1840000
#define SPRAM_MEMSIZE			0x2500000
#define SPRAM_SAFE_OFFSET		0x1404
#define SPRAM_VTHREAD_MAILBOX_OFFSET	0x1408
#define SPRAM_USE_BASE			(VENEZIA_SPRAM_ADDR + SPRAM_SAFE_OFFSET)
#define SPRAM_MAILBOX_BASE		(VENEZIA_SPRAM_ADDR + SPRAM_VTHREAD_MAILBOX_OFFSET)

#define VTHREAD_STATE_READY		1
#define VTHREAD_STATE_FINISHED	2

#define STB_DXT_VNZ_CONST_STORAGE	SPRAM_USE_BASE
#define STB_DXT_IMPLEMENTATION
#include "stb_dxt_vnz.h"

void *vnzMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset);
void *vnzMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset);
//float powf_c(float x, float n);

#define DXT_FLAGS (STB_DXT_DITHER | STB_DXT_HIGHQUAL)
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

typedef enum DDSFormat {
	FORMAT_DXT1,
	FORMAT_DXT2,
	FORMAT_DXT3,
	FORMAT_DXT4,
	FORMAT_DXT5
} DDSFormat;

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

void myEntry(void *arg)
{
	VnzMemory memIn;
	VnzMemory memOut;
	VnzMemory memStb;
	DxtCompressionArg *dxtArg = *(DxtCompressionArg **)(SPRAM_MAILBOX_BASE);

	unsigned char* input = dxtArg->src;
	void *output = dxtArg->dst;

	vnzMemoryCreate(&memOut, output, dxtArg->dstMemSize);
	vnzMemoryOpenPrivate(&memOut);

	vnzMemoryCreate(&memIn, input, dxtArg->srcMemSize);
	vnzMemoryOpenProtected(&memIn);

	vnzMemoryCreate(&memStb, dxtArg->stbDxtData, 4 * 1024);
	vnzMemoryOpenProtected(&memStb);

	output = memOut.vaddrVnz;
	input = memIn.vaddrVnz;
	*(void **)(SPRAM_USE_BASE) = memStb.vaddrVnz;

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
				break;*/
			case FORMAT_DXT4:
			case FORMAT_DXT5:
				stb_compress_bc4_block(chunk, alpha);
				stb_compress_dxt_block(chunk + 8, block, 0, DXT_FLAGS);
				break;
			}
			memcpy(output, chunk, chunkSize);
			output += chunkSize;
		}
	}

	vnzMemoryClosePrivate(&memOut);
	vnzMemoryCloseProtected(&memIn);
	vnzMemoryCloseProtected(&memStb);

	return;
}

int main(void *pVThreadProcessingResource, void *pUserArg)
{
	int ret = 0;
	VnzVThreadContext thrdCtx;

	*(void **)(SPRAM_MAILBOX_BASE) = pUserArg;

	thrdCtx = vnzVThreadCreateContext(1, pVThreadProcessingResource);

	if (thrdCtx != 0) {
		ret = vnzVThreadExecute(thrdCtx, 0, myEntry, 0, 0, 0, 0);
		vnzVThreadDeleteContext(thrdCtx);
	}

	*(int *)(SPRAM_MAILBOX_BASE) = VTHREAD_STATE_FINISHED;

	return 0;
}

void *vnzMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset)
{
	if (spramOffset > SPRAM_MEMSIZE)
		return 0;

	return memcpy(VENEZIA_SPRAM_ADDR + spramOffset, src, size);
}

void *vnzMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset)
{
	if (spramOffset > SPRAM_MEMSIZE)
		return 0;

	return memcpy(dst, VENEZIA_SPRAM_ADDR + spramOffset, size);
}

void * memcpy(void *s1, const void *s2, unsigned int n)
{
	char * dest = (char *)s1;
	const char * src = (const char *)s2;

	while (n--)
	{
		*dest++ = *src++;
	}

	return s1;
}