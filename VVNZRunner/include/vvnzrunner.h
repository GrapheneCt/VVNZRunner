#ifndef _VVNZRUNNER_H
#define _VVNZRUNNER_H

int vnzBridgeInject(const void *mepElfMemblockAddr, unsigned int mepElfMemblockSize);
int vnzBridgeRestore();
int vnzBridgeExec(void *pUserArg, unsigned int userArgSize);

int vnzBridgeGetSpramValue(unsigned int offset);
int vnzBridgeMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset);
int vnzBridgeMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset);

int vnzBridgeMapMemory(void *vaddr, unsigned int size, void **vnzPaddr, int isVnzWritable);
int vnzBridgeUnmapMemory(void *vaddr, unsigned int size, int isVnzWritable);

int vnzBridgeGetVeneziaExecClockFrequency();
int vnzBridgeSetVeneziaExecClockFrequency(int clock);

typedef struct SceCodecEnginePmonProcessorLoadExt {
	int core0;
	int core1;
	int core2;
	int core3;
	int core4;
	int core5;
	int core6;
	int core7;
	int average;
	int peak;
} SceCodecEnginePmonProcessorLoadExt;

int sceCodecEnginePmonGetProcessorLoadExt(SceCodecEnginePmonProcessorLoadExt *data);

#endif /* _VVNZRUNNER_H */
