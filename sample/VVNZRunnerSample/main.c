#include <kernel.h>

#define SPRAM_SAFE_OFFSET	0x1400

/* C:\PSDK3v3\home\Administrator\build\ime_sample (24.08.2021 2:23:43)
   StartOffset(h): 00001000, EndOffset(h): 0000111F, Length(h): 00000120 */

unsigned char mep_code[288] = {
	0xA0, 0x6F, 0x16, 0x4D, 0x12, 0x4E, 0x1A, 0x7B, 0x0E, 0x4B, 0x21, 0xC3,
	0x84, 0xF1, 0x34, 0xC3, 0x00, 0x14, 0x3E, 0x03, 0x06, 0x43, 0x21, 0xC3,
	0x84, 0xF1, 0x34, 0xC3, 0x04, 0x14, 0x3E, 0x03, 0xFA, 0x03, 0x07, 0x42,
	0xFE, 0x03, 0x33, 0x92, 0x30, 0x01, 0x13, 0xB0, 0xE8, 0xD9, 0x06, 0x80,
	0x00, 0x00, 0x13, 0x4E, 0x17, 0x4D, 0x0F, 0x4B, 0x60, 0x6F, 0xBE, 0x10,
	0xE0, 0x6F, 0x06, 0x41, 0x21, 0xC3, 0x84, 0xF1, 0x34, 0xC3, 0x08, 0x14,
	0x07, 0x42, 0x3A, 0x02, 0x00, 0x00, 0x20, 0x6F, 0x02, 0x70, 0x80, 0x6F,
	0x1E, 0x4D, 0x1A, 0x4E, 0x1A, 0x7B, 0x16, 0x4B, 0x1E, 0x4D, 0xE0, 0x09,
	0x0E, 0x41, 0x0A, 0x42, 0x06, 0x43, 0x07, 0x43, 0x21, 0xC0, 0x50, 0x02,
	0x04, 0xC0, 0x01, 0x00, 0x03, 0x03, 0x00, 0x03, 0x07, 0xA3, 0x00, 0x53,
	0x18, 0xB0, 0x07, 0x43, 0x21, 0xC2, 0x84, 0xF1, 0x23, 0x93, 0x30, 0x01,
	0x0B, 0x43, 0x0F, 0x42, 0x1F, 0x4D, 0x90, 0x0E, 0x5B, 0xB0, 0x00, 0x03,
	0x30, 0x00, 0x1B, 0x4E, 0x1F, 0x4D, 0x17, 0x4B, 0x20, 0x4F, 0xBE, 0x10,
	0x80, 0x6F, 0x1E, 0x4D, 0x1A, 0x4E, 0x1A, 0x7B, 0x16, 0x4B, 0x1E, 0x4D,
	0xE0, 0x09, 0x0E, 0x41, 0x0A, 0x42, 0x06, 0x43, 0x07, 0x43, 0x21, 0xC0,
	0x50, 0x02, 0x04, 0xC0, 0x01, 0x00, 0x03, 0x03, 0x00, 0x03, 0x07, 0xA3,
	0x00, 0x53, 0x18, 0xB0, 0x07, 0x43, 0x21, 0xC2, 0x84, 0xF1, 0x23, 0x93,
	0x30, 0x02, 0x0B, 0x43, 0x0F, 0x41, 0x1F, 0x4D, 0x90, 0x0E, 0x11, 0xB0,
	0x00, 0x03, 0x30, 0x00, 0x1B, 0x4E, 0x1F, 0x4D, 0x17, 0x4B, 0x20, 0x4F,
	0xBE, 0x10, 0xA0, 0x6F, 0x0E, 0x41, 0x0A, 0x42, 0x06, 0x43, 0x0F, 0x43,
	0x16, 0x43, 0x0B, 0x43, 0x12, 0x43, 0x16, 0xB0, 0x17, 0x43, 0x30, 0xC2,
	0x01, 0x00, 0x16, 0x42, 0x13, 0x42, 0x20, 0xC1, 0x01, 0x00, 0x12, 0x41,
	0x2C, 0x02, 0x38, 0x02, 0x07, 0x43, 0x30, 0xC2, 0xFF, 0xFF, 0x06, 0x42,
	0xE5, 0xA3, 0x0F, 0x43, 0x30, 0x00, 0x60, 0x6F, 0x02, 0x70, 0x00, 0x00
};

int vnzBridgeInject(void *mepCodeJump, unsigned int mepCodeOffset, void *mepCode, unsigned int mepCodeSize);
unsigned int vnzBridgeGetSpramValue(unsigned int offset);
int vnzBridgeExec();
int vnzBridgeMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset);
int vnzBridgeMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset);

int main()
{
	int err = 0;
	int res = 0;

	int a1 = 123;
	int a2 = -123;

	vnzBridgeMemcpyToSpram(&a1, 4, SPRAM_SAFE_OFFSET);
	vnzBridgeMemcpyToSpram(&a2, 4, SPRAM_SAFE_OFFSET + 4);

	sceClibPrintf("a1: %d\n", a1);
	sceClibPrintf("a2: %d\n", a2);

	err = vnzBridgeInject(NULL, 0, mep_code, sizeof(mep_code));
	sceClibPrintf("vnzBridgeInject: 0x%X\n", err);
	//vnz_test_dump_image();
	err = vnzBridgeExec();
	sceClibPrintf("vnzBridgeExec: 0x%X\n", err);

	//vnz_test_dump_image();

	sceKernelDelayThread(10000);

	res = vnzBridgeGetSpramValue(0);
	sceClibPrintf("spram = 0x%X\n", res);
	res = vnzBridgeGetSpramValue(SPRAM_SAFE_OFFSET + 8);
	sceClibPrintf("a1 + a2 = 0x%X\n", res);

	return 0;
}