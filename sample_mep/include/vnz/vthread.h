#ifndef _VENEZIA_VTHREAD_H
#define _VENEZIA_VTHREAD_H

#include "common.h"

typedef int VnzVThreadContext;
typedef int(*VnzVThreadEntry)(void *argp);

//int vnzRunOnVThread(void *pVThreadProcessingResource, VnzVThreadEntry entry);

static inline int vnzVThreadCreateContext(unsigned int priority, void *pVThreadProcessingResource)
{
	int(*_vnzVThreadCreateContext)(unsigned int priority, void *pVThreadProcessingResource);
	_vnzVThreadCreateContext = GET_IMAGE_OFFSET(0x73D0);
	return _vnzVThreadCreateContext(priority, pVThreadProcessingResource);
}

static inline int vnzVThreadExecute(VnzVThreadContext context, int unk_a2, VnzVThreadEntry entry, void *arg1, void *arg2, void *arg3, void *arg4)
{
	int(*_vnzVThreadExecute)(VnzVThreadContext context, int unk_a2, VnzVThreadEntry entry, void *arg1, void *arg2, void *arg3, void *arg4);
	_vnzVThreadExecute = GET_IMAGE_OFFSET(0x5FD6A);
	return _vnzVThreadExecute(context, unk_a2, entry, arg1, arg2, arg3, arg4);
}

static inline int vnzVThreadDeleteContext(VnzVThreadContext context)
{
	int(*_vnzVThreadDeleteContext)(VnzVThreadContext context);
	_vnzVThreadDeleteContext = GET_IMAGE_OFFSET(0x767E);
	return _vnzVThreadDeleteContext(context);
}

#endif /* _VENEZIA_VTHREAD_H */
