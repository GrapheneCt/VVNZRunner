#include <vnz/common.h>

int _start(void *pVThreadProcessingResource, void *pUserArg, unsigned int userArgSize, SceVeneziaCommParam *pCommParam)
{
	int(*main)(void *pVThreadProcessingResource, void *pUserArg, unsigned int userArgSize, SceVeneziaCommParam *pCommParam);
	main = (void *)(pCommParam->requestedCodeBase) + pCommParam->requestedCodeOffset;
	return main(pVThreadProcessingResource, pUserArg, userArgSize, pCommParam);
}