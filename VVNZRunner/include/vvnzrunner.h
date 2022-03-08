#ifndef _VVNZRUNNER_H
#define _VVNZRUNNER_H

#include <stdint.h>
#include <scetypes.h>

#ifdef __cplusplus
extern "C" {
#endif

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

int vnzBridgeAllocUnmapMemory(SceUID uid, SceUInt32 size, SceUInt32 alignment, void **vnzVaddr);
int vnzBridgeFreeUnmapMemory(SceUID uid, void *vnzVaddr, SceUInt32 size);

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

int _sceCodecEngineUnmapPAtoUserVA(void *paddr, SceUIntVAddr *memory);
int _sceCodecEngineUnmapUserVAtoPA(SceUIntVAddr memory, void **paddr);
int _sceCodecEngineUnmapKernelVAtoUserVA(SceUIntVAddr kernelMemory, SceUIntVAddr *memory);
int _sceCodecEngineUnmapUserVAtoKernelVA(SceUIntVAddr memory, SceUIntVAddr *kernelMemory);
int _sceCodecEngineRegisterUnmapMemory(SceUIntVAddr memory, SceUInt32 size);
int _sceCodecEngineUnregisterUnmapMemory(SceUID pid, SceUIntVAddr memory, SceUInt32 size);
void *_sceCodecEngineWrapperOpenPublicMemory(void *paddr, SceUInt32 size);
void *_sceCodecEngineWrapperClosePublicMemory(void *vnzVaddr, SceUInt32 size);

#ifndef VVNZRUNNER_BUILD
#define  sceCodecEngineUnmapPAtoUserVA _sceCodecEngineUnmapPAtoUserVA
#define  sceCodecEngineUnmapUserVAtoPA _sceCodecEngineUnmapUserVAtoPA
#define  sceCodecEngineUnmapKernelVAtoUserVA _sceCodecEngineUnmapKernelVAtoUserVA
#define  sceCodecEngineUnmapUserVAtoKernelVA _sceCodecEngineUnmapUserVAtoKernelVA
#define  sceCodecEngineRegisterUnmapMemory _sceCodecEngineRegisterUnmapMemory
#define  sceCodecEngineUnregisterUnmapMemory _sceCodecEngineUnregisterUnmapMemory
#define  sceCodecEngineWrapperOpenPublicMemory _sceCodecEngineWrapperOpenPublicMemory
#define  sceCodecEngineWrapperClosePublicMemory _sceCodecEngineWrapperClosePublicMemory
#endif

#ifdef __cplusplus
}
#endif

#endif /* _VVNZRUNNER_H */
