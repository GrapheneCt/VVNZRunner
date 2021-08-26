#define VENEZIA_SPRAM_ADDR		0xF1840000
#define SPRAM_MEMSIZE			0x2500000
#define SPRAM_SAFE_OFFSET		0x1400
#define SPRAM_USE_BASE			(VENEZIA_SPRAM_ADDR + SPRAM_SAFE_OFFSET)

void *vnzMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset);
void *vnzMemcpyFromSpram(void *dst, unsigned int size, unsigned int spramOffset);
void *memcpy(void *s1, const void *s2, unsigned int n);
void main();
void writeAnswer(float res);
float powf_c(float x, float n);

void _start()
{
	asm volatile(
		"bsr main		\n"
		".tohalt:		\n"
		"halt			\n"
		"bra .tohalt	\n"
		);
}

void main()
{
	float a1 = *(float *)(SPRAM_USE_BASE);
	float a2 = *(float *)(SPRAM_USE_BASE + 4);

	writeAnswer(powf_c(a1, a2));
}

void writeAnswer(float res)
{
	*(float *)(SPRAM_USE_BASE + 8) = res;
	//vnzMemcpyToSpram(&res, 4, SPRAM_SAFE_OFFSET);
}


// Impl from math_neon
const float __powf_rng[2] = {
	1.442695041f,
	0.693147180f
};

const float __powf_lut[16] = {
	-2.295614848256274, 	//p0	log
	-2.470711633419806, 	//p4
	-5.686926051100417, 	//p2
	-0.165253547131978, 	//p6
	+5.175912446351073, 	//p1
	+0.844006986174912, 	//p5
	+4.584458825456749, 	//p3
	+0.014127821926000,		//p7
	0.9999999916728642,		//p0	exp
	0.04165989275009526, 	//p4
	0.5000006143673624, 	//p2
	0.0014122663401803872, 	//p6
	1.000000059694879, 		//p1
	0.008336936973260111, 	//p5
	0.16666570253074878, 	//p3
	0.00019578093328483123	//p7
};

float powf_c(float x, float n)
{
	float a, b, c, d, xx;
	int m;

	union {
		float   f;
		int 	i;
	} r;

	//extract exponent
	r.f = x;
	m = (r.i >> 23);
	m = m - 127;
	r.i = r.i - (m << 23);

	//Taylor Polynomial (Estrins)
	xx = r.f * r.f;
	a = (__powf_lut[4] * r.f) + (__powf_lut[0]);
	b = (__powf_lut[6] * r.f) + (__powf_lut[2]);
	c = (__powf_lut[5] * r.f) + (__powf_lut[1]);
	d = (__powf_lut[7] * r.f) + (__powf_lut[3]);
	a = a + b * xx;
	c = c + d * xx;
	xx = xx * xx;
	r.f = a + c * xx;

	//add exponent
	r.f = r.f + ((float)m) * __powf_rng[1];

	r.f = r.f * n;


	//Range Reduction:
	m = (int)(r.f * __powf_rng[0]);
	r.f = r.f - ((float)m) * __powf_rng[1];

	//Taylor Polynomial (Estrins)
	a = (__powf_lut[12] * r.f) + (__powf_lut[8]);
	b = (__powf_lut[14] * r.f) + (__powf_lut[10]);
	c = (__powf_lut[13] * r.f) + (__powf_lut[9]);
	d = (__powf_lut[15] * r.f) + (__powf_lut[11]);
	xx = r.f * r.f;
	a = a + b * xx;
	c = c + d * xx;
	xx = xx * xx;
	r.f = a + c * xx;

	//multiply by 2 ^ m 
	m = m << 23;
	r.i = r.i + m;

	return r.f;
}

/*void *vnzMemcpyToSpram(void *src, unsigned int size, unsigned int spramOffset)
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
}*/