#ifndef _VVNZRUNNER_H
#define _VVNZRUNNER_H

#include <stdint.h>
#include <scetypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Inject MeP code execution thunk in Venezia image. Only one injection per process is possible
 * Only base CDRAM address is currently supported
 *
 * @param[in] mepElfMemblockAddr - base address of the memblock holding MeP ELF
 * @param[in] mepElfMemblockSize - size of the memblock holding MeP ELF
 *
 * @return SCE_OK, <0 on error.
 */
int vnzBridgeInject(const void *mepElfMemblockAddr, unsigned int mepElfMemblockSize);

/**
 * Remove MeP code execution thunk in Venezia image
 *
 * @return SCE_OK, <0 on error.
 */
int vnzBridgeRestore();

/**
 * Execute MeP code
 * Maximum size of the user argument block is 500 bytes
 * If value of userArgSize exceeds 500, SCE_ERROR_ERRNO_E2BIG will be returned
 * Venezia may write new data to user argument block
 *
 * @param[in/out] pUserArg - pointer to user argument block passed to the MeP code
 * @param[in] userArgSize - size of the user argument block
 *
 * @return SCE_OK, <0 on error.
 */
int vnzBridgeExec(void *pUserArg, unsigned int userArgSize);

/**
 * Execute MeP code without internal process suspend safeguard
 * [WARNING] This call needs to be externally guarded with vnzLockProcessSuspend() and vnzUnlockProcessSuspend()
 * Maximum size of the user argument block is 500 bytes
 * If value of userArgSize exceeds 500, SCE_ERROR_ERRNO_E2BIG will be returned
 * Venezia may write new data to user argument block
 *
 * @param[in/out] pUserArg - pointer to user argument block passed to the MeP code
 * @param[in] userArgSize - size of the user argument block
 *
 * @return SCE_OK, <0 on error.
 */
int vnzBridgeExecNoSuspend(void *pUserArg, unsigned int userArgSize);

/**
 * Lock process suspend core. Use this function only when preparing vnzBridgeExecNoSuspend() call
 *
 * @return SCE_OK, <0 on error.
 */
int vnzLockProcessSuspend();

/**
 * Unlock process suspend core. Use this function only when finishing vnzBridgeExecNoSuspend() call
 *
 * @return SCE_OK, <0 on error.
 */
int vnzUnlockProcessSuspend();

/**
 * Get value of Venezia SPRAM at specified offset, interpreted as int
 *
 * @param[in] offset - offset in SPRAM to get the value from
 *
 * @return SPRAM value.
 */
int vnzBridgeGetSpramValue(unsigned int offset);

/**
 * Copy data block from ARM RAM to Venezia SPRAM
 *
 * @param[in] src - address of data to copy
 * @param[in] size - size of data to copy
 * @param[in] spramOffset - starting offset in SPRAM
 *
 * @return SCE_OK, <0 on error.
 */
int vnzBridgeMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset);

/**
 * Copy data block from Venezia SPRAM to ARM RAM
 *
 * @param[in] dst - address of buffer to hold copied data
 * @param[in] size - size of data to copy
 * @param[in] spramOffset - starting offset in SPRAM
 *
 * @return SCE_OK, <0 on error.
 */
int vnzBridgeMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset);

/**
 * Map memory to Venezia. To ensure cache coherence memory must be 256 bytes aligned
 *
 * @param[in] vaddr - address of memory
 * @param[in] size - size of memory
 * @param[out] vnzPaddr - mapped Venezia paddr. Do not use this value on user side, only pass as argument to MeP code
 * @param[in] isVnzWritable - 0 - map memory as RO, 1 - map memory as RW
 *
 * @return SCE_OK, <0 on error.
 */
int vnzBridgeMapMemory(void *vaddr, unsigned int size, void **vnzPaddr, int isVnzWritable);

/**
 * Unmap memory from Venezia
 *
 * @param[in] vaddr - address of memory
 * @param[in] size - size of memory
 * @param[in] isVnzWritable - must be the same value as was passed to vnzBridgeMapMemory()
 *
 * @return SCE_OK, <0 on error.
 */
int vnzBridgeUnmapMemory(void *vaddr, unsigned int size, int isVnzWritable);

/**
 * Get clock frequency of Venezia for active code execution
 *
 * @param[in] vaddr - address of memory
 * @param[in] size - size of memory
 * @param[out] vnzPaddr - mapped Venezia paddr. Do not use this value on user side, only pass as argument to MeP code
 * @param[in] isVnzWritable - 0 - map memory as RO, 1 - map memory as RW
 *
 * @return frequency value in MHz.
 */
int vnzBridgeGetVeneziaExecClockFrequency();

/**
 * Set clock frequency of Venezia for active code execution. Default value is 166 MHz
 *
 * @param[in] clock - target clock frequency
 *
 * @return SCE_OK, <0 on error.
 */
int vnzBridgeSetVeneziaExecClockFrequency(int clock);

/**
 * Allocate Venezia memory from unmapped memblock
 * Allocated memory is mapped as PUBLIC on Venezia side 
 *
 * @param[in] uid - UID of unmapped memblock obtained with sceCodecEngineOpenUnmapMemBlock()
 * @param[in] size - size of the memory to allocate
 * @param[in] alignment - alignment of the memory to allocate
 * @param[out] vnzVaddr - mapped Venezia address. Do not use this value on user side, only pass as argument to MeP code
 *
 * @return SCE_OK, <0 on error.
 */
int vnzBridgeAllocUnmapMemory(SceUID uid, SceUInt32 size, SceUInt32 alignment, void **vnzVaddr);

/**
 * Free Venezia memory from unmapped memblock
 *
 * @param[in] uid - UID of unmapped memblock obtained with sceCodecEngineOpenUnmapMemBlock()
 * @param[in] vnzVaddr - mapped Venezia address
 * @param[in] size - size of the memory
 *
 * @return SCE_OK, <0 on error.
 */
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

/**
 * Get extended Venezia core load info
 * Usage is the same as sceCodecEnginePmonGetProcessorLoad(), please see libcodecengine-Overview for more details
 *
 * @param[out] data - structure to hold core load info
 *
 * @return SCE_OK, <0 on error.
 */
int sceCodecEnginePmonGetProcessorLoadExt(SceCodecEnginePmonProcessorLoadExt *data);

#ifndef VVNZRUNNER_BUILD

int _sceCodecEngineUnmapPAtoUserVA(void *paddr, SceUIntVAddr *memory);
int _sceCodecEngineUnmapUserVAtoPA(SceUIntVAddr memory, void **paddr);
int _sceCodecEngineUnmapKernelVAtoUserVA(SceUIntVAddr kernelMemory, SceUIntVAddr *memory);
int _sceCodecEngineUnmapUserVAtoKernelVA(SceUIntVAddr memory, SceUIntVAddr *kernelMemory);
int _sceCodecEngineRegisterUnmapMemory(SceUIntVAddr memory, SceUInt32 size);
int _sceCodecEngineUnregisterUnmapMemory(SceUIntVAddr memory, SceUInt32 size);
void *_sceVeneziaOpenPublicMemory(void *paddr, SceUInt32 size);
void *_sceVeneziaClosePublicMemory(void *vnzVaddr, SceUInt32 size);

/**
 * Convert physical address within unmapped memory to user virtual address
 *
 * @param[in] paddr - physical address
 * @param[out] memory - user virtual address
 *
 * @return SCE_OK, <0 on error.
 */
#define  sceCodecEngineUnmapPAtoUserVA _sceCodecEngineUnmapPAtoUserVA

/**
 * Convert user virtual address within unmapped memory to physical address
 *
 * @param[in] memory - user virtual address
 * @param[out] paddr - physical address
 *
 * @return SCE_OK, <0 on error.
 */
#define  sceCodecEngineUnmapUserVAtoPA _sceCodecEngineUnmapUserVAtoPA

/**
 * Convert kernel virtual address within unmapped memory to user virtual address
 *
 * @param[in] kernelMemory - kernel virtual address
 * @param[out] memory - user virtual address
 *
 * @return SCE_OK, <0 on error.
 */
#define  sceCodecEngineUnmapKernelVAtoUserVA _sceCodecEngineUnmapKernelVAtoUserVA

/**
 * Convert user virtual address within unmapped memory to kernel virtual address
 *
 * @param[in] memory - user virtual address
 * @param[out] kernelMemory - kernel virtual address
 *
 * @return SCE_OK, <0 on error.
 */
#define  sceCodecEngineUnmapUserVAtoKernelVA _sceCodecEngineUnmapUserVAtoKernelVA

/**
 * Register unmapped memory allocation as used
 *
 * @param[in] memory - user virtual address
 * @param[in] size - memory size
 *
 * @return SCE_OK, <0 on error.
 */
#define  sceCodecEngineRegisterUnmapMemory _sceCodecEngineRegisterUnmapMemory

/**
 * Unregister unmapped memory allocation as used
 *
 * @param[in] memory - user virtual address
 * @param[in] size - memory size
 *
 * @return SCE_OK, <0 on error.
 */
#define  sceCodecEngineUnregisterUnmapMemory _sceCodecEngineUnregisterUnmapMemory

/**
 * Open physical memory as PUBLIC on Venezia side
 *
 * @param[in] paddr - physical address
 * @param[in] size - memory size
 *
 * @return Venezia address of opened memory.
 */
#define  sceVeneziaOpenPublicMemory _sceVeneziaOpenPublicMemory

 /**
  * Open physical memory as PUBLIC on Venezia side
  *
  * @param[in] vnzVaddr - Venezia address
  * @param[in] size - memory size
  *
  * @return SCE_OK, <0 on error.
  */
#define  sceVeneziaClosePublicMemory _sceVeneziaClosePublicMemory

#endif

#ifdef __cplusplus
}
#endif

#endif /* _VVNZRUNNER_H */
