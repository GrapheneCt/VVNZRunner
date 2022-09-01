#ifndef _VVNZRUNNER_COMMON_H
#define _VVNZRUNNER_COMMON_H

#include <stdint.h>
#include <scetypes.h>

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))

#define IS_SIZE_ALIGNED( sizeToTest, PowerOfTwo )  \
		(((sizeToTest) & ((PowerOfTwo) - 1)) == 0)

#define VADDR_PERVASIVE2_REG	0x280E2000
#define VADDR_VENEZIA_SPRAM		0x28100000
#define VADDR_VENEZIA_IMAGE		0x28200000
#define VADDR_VENEZIA_VIP		0x28600000

#define PADDR_VENEZIA_IMAGE		0x40800000

#define PADDR_VENEZIA_SPRAM		0x1F840000
#define SPRAM_MAP_SIZE			0x20000

#define VIP_MEMSIZE				0x100000
#define IMAGE_MEMSIZE			0x400000
#define SPRAM_MEMSIZE			0x2500000

#define SPRAM_USE_OFFSET		0x1400

#define INJECT_CODE_BASE_OFFSET	0xEAE70

#define COMM_HEAP_SIZE			512
#define COMM_HEAP_OFFSET		256

#define THUNK_CMD_ID			8
#define THUNK_PTR_INJECT_ADDR	0xEADF0
#define THUNK_PTR_INJECT_DST	0x8EAE70

/*

#define SCE_SYSMEM_VENEZIA_PARAM_IMAGE 8
#define SCE_SYSMEM_VENEZIA_PARAM_SPRAM 1

typedef struct SceSysmemVeneziaImageParam {
	unsigned int size; // 0xc
	unsigned int paddr;
	unsigned int memsize;
} SceSysmemVeneziaImageParam;

*/

#endif /* _VVNZRUNNER_COMMON_H */
