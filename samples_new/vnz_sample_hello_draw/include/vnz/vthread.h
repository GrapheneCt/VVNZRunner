#ifndef _VENEZIA_VTHREAD_H
#define _VENEZIA_VTHREAD_H

#include "common.h"

typedef int VnzVThreadContext;
typedef int(*VnzVThreadEntry)(int a1, int a2, int a3, int a4);

//int vnzRunOnVThread(void *pVThreadProcessingResource, VnzVThreadEntry entry);

static __attribute__((noinline)) int vnzVThreadCreateContext(unsigned int priority, void *pVThreadProcessingResource)
{
	int(*_vnzVThreadCreateContext)(unsigned int priority, void *pVThreadProcessingResource);
	_vnzVThreadCreateContext = GET_IMAGE_OFFSET(0x73D0);
	return _vnzVThreadCreateContext(priority, pVThreadProcessingResource);
}

static __attribute__((noinline)) int vnzVThreadExecute(VnzVThreadContext context, int unk_a2, VnzVThreadEntry entry, int arg1, int arg2, int arg3, int arg4)
{
	int(*_vnzVThreadExecute)(VnzVThreadContext context, int unk_a2, VnzVThreadEntry entry, int arg1, int arg2, int arg3, int arg4);
	_vnzVThreadExecute = GET_IMAGE_OFFSET(0x5FD6A);
	return _vnzVThreadExecute(context, unk_a2, entry, arg1, arg2, arg3, arg4);
}

static __attribute__((noinline)) int vnzVThreadDeleteContext(VnzVThreadContext context)
{
	int(*_vnzVThreadDeleteContext)(VnzVThreadContext context);
	_vnzVThreadDeleteContext = GET_IMAGE_OFFSET(0x767E);
	return _vnzVThreadDeleteContext(context);
}

#endif /* _VENEZIA_VTHREAD_H */
