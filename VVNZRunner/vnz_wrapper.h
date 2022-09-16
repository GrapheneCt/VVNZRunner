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

typedef void* SceVeneziaRpcMemoryContext;

typedef struct SceVeneziaThunkArg {
	void *pVThreadProcessingResource;
	void *userArg2;
	void *userArg3;
	void *userArg4;
} SceVeneziaThunkArg;

typedef struct SceVeneziaConvertOpt {
	int flags;
	unsigned int size;
} SceVeneziaConvertOpt;

typedef SceVoid(*SceVeneziaRpcMemoryCommBegin)(SceVeneziaRpcMemoryContext context, SceVeneziaThunkArg *in, SceVeneziaThunkArg *out);
typedef SceVoid(*SceVeneziaRpcMemoryCommEnd)(SceVeneziaRpcMemoryContext context);

int sceVeneziaRpcCallGenericThunk(unsigned int id, SceVeneziaThunkArg *arg, SceVeneziaRpcMemoryCommBegin beginCallback, SceVeneziaRpcMemoryCommEnd endCallback);
int sceVeneziaRpcCallGenericThunk2(unsigned int id, int arg1, int arg2);  //guessed name, calls same function as sceVeneziaRpcCallGenericThunk() with different arg arrangement
void *sceVeneziaGetVThreadProcessingResource(unsigned int key);

int sceVeneziaLockProcessSuspend();
int sceVeneziaUnlockProcessSuspend();

int sceVeneziaConvertVirtualToPhysical(SceVeneziaRpcMemoryContext context, void *vaddr, unsigned int size, SceVeneziaConvertOpt *opt);
int sceVeneziaConvertPhysicalToVirtual(SceVeneziaRpcMemoryContext context);
int sceVeneziaInitRpcMemory(SceVeneziaRpcMemoryContext context); //guessed name
int sceVeneziaMemcpyChain(SceVeneziaRpcMemoryContext context);
int sceVeneziaTermRpcMemory(SceVeneziaRpcMemoryContext context); //guessed name

#define SCE_VENEZIA_HEAP_IMAGE_0	0 // First allocation paddr = 0x40963900, heap size = 0x80600
#define SCE_VENEZIA_HEAP_IMAGE		1 // First allocation paddr = 0x40A45700, heap size = 0x1BAA00
#define SCE_VENEZIA_HEAP_VRAM		2 // First allocation paddr = 0x20000000, heap size = 0x8000100

void *sceVeneziaHeapAlloc(unsigned int heapType, int size);  //guessed name
void sceVeneziaHeapFree(unsigned int heapType, void *addr, int size); //guessed name

void *sceVeneziaConvertVirtualToPhysicalForVenezia(void *vaddr); //guessed name
void *sceVeneziaConvertPhysicalToVirtualForVenezia(void *paddr); //guessed name, used to convert return of sceVeneziaHeapAlloc()

int sceCodecEngineUnmapPAtoUserVA(SceUID pid, void *paddr, SceUIntVAddr *memory);
int sceCodecEngineUnmapUserVAtoPA(SceUID pid, SceUIntVAddr memory, void **paddr);

int sceCodecEngineUnmapKernelVAtoUserVA(SceUID pid, SceUIntVAddr kernelMemory, SceUIntVAddr *memory);
int sceCodecEngineUnmapUserVAtoKernelVA(SceUID pid, SceUIntVAddr memory, SceUIntVAddr *kernelMemory);

int sceCodecEngineRegisterUnmapMemory(SceUID pid, SceUIntVAddr memory, SceUInt32 size);
int sceCodecEngineUnregisterUnmapMemory(SceUID pid, SceUIntVAddr memory, SceUInt32 size);

void *sceVeneziaOpenPublicMemory(void *paddr, SceUInt32 size); //calls Venezia function, see vnz/memory.h
void *sceVeneziaClosePublicMemory(void *vnzVaddr, SceUInt32 size); //calls Venezia function, see vnz/memory.h

#ifdef __cplusplus
}
#endif

#endif /* _VDSUITE_KERNEL_VNZ_WRAPPER_H */
