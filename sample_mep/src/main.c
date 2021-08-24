#define VENEZIA_SPRAM_ADDR	0xF1840000
#define SPRAM_MEMSIZE		0x2500000
#define SPRAM_SAFE_OFFSET	0x1400

void *vnzMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset);
void *vnzMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset);
void *memcpy(void *s1, const void *s2, unsigned int n);

void _start()
{
	unsigned int a1 = 0;
	unsigned int a2 = 0x123;

	writeAnswer(a1 + a2);

	asm volatile(
		"jmp 0x80063C\n");
}

void writeAnswer(unsigned int res)
{
	vnzMemcpyToSpram(&res, 4, SPRAM_SAFE_OFFSET);
}

void *vnzMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset)
{
	if (spramOffset > SPRAM_MEMSIZE)
		return 0;

	return memcpy(VENEZIA_SPRAM_ADDR + spramOffset, src, size);
}

void *vnzMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset)
{
	if (spramOffset > SPRAM_MEMSIZE)
		return 0;

	return memcpy(dst, VENEZIA_SPRAM_ADDR + spramOffset, size);
}

void *memcpy(void *s1, const void *s2, unsigned int n)
{
	char * dest = (char *)s1;
	const char * src = (const char *)s2;

	while (n--)
	{
		*dest++ = *src++;
	}

	return s1;
}