#include <kernel.h>

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))

int freeCdramMemblock(void *vaddr)
{
	if (!vaddr)
		return SCE_ERROR_ERRNO_EINVAL;

	SceUID mbId = sceKernelFindMemBlockByAddr(vaddr, 0);
	sceKernelFreeMemBlock(mbId);

	return SCE_OK;
}

void *allocCdramMemblock(unsigned int size)
{
	void *mem = NULL;

	SceUID workMemUID = sceKernelAllocMemBlock("SceVeneziaCdramMemory", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, ALIGN(size, SCE_KERNEL_256KiB), NULL);
	sceKernelGetMemBlockBase(workMemUID, &mem);

	return mem;
}

int loadMepElf(const char *path, void **vaddr, unsigned int *mbSize)
{
	SceUInt32 mepCodeFileSize = 0;
	SceUID fd;
	int ret = SCE_OK;

	if (!path || !vaddr || !mbSize)
		return SCE_ERROR_ERRNO_EINVAL;

	fd = sceIoOpen(path, SCE_O_RDONLY, 0);
	if (fd <= 0)
		return fd;

	mepCodeFileSize = (SceUInt32)sceIoLseek(fd, 0, SCE_SEEK_END);
	sceIoLseek(fd, 0, SCE_SEEK_SET);

	*mbSize = ALIGN(mepCodeFileSize, SCE_KERNEL_256KiB);

	*vaddr = allocCdramMemblock(*mbSize);

	if (!*vaddr) {
		sceIoClose(fd);
		return SCE_ERROR_ERRNO_ENOMEM;
	}

	sceClibMemset(*vaddr, 0, *mbSize);
	sceIoRead(fd, *vaddr, mepCodeFileSize);
	sceIoClose(fd);

	return ret;
}

int unloadMepElf(void *vaddr)
{
	return freeCdramMemblock(vaddr);
}