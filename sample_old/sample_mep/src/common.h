#ifndef _COMMON_H
#define _COMMON_H

#define VNZ_CMD_NONE			0
#define VNZ_CMD_DEBUG_FONT_DRAW	1

typedef struct VnzCallArg {
	unsigned int cmd;
	unsigned int width;
	unsigned int height;
	void *addr;
	int x;
	int y;
	int lastError;
	char text[400];
} VnzCallArg;

#endif /* _COMMON_H */