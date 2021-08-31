#ifndef _VVNZRUNNER_H
#define _VVNZRUNNER_H

int vnzBridgeInject(void *mepCodeJump, unsigned int mepCodeOffset, void *mepCode, unsigned int mepCodeSize);
int vnzBridgeRestore();
int vnzBridgeExec(void *pUserArg, unsigned int userArgSize);

int vnzBridgeGetSpramValue(unsigned int offset);
int vnzBridgeMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset);
int vnzBridgeMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset);

int vnzBridgeMapMemory(void *vaddr, unsigned int size, void **vnzPaddr);
int vnzBridgeUnmapMemory(void *vaddr, unsigned int size);

int vnzBridgeGetVeneziaClockFrequency();
int vnzBridgeSetVeneziaClockFrequency(int clock);

#endif /* _VVNZRUNNER_H */
