#ifndef _VENEZIA_MEMORY_H
#define _VENEZIA_MEMORY_H

#include <stdbool.h>

#include "common.h"

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

static inline void vnzMemoryCreate(VnzMemory *memory, void *paddr, unsigned int size)
{
	void(*_vnzMemoryCreate)(VnzMemory *memory, void *paddr, unsigned int size);
	_vnzMemoryCreate = GET_IMAGE_OFFSET(0x5FC14);
	_vnzMemoryCreate(memory, paddr, size);
}

static inline void vnzMemoryOpenPublic(VnzMemory *memory)
{
	void(*_vnzMemoryOpenPublic)(VnzMemory *memory);
	_vnzMemoryOpenPublic = GET_IMAGE_OFFSET(0x5FC22);
	_vnzMemoryOpenPublic(memory);
}

static inline void vnzMemoryClosePublic(VnzMemory *memory)
{
	void(*_vnzMemoryClosePublic)(VnzMemory *memory);
	_vnzMemoryClosePublic = GET_IMAGE_OFFSET(0x5FC52);
	_vnzMemoryClosePublic(memory);
}

static inline void vnzMemoryOpenPrivate(VnzMemory *memory)
{
	void(*_vnzMemoryOpenPrivate)(VnzMemory *memory);
	_vnzMemoryOpenPrivate = GET_IMAGE_OFFSET(0x5FC82);
	_vnzMemoryOpenPrivate(memory);
}

static inline void vnzMemoryClosePrivate(VnzMemory *memory)
{
	void(*_vnzMemoryClosePrivate)(VnzMemory *memory);
	_vnzMemoryClosePrivate = GET_IMAGE_OFFSET(0x5FCBC);
	_vnzMemoryClosePrivate(memory);
}

static inline void vnzMemoryOpenProtected(VnzMemory *memory)
{
	void(*_vnzMemoryOpenProtected)(VnzMemory *memory);
	_vnzMemoryOpenProtected = GET_IMAGE_OFFSET(0x5FCF6);
	_vnzMemoryOpenProtected(memory);
}

static inline void vnzMemoryCloseProtected(VnzMemory *memory)
{
	void(*_vnzMemoryCloseProtected)(VnzMemory *memory);
	_vnzMemoryCloseProtected = GET_IMAGE_OFFSET(0x5FD30);
	_vnzMemoryCloseProtected(memory);
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

static inline int vnzExtMemoryCreate(VnzExtMemory *memory)
{
	int(*_vnzExtMemoryCreate)(VnzExtMemory *memory);
	_vnzExtMemoryCreate = GET_IMAGE_OFFSET(0x11ED0);
	return _vnzExtMemoryCreate(memory);
}

static inline int vnzExtMemoryOpen(VnzExtMemory *memory, bool isManagedMemory, unsigned int mode, void *addr, unsigned int size)
{
	int(*_vnzExtMemoryOpen)(VnzExtMemory *memory, bool isManagedMemory, unsigned int mode, void *addr, unsigned int size);
	_vnzExtMemoryOpen = GET_IMAGE_OFFSET(0x11EEC);
	return _vnzExtMemoryOpen(memory, isManagedMemory, mode, addr, size);
}

static inline int vnzExtMemoryClose(VnzExtMemory *memory, void **addr)
{
	int(*_vnzExtMemoryClose)(VnzExtMemory *memory, void **addr);
	_vnzExtMemoryClose = GET_IMAGE_OFFSET(0x11FAE);
	return _vnzExtMemoryClose(memory, addr);
}

/*static inline void *mallocMaybe(void *mem, unsigned int size)
{
	asm volatile("jmp 0x811e02 \n");
}*/

#endif /* _VENEZIA_MEMORY_H */
