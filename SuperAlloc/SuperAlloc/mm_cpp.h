// Combines the 'super allocator' of our chosing with mm and provides a C++ friendly interface
#pragma once

extern "C" {
	#include "mm.h"
	#include "mm_super.h"
}

class HeapHandle {
public:
	HeapHandle() {
		if (!s_refCount) {
			mem_init_super_alloc();
			mm_sub_init();
		}
		s_refCount += 1;
	}
	~HeapHandle() {
		s_refCount -= 1;
		if (!s_refCount)
			mem_destroy_super();
	}
private:
	static unsigned int s_refCount;
};

/* static */ unsigned int HeapHandle::s_refCount = 0;

class MyAllocator {
public:
	MyAllocator() {	}
	template<typename T> T* Malloc() {
		return (T*)mm_malloc(sizeof(T));
	}
	template<typename T> void Free(T* obj) {
		mm_free(obj);
	}
private:
	HeapHandle m_heapHandle;
};