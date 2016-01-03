// SuperAlloc.cpp : Defines the entry point for the console application.
//

#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include "mm_cpp.h"

struct POD {
	int x, y, z;
};

struct POD2 {
	int x;
};

void foo() {
	MyAllocator alloc;
	POD* p = alloc.Malloc<POD>();
	p->x = p->y = p->z = 0xFF;

	alloc.Free(p);
}

int _tmain(int argc, _TCHAR* argv[])
{
	foo();

	MyAllocator alloc;
	POD* p = alloc.Malloc<POD>();

	p->x = 1;
	p->y = 2;
	p->z = 3;

	foo();
	POD2* p2 = alloc.Malloc<POD2>();
	p2->x = 0xFFFFF;

	alloc.Free(p2);
	alloc.Free(p);
	return 0;
}

