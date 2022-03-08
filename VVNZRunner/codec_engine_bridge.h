#ifndef _VVNZRUNNER_PERF_H
#define _VVNZRUNNER_PERF_H

#include <stdint.h>
#include <scetypes.h>
#include "include/vvnzrunner.h"

int(*_sceVeneziaGetProcessorLoad)(SceCodecEnginePmonProcessorLoadExt *data);
SceUIntVAddr(*_sceCodecEngineAllocMemoryFromUnmapMemBlock)(SceUID uid, SceUInt32 size, SceUInt32 alignment);
SceInt32(*_sceCodecEngineFreeMemoryFromUnmapMemBlock)(SceUID uid, SceUIntVAddr p);


#endif /* _VVNZRUNNER_PERF_H */
