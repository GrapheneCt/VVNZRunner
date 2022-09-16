#pragma once

int freeCdramMemblock(void *vaddr);
void *allocCdramMemblock(unsigned int size);

int loadMepElf(const char *path, void **vaddr, unsigned int *mbSize);
int unloadMepElf(void *vaddr);