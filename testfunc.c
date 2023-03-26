#include <stdlib.h>

int abc()
{
	return 3;
}
int three()
{
	return 3;
}

int identity(int a)
{
	return a;
}

int add(int a, int b)
{
	return a + b;
}

int addsix(int a, int b, int c, int d, int e, int f)
{
	return a + b + c + d + e + f;
}

int firstarg(int a, int b, int c, int d, int e, int f)
{
	return a;
}

int secondarg(int a, int b, int c, int d, int e, int f)
{
	return b;
}
int thirdarg(int a, int b, int c, int d, int e, int f)
{
	return c;
}
int fourtharg(int a, int b, int c, int d, int e, int f)
{
	return d;
}
int fiftharg(int a, int b, int c, int d, int e, int f)
{
	return e;
}
int sixtharg(int a, int b, int c, int d, int e, int f)
{
	return f;
}

int myglobal = 11;

int *pointerTest()
{
	return &myglobal;
}

int allocfour(int **ptr)
{
	int *x = calloc(4, sizeof(int));
	x[0] = 1;
	x[1] = 12;
	x[2] = 13;
	x[3] = 14;
	*ptr = x;

	return 0;
}