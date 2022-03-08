/*
	Vita Development Suite Libraries
*/

#ifndef _VDSUITE_KERNEL_VNZ_WRAPPER_H
#define _VDSUITE_KERNEL_VNZ_WRAPPER_H

#include <stdint.h>
#include <scetypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* SceCodecEngineWrapperRpcMemoryContext;

typedef struct SceCodecEngineWrapperThunkArg {
	void *pVThreadProcessingResource;
	void *userArg2;
	void *userArg3;
	void *userArg4;
} SceCodecEngineWrapperThunkArg;

typedef struct SceCodecEngineWrapperConvertOpt {
	int flags;
	unsigned int size;
} SceCodecEngineWrapperConvertOpt;

typedef SceVoid(*SceCodecEngineWrapperRpcMemoryCommBegin)(SceCodecEngineWrapperRpcMemoryContext context, SceCodecEngineWrapperThunkArg *arg, void *memoryRegion);
typedef SceVoid(*SceCodecEngineWrapperRpcMemoryCommEnd)(SceCodecEngineWrapperRpcMemoryContext context);

int sceCodecEngineWrapperCallGenericThunk(unsigned int id, SceCodecEngineWrapperThunkArg *arg, SceCodecEngineWrapperRpcMemoryCommBegin beginCallback, SceCodecEngineWrapperRpcMemoryCommEnd endCallback);
void *sceCodecEngineWrapperGetVThreadProcessingResource(unsigned int key);
int sceCodecEngineWrapperLockProcessSuspendCore();
int sceCodecEngineWrapperUnlockProcessSuspendCore();
int sceCodecEngineWrapperConvertVirtualToPhysical(SceCodecEngineWrapperRpcMemoryContext context, void *vaddr, unsigned int size, SceCodecEngineWrapperConvertOpt *opt);
int sceCodecEngineWrapperConvertPhysicalToVirtual(SceCodecEngineWrapperRpcMemoryContext context);
int sceCodecEngineWrapperInitRpcMemory(SceCodecEngineWrapperRpcMemoryContext context);
int sceCodecEngineWrapperMemcpyChain(SceCodecEngineWrapperRpcMemoryContext context);
int sceCodecEngineWrapperTermRpcMemory(SceCodecEngineWrapperRpcMemoryContext context);

#ifdef __cplusplus
}
#endif

#endif /* _VDSUITE_KERNEL_VNZ_WRAPPER_H */
