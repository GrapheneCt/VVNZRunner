#ifndef _VENEZIA_MEMORY_H
#define _VENEZIA_MEMORY_H

#include <stdbool.h>

/*

The PRIVATE state
In this state, only a task that has changed a memory region to
this state is permitted to perform a read/write access
by the L1C/L2C. This access is achieved by accessing a
virtual address that is mapped in a physical memory by the
L1 CFL2C.

The PROTECTED state
In this state, one or more tasks that belong to the
same process and that have requested a transition to this state
are permitted to perform a read access by the L1C/L2C. A task
is not permitted to perform a write access to a memory region
in this state.

The PUBLIC state
In this state, tasks of which mapping set by the ATU
is the same, that is, tasks that belong to the same process,
share data. However, a task cannot directly access a memory region
in this state. Only tasks that belong to the same process
are permitted to access, after changing a memory in this state
to the PRIVATE state or the PROTECTED state

The UNMANAGED state
This state is a state that is not managed by the kernel.
That is, this is a portion excluding the kernel management region
out of memory regions that the memory has. Because the kernel
does not manage consistency of caches in a memory region in this state,
a task must hold consistency of the caches by taking responsibility,
when a task uses this memory region.

*/

/* Simple memory management */

typedef struct VnzMemory {
	void *paddr;
	unsigned int size;
	void *vaddrVnz;
} VnzMemory;

void vnzMemoryCreate(VnzMemory *memory, void *paddr, unsigned int size);
void vnzMemoryOpenPublic(VnzMemory *memory);
void vnzMemoryClosePublic(VnzMemory *memory);
void vnzMemoryOpenPrivate(VnzMemory *memory);
void vnzMemoryClosePrivate(VnzMemory *memory);
void vnzMemoryOpenProtected(VnzMemory *memory);
void vnzMemoryCloseProtected(VnzMemory *memory);


__attribute__((noinline, optimize(1))) void vnzMemoryCreate(VnzMemory *memory, void *paddr, unsigned int size)
{
	asm volatile("jmp 0x85FC14 \n");
}

__attribute__((noinline, optimize(1))) void vnzMemoryOpenPublic(VnzMemory *memory)
{
	asm volatile("jmp 0x85FC22 \n");
}

__attribute__((noinline, optimize(1))) void vnzMemoryClosePublic(VnzMemory *memory)
{
	asm volatile("jmp 0x85FC52 \n");
}

__attribute__((noinline, optimize(1))) void vnzMemoryOpenPrivate(VnzMemory *memory)
{
	asm volatile("jmp 0x85FC82 \n");
}

__attribute__((noinline, optimize(1))) void vnzMemoryClosePrivate(VnzMemory *memory)
{
	asm volatile("jmp 0x85FCBC \n");
}

__attribute__((noinline, optimize(1))) void vnzMemoryOpenProtected(VnzMemory *memory)
{
	asm volatile("jmp 0x85FCF6 \n");
}

__attribute__((noinline, optimize(1))) void vnzMemoryCloseProtected(VnzMemory *memory)
{
	asm volatile("jmp 0x85FD30 \n");
}

/* Ext memory management */

#define VNZ_MEMORY_MODE_PROTECTED			0x100
#define VNZ_MEMORY_MODE_PRIVATE				0x10
#define VNZ_MEMORY_MODE_PUBLIC				0x1

typedef struct VnzExtMemory {
	void *paddr;
	void *vaddrManagedHeader;
	void *vaddrPublicVnz;
	void *vaddrPrivateVnz;
	void *vaddrProtectedVnz;
	unsigned int size;
} VnzExtMemory;

int vnzExtMemoryCreate(VnzExtMemory *memory);
int vnzExtMemoryOpen(VnzExtMemory *memory, bool isManagedMemory, unsigned int mode, void *addr, unsigned int size);
int vnzExtMemoryClose(VnzExtMemory *memory, void **addr);

__attribute__((noinline, optimize(1))) int vnzExtMemoryCreate(VnzExtMemory *memory)
{
	asm volatile("jmp 0x811ED0 \n");
}

__attribute__((noinline, optimize(1))) int vnzExtMemoryOpen(VnzExtMemory *memory, bool isManagedMemory, unsigned int mode, void *addr, unsigned int size)
{
	asm volatile("jmp 0x811EEC \n");
}

__attribute__((noinline, optimize(1))) int vnzExtMemoryClose(VnzExtMemory *memory, void **addr)
{
	asm volatile("jmp 0x811FAE \n");
}

/*__attribute__((noinline, optimize(1))) void *mallocMaybe(void *mem, unsigned int size)
{
	asm volatile("jmp 0x811e02 \n");
}*/

#endif /* _VENEZIA_MEMORY_H */
