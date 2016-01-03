// Public interface for Dustin's Malloc

void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);
void print_heap();
void mm_sub_init(void);