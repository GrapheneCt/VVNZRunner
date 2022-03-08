#ifndef _VENEZIA_DEBUG_H
#define _VENEZIA_DEBUG_H

#include <stdbool.h>

#include "common.h"

static inline void vnzDebugSetGPO(unsigned int value)
{
	GET_SPRAM_VALUE(8) = value;
}

#endif /* _VENEZIA_DEBUG_H */
