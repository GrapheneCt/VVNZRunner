#ifndef _VVNZRUNNER_MEMORY_H
#define _VVNZRUNNER_MEMORY_H

#include <stdint.h>
#include <scetypes.h>
#include <kernel.h>
#include <kernel/sysmem.h>
#include <kernel/sysmem/memblock.h>

int(*_sceAvcodecMapMemoryToVenezia)(void **vnzPaddr, const void *vaddr, unsigned int size, SceKernelMemoryRefPerm perm, unsigned int mode, unsigned int plsAllowVnz);

#endif /* _VVNZRUNNER_MEMORY_H */
