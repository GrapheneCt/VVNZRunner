#include <vnz/common.h>

__attribute__((noinline)) float __mulsf3(float a1, float a2)
{
	float(*___mulsf3)(float a1, float a2);
	___mulsf3 = GET_IMAGE_OFFSET(0x124F90);
	return ___mulsf3(a1, a2);
}

__attribute__((noinline)) float __divsf3(float a1, float a2)
{
	float(*___divsf3)(float a1, float a2);
	___divsf3 = GET_IMAGE_OFFSET(0x125124);
	return ___divsf3(a1, a2);
}

__attribute__((noinline)) int __nesf2(float a1, float a2)
{
	int(*___nesf2)(float a1, float a2);
	___nesf2 = GET_IMAGE_OFFSET(0x1252DE);
	return ___nesf2(a1, a2);
}

__attribute__((noinline)) int __eqsf2(float a1, float a2)
{
	int(*___eqsf2)(float a1, float a2);
	___eqsf2 = GET_IMAGE_OFFSET(0x125324);
	return ___eqsf2(a1, a2);
}

__attribute__((noinline)) int __gtsf2(float a1, float a2)
{
	int(*___gtsf2)(float a1, float a2);
	___gtsf2 = GET_IMAGE_OFFSET(0x12536A);
	return ___gtsf2(a1, a2);
}

__attribute__((noinline)) int __gesf2(float a1, float a2)
{
	int(*___gesf2)(float a1, float a2);
	___gesf2 = GET_IMAGE_OFFSET(0x1253F0);
	return ___gesf2(a1, a2);
}

__attribute__((noinline)) int __ltsf2(float a1, float a2)
{
	int(*___ltsf2)(float a1, float a2);
	___ltsf2 = GET_IMAGE_OFFSET(0x125476);
	return ___ltsf2(a1, a2);
}

__attribute__((noinline)) int __lesf2(float a1, float a2)
{
	int(*___lesf2)(float a1, float a2);
	___lesf2 = GET_IMAGE_OFFSET(0x1254FC);
	return ___lesf2(a1, a2);
}

__attribute__((noinline)) int __fixsfsi(float a1)
{
	int(*___fixsfsi)(float a1);
	___fixsfsi = GET_IMAGE_OFFSET(0x125686);
	return ___fixsfsi(a1);
}

__attribute__((noinline)) float __floatsisf(int a1)
{
	float(*___floatsisf)(int a1);
	___floatsisf = GET_IMAGE_OFFSET(0x1256DC);
	return ___floatsisf(a1);
}