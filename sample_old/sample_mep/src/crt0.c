#include <vnz/common.h>
#include <vnz/vthread.h>
#include <vnz/memory.h>
#include <vnz/debug.h>
#include <vnz/errno.h>

#include <string.h>

#pragma GCC coprocessor available $c0...$c31
#pragma GCC coprocessor call_saved $c6...$c7

#define COMM_HEAP_OFFSET		256

extern int main(void *pVThreadProcessingResource, void *pUserArg, unsigned int userArgSize);

int _start(void *pVThreadProcessingResource, void *pUserArg, unsigned int userArgSize, SceVeneziaCommParam *pCommParam)
{
	int ret = 0;

	VnzMemory argMem;
	vnzMemoryCreate(&argMem, (void *)pCommParam, sizeof(SceVeneziaCommParam) + userArgSize);

	void *pArgMem = vnzMemoryOpenPublic(&argMem);
	if (!pArgMem) {
		return SCE_ERROR_ERRNO_EINVAL;
	}

	GET_COMM_PARAM = pArgMem;

	ret = main(pVThreadProcessingResource, userArgSize ? pArgMem + sizeof(SceVeneziaCommParam) : NULL, userArgSize);

	vnzMemoryClosePublic(&argMem);

	return ret;
}