#include <stdlib.h>
#include <scetypes.h>
#include <stddef.h>
#include <kernel.h>
#include <kernel/sysmem.h>

#include "perf.h"

int sceCodecEnginePmonGetProcessorLoadExt(SceCodecEnginePmonProcessorLoadExt *data)
{
	SceCodecEnginePmonProcessorLoadExt kdata;
	int ret = _sceVeneziaGetProcessorLoad(&kdata);

	sceKernelCopyToUser(data, &kdata, sizeof(SceCodecEnginePmonProcessorLoadExt));

	return ret;
}