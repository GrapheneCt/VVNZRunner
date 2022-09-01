#ifndef _VENEZIA_DEBUG_H
#define _VENEZIA_DEBUG_H

#include <stdbool.h>

#include "common.h"

static inline void vnzDebugSetGPO(unsigned int value)
{
	*(unsigned int *)(VENEZIA_SPRAM_ADDR + SPRAM_USE_OFFSET + 4) = value;
}

#endif /* _VENEZIA_DEBUG_H */
