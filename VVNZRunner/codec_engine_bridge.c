#include <stdlib.h>
#include <scetypes.h>
#include <stddef.h>
#include <kernel.h>
#include <kernel/sysmem.h>

#include "codec_engine_bridge.h"
#include "vnz_wrapper.h"

int sceCodecEnginePmonGetProcessorLoadExt(SceCodecEnginePmonProcessorLoadExt *data)
{
	SceCodecEnginePmonProcessorLoadExt kdata;
	int ret = _sceVeneziaGetProcessorLoad(&kdata);

	sceKernelCopyToUser(data, &kdata, sizeof(SceCodecEnginePmonProcessorLoadExt));

	return ret;
}

int vnzBridgeAllocUnmapMemory(SceUID uid, SceUInt32 size, SceUInt32 alignment, void **vnzVaddr)
{
	int ret = 0;
	void *pa = NULL;
	void *retMem = NULL;
	SceUIntVAddr memory = 0;

	if (!vnzVaddr)
		return SCE_ERROR_ERRNO_EINVAL;

	memory = _sceCodecEngineAllocMemoryFromUnmapMemBlock(uid, size, alignment);
	if (!memory)
		return SCE_ERROR_ERRNO_EINVAL;

	/*ret = sceCodecEngineRegisterUnmapMemory(SCE_KERNEL_PROCESS_ID_SELF, memory, size);
	if (ret < 0)
		goto error;*/

	ret = sceCodecEngineUnmapUserVAtoPA(SCE_KERNEL_PROCESS_ID_SELF, memory, &pa);
	if (ret < 0)
		goto error;

	retMem = sceVeneziaOpenPublicMemory(pa, size);
	if (!retMem) {
		ret = SCE_ERROR_ERRNO_EINVAL;
		goto error;
	}

	sceKernelCopyToUser(vnzVaddr, &retMem, sizeof(void *));

	return ret;

error:

	//sceCodecEngineUnregisterUnmapMemory(SCE_KERNEL_PROCESS_ID_SELF, memory, size);
	_sceCodecEngineFreeMemoryFromUnmapMemBlock(uid, memory);

	return ret;
}

int vnzBridgeFreeUnmapMemory(SceUID uid, void *vnzVaddr, SceUInt32 size)
{
	int ret = 0;
	void *pa = NULL;
	SceUIntVAddr memory = 0;

	if (!vnzVaddr || !size)
		return SCE_ERROR_ERRNO_EINVAL;

	pa = sceVeneziaClosePublicMemory(vnzVaddr, size);
	if (!pa) {
		ret = SCE_ERROR_ERRNO_EINVAL;
		return ret;
	}

	ret = sceCodecEngineUnmapPAtoUserVA(SCE_KERNEL_PROCESS_ID_SELF, pa, &memory);
	if (ret < 0)
		return ret;

	/*ret = sceCodecEngineUnregisterUnmapMemory(SCE_KERNEL_PROCESS_ID_SELF, memory, size);
	if (ret < 0)
		return ret;*/

	ret = _sceCodecEngineFreeMemoryFromUnmapMemBlock(uid, memory);

	return ret;
}

int _sceCodecEngineUnmapPAtoUserVA(void *paddr, SceUIntVAddr *memory)
{
	SceUIntVAddr res;
	int ret = sceCodecEngineUnmapPAtoUserVA(SCE_KERNEL_PROCESS_ID_SELF, paddr, &res);

	sceKernelCopyToUser(memory, &res, sizeof(SceUIntVAddr));

	return ret;
}

int _sceCodecEngineUnmapUserVAtoPA(SceUIntVAddr memory, void **paddr)
{
	void *res;
	int ret = sceCodecEngineUnmapUserVAtoPA(SCE_KERNEL_PROCESS_ID_SELF, memory, &res);

	sceKernelCopyToUser(paddr, &res, sizeof(void *));

	return ret;
}

int _sceCodecEngineUnmapKernelVAtoUserVA(SceUIntVAddr kernelMemory, SceUIntVAddr *memory)
{
	SceUIntVAddr res;
	int ret = sceCodecEngineUnmapKernelVAtoUserVA(SCE_KERNEL_PROCESS_ID_SELF, kernelMemory, &res);

	sceKernelCopyToUser(memory, &res, sizeof(SceUIntVAddr));

	return ret;
}


int _sceCodecEngineUnmapUserVAtoKernelVA(SceUIntVAddr memory, SceUIntVAddr *kernelMemory)
{
	SceUIntVAddr res;
	int ret = sceCodecEngineUnmapUserVAtoKernelVA(SCE_KERNEL_PROCESS_ID_SELF, memory, &res);

	sceKernelCopyToUser(kernelMemory, &res, sizeof(SceUIntVAddr));

	return ret;
}

int _sceCodecEngineRegisterUnmapMemory(SceUIntVAddr memory, SceUInt32 size)
{
	return sceCodecEngineRegisterUnmapMemory(SCE_KERNEL_PROCESS_ID_SELF, memory, size);
}

int _sceCodecEngineUnregisterUnmapMemory(SceUIntVAddr memory, SceUInt32 size)
{
	return sceCodecEngineUnregisterUnmapMemory(SCE_KERNEL_PROCESS_ID_SELF, memory, size);
}

void *_sceVeneziaOpenPublicMemory(void *paddr, SceUInt32 size)
{
	return sceVeneziaOpenPublicMemory(paddr, size);
}

void *_sceVeneziaClosePublicMemory(void *vnzVaddr, SceUInt32 size)
{
	return sceVeneziaClosePublicMemory(vnzVaddr, size);
}