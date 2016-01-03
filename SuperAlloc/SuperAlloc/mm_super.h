/* Super-allocator settings */
#define MAX_HEAP 100000000 // 100 MB Max Heap
#define SYSTEM_MALLOC malloc // System specific malloc call
#define SYSTEM_FREE free

// Super-allocator
void* mem_heap_hi();
void* mem_heap_lo();
void* mem_sbrk(int incr);
void mem_init_super_alloc();
void mem_destroy_super();