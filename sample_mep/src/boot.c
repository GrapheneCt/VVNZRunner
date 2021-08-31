int main(void *pVThreadProcessingResource, void *pUserArg);

int __attribute__((noinline, optimize(1))) _start(void *pVThreadProcessingResource, void *pUserArg, int unused1, int unused2)
{
	return main(pVThreadProcessingResource, pUserArg);
}