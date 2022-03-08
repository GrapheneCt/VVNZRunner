#include "common.h"

int _start(void *pVThreadProcessingResource, void *pUserArg, int unused1, int unused2)
{
	int(*main)(void *pVThreadProcessingResource, void *pUserArg);
	main = *(void **)(VENEZIA_SPRAM_ADDR + SPRAM_USE_OFFSET) + *(unsigned int *)(VENEZIA_SPRAM_ADDR + SPRAM_USE_OFFSET + sizeof(int));
	return main(pVThreadProcessingResource, pUserArg);
}