#include <stdlib.h>
#include <scetypes.h>
#include <stddef.h>
#include <kernel.h>
#include <kernel/sysmem.h>
#include <kernel/sysmem/memblock.h>

#include "common.h"
#include "memory.h"

#define sceKernelDcacheInvalidateRange_1	sceKernelCpuDcacheAndL2InvalidateRange

int vnzBridgeMapMemory(void *vaddr, unsigned int size, void **vnzPaddr, int isVnzWritable)
{
	void *res = 0;
	int ret = 0;

	if (isVnzWritable)
		ret = _sceAvcodecMapMemoryToVenezia(&res, vaddr, size, SCE_KERNEL_MEMORY_REF_PERM_KERN_W, 2, 1);
	else
		ret = _sceAvcodecMapMemoryToVenezia(&res, vaddr, size, SCE_KERNEL_MEMORY_REF_PERM_KERN_R, 1, 1);

	sceKernelCopyToUser(vnzPaddr, &res, 4);

	return ret;
}

int vnzBridgeUnmapMemory(void *vaddr, unsigned int size, int isVnzWritable)
{
	unsigned int va = vaddr;
	unsigned int rangeSize = size + 0xff + (va & 0xff) & 0xffffff00;
	unsigned int rangeBase = va & 0xffffff00;
	int ret = 0;

	if (isVnzWritable) {
		sceKernelDcacheInvalidateRange_1(rangeBase, ((va + 0xff) - (va & 0xffffff00)) + size & 0xffffff00);
		ret = sceKernelMemRangeReleaseWithPerm(SCE_KERNEL_MEMORY_REF_PERM_KERN_W, rangeBase, rangeSize);
	}
	else {
		ret = sceKernelMemRangeReleaseWithPerm(SCE_KERNEL_MEMORY_REF_PERM_KERN_R, rangeBase, rangeSize);
	}

	return ret;
}

int vnzBridgeMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset)
{
	if ((spramOffset + size) > SPRAM_MEMSIZE)
		return SCE_ERROR_ERRNO_ERANGE;

	return sceKernelCopyFromUser(VADDR_VENEZIA_SPRAM + spramOffset, src, size);
}

int vnzBridgeMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset)
{
	if ((spramOffset + size) > SPRAM_MEMSIZE)
		return SCE_ERROR_ERRNO_ERANGE;

	return sceKernelCopyToUser(dst, VADDR_VENEZIA_SPRAM + spramOffset, size);
}

int vnzBridgeGetSpramValue(unsigned int offset)
{
	if (offset > (SPRAM_MEMSIZE - sizeof(unsigned int)))
		return SCE_ERROR_ERRNO_ERANGE;

	int value = *(int *)(VADDR_VENEZIA_SPRAM + offset);

	return value;
}