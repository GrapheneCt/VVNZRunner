#ifndef _VENEZIA_MEMORY_H
#define _VENEZIA_MEMORY_H

typedef struct VnzMemoryRange {
	void *paddr;
	unsigned int size;
	void *paddrVnz;
} VnzMemoryRange;

void vnzCreateMemoryRange(VnzMemoryRange *range, void *paddr, unsigned int size);
void *vnzDeleteMemoryRangeWithSyncRead(VnzMemoryRange *range);
void *vnzDeleteMemoryRangeWithSyncWrite(VnzMemoryRange *range);
void *vnzMemoryRangeSyncRead(VnzMemoryRange *range);
void *vnzMemoryRangeSyncWrite(VnzMemoryRange *range);

__attribute__((noinline, optimize(1))) void vnzCreateMemoryRange(VnzMemoryRange *range, void *paddr, unsigned int size)
{
	asm volatile("jmp 0x85FC14 \n");
}

__attribute__((noinline, optimize(1))) void *vnzDeleteMemoryRangeWithSyncRead(VnzMemoryRange *range)
{
	asm volatile("jmp 0x85FD30 \n");
}

__attribute__((noinline, optimize(1))) void *vnzDeleteMemoryRangeWithSyncWrite(VnzMemoryRange *range)
{
	asm volatile("jmp 0x85FCBC \n");
}

__attribute__((noinline, optimize(1))) void *vnzMemoryRangeSyncRead(VnzMemoryRange *range)
{
	asm volatile("jmp 0x85FCF6 \n");
}

__attribute__((noinline, optimize(1))) void *vnzMemoryRangeSyncWrite(VnzMemoryRange *range)
{
	asm volatile("jmp 0x85FC82 \n");
}

/*__attribute__((noinline, optimize(1))) void *mallocMaybe(void *mem, unsigned int size)
{
	asm volatile("jmp 0x811e02 \n");
}*/

#endif /* _VENEZIA_MEMORY_H */
