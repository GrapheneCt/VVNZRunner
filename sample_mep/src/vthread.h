#ifndef _VENEZIA_VTHREAD_H
#define _VENEZIA_VTHREAD_H

typedef int VnzVThreadContext;
typedef int(*VnzVThreadEntry)(void *argp);

VnzVThreadContext vnzVThreadCreateContext(unsigned int priority, void *pVThreadProcessingResource);
int vnzVThreadExecute(VnzVThreadContext context, int unk_a2, VnzVThreadEntry entry, void *arg1, void *arg2, void *arg3, void *arg4);
int vnzVThreadDeleteContext(VnzVThreadContext context);
//int vnzRunOnVThread(void *pVThreadProcessingResource, VnzVThreadEntry entry);

int __attribute__((noinline, optimize(1))) vnzVThreadCreateContext(unsigned int priority, void *pVThreadProcessingResource)
{
	asm volatile("jmp 0x8073D0 \n");
}

int __attribute__((noinline, optimize(1))) vnzVThreadExecute(VnzVThreadContext context, int unk_a2, VnzVThreadEntry entry, void *arg1, void *arg2, void *arg3, void *arg4)
{
	asm volatile("jmp 0x85FD6A \n");
}

int __attribute__((noinline, optimize(1))) vnzVThreadDeleteContext(VnzVThreadContext context)
{
	asm volatile("jmp 0x80767E \n");
}

#endif /* _VENEZIA_VTHREAD_H */
