/*
 * Super-Allocator for mm. This shim mimics the test harness from the orig. CS351 lab.
 * Credit: http://csapp.cs.cmu.edu/public/ics2/code/vm/malloc/memlib.c
 */

#include <stdio.h>
#include "mm_super.h"

#ifndef nullptr
#define nullptr 0
#endif

/* Private global variables */
static char *mem_heap	  = nullptr;		/* Points to first byte of heap */
static char *mem_brk	  = nullptr;      /* Points to last byte of heap plus 1 */
static char *mem_max_addr = nullptr;		/* Max legal heap addr plus 1*/

void mem_init_super_alloc() {
	if (!mem_heap) {
		mem_heap = SYSTEM_MALLOC(MAX_HEAP);
		mem_brk = mem_heap;
		mem_max_addr = mem_heap + MAX_HEAP;
#ifdef _DEBUG
		memset(mem_heap, 0xFF, MAX_HEAP);
#endif
	}
	else {
		fprintf(stderr, "ERROR: asked to reinitialize existing heap allocation\n");
	}
}

void* mem_heap_hi() {
	return mem_brk - 1;
}

void* mem_heap_lo() {
	return mem_heap;
}

void* mem_sbrk(int incr) {
	char *old_brk = mem_brk;

	if ((incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
		fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
		return (void *)-1;
	}
	mem_brk += incr;
	return (void *)old_brk;

}

void mem_destroy_super() {
	if (mem_heap) {
		SYSTEM_FREE(mem_heap);
		mem_heap = mem_brk = mem_max_addr = nullptr;
	}
} 