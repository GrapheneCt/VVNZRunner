#ifndef _VENEZIA_COMMON_H
#define _VENEZIA_COMMON_H

typedef struct SceVeneziaCommParam {
	unsigned int requestedCodeBase;
	unsigned int requestedCodeOffset;
	unsigned int requestedCodeSize;
} SceVeneziaCommParam;

#define VENEZIA_SPRAM_ADDR		0xF1840000
#define VENEZIA_SPRAM_SIZE		0x20000
#define VENEZIA_IMAGE_ADDR		0x40800000
#define SPRAM_USE_OFFSET		0x1400

#define GET_SPRAM_VALUE(x)		*(int *)(VENEZIA_SPRAM_ADDR + SPRAM_USE_OFFSET + x)
#define GET_IMAGE_OFFSET(x)		(void *)(VENEZIA_IMAGE_ADDR + x)
#define GET_EFF_ADDR(x)			(void *)(*(unsigned int *)(VENEZIA_SPRAM_ADDR + SPRAM_USE_OFFSET) + x)

#define GET_COMM_PARAM			(*(SceVeneziaCommParam **)(VENEZIA_SPRAM_ADDR + SPRAM_USE_OFFSET))
#define GET_CODE_BASE			(GET_COMM_PARAM->requestedCodeBase)
#define GET_CODE_OFFSET			(GET_COMM_PARAM->requestedCodeOffset)
#define GET_CODE_SIZE			(GET_COMM_PARAM->requestedCodeSize)

#define GET_SYMBOL_ADDR(x)		((void *)(GET_CODE_BASE + (unsigned int)(&x)))
#define GET_SYMBOL_INT(x)		(*(int *)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_PINT(x)		(*(int **)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_UINT(x)		(*(unsigned int *)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_PUINT(x)		(*(unsigned int **)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_SHORT(x)		(*(short *)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_PSHORT(x)	(*(short **)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_USHORT(x)	(*(unsigned short *)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_PUSHORT(x)	(*(unsigned short **)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_CHAR(x)		(*(char *)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_PCHAR(x)		(*(char **)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_FLOAT(x)		(*(float *)(GET_SYMBOL_ADDR(x)))
#define GET_SYMBOL_PFLOAT(x)	(*(float **)(GET_SYMBOL_ADDR(x)))

#endif /* _VENEZIA_VTHREAD_H */
