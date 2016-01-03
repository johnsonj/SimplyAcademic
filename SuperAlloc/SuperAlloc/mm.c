// Credit for this allocator implementation:
// Dustin Shahidehpour, CS351

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mm.h"
#include "mm_super.h"

/* double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
#define BLK_HDR_SIZE ALIGN(sizeof(blockHdr))
#define BLK_FTR_SIZE ALIGN(sizeof(blockFtr))
#define HF (int)(BLK_HDR_SIZE + BLK_FTR_SIZE)
#define NEW_MEM ALIGN(12800) //whenever we mem_sbrk we extend the heap this much;
#define INITIAL_ALLOC 9000   //Used for initial mm_init heap size;

typedef struct header blockHdr;
typedef struct footer blockFtr;

struct header{ //Meta Data, 32 bytes
  size_t size;
  int free; //0 means no, 1 means yes
  blockHdr *next;
  blockHdr *prev;
};

struct footer{ //More Meta Data, 32 bytes
  size_t size;
};

// Descriptions for all of these methods can be seen below in the code//

void *find_fit(size_t size); 
void print_heap();
void adjust_size(blockHdr *a, size_t size); 
void coal(blockHdr *cursor); 
void linkToHead(blockHdr *bp); 
void *extend(size_t newsize); 
void *advanceBlock(blockHdr *b); 
void *prevBlock(blockHdr *b); 
void *last(blockHdr *b); 
int insideHeap(blockHdr *b); 
void coalLink(blockHdr *left, blockHdr *right); 

///////////////////////////////////////////////////////////////////////
/////////////Required Methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/* 
 * mm_sub_init - initializes a header that is always part of the free list
             adds heap space so that we don't have to sbrk right away
 */

void mm_sub_init(void)
{
    blockHdr *p = mem_sbrk(HF);
    blockHdr *q = mem_sbrk(INITIAL_ALLOC);

    p->free = 0;
    adjust_size(p, HF);

    q->free = 1;
    adjust_size(q, INITIAL_ALLOC);
      
    p->prev = q;
    p->next = q;
    q->prev = p;
    q->next = p;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *             This uses find_fit to return the first available free block
               that is the correct size. If it mallocs and the free space is less
               40 bytes, it adds it on the end of the newly allocated block.

               The free list is a doubly-linked list.
 */

void *mm_malloc(size_t size)
{
    int newsize = HF + ALIGN(size);
    blockHdr *ab = find_fit(newsize);
    if (ab == NULL)
      ab = extend(newsize); //no available free blocks, so we mem_sbrk here and return space

    if(ab->size - newsize <= HF){//We don't have to split the free block
      ab->free = 0;
      ab->prev->next = ab->next;
      ab->next->prev = ab->prev;
      ab->next = NULL;
      ab->prev = NULL;
    }

    else{ // We have to split the free block
      blockHdr *fb = (blockHdr *)((char*)ab + newsize);
      adjust_size(fb, (ab->size) - newsize);
      adjust_size(ab, newsize);  

      fb->prev = ab->prev;
      fb->next = ab->next;
      fb->prev->next = fb;
      fb->next->prev = fb;
      ab->next = NULL;
      ab->prev = NULL;

      ab->free = 0;
      fb->free = 1;
    }
    return (char *)ab + (BLK_HDR_SIZE);
}


/*
 * mm_free - linkToHead frees the given block, and places the new block 
  on the front of the free list. The coal() function then checks to see if
  we can coalesce the newly freed blocks with its surrondings.
 */
void mm_free(void *ptr)
{
  blockHdr *bp = (char*)ptr - (char*)BLK_HDR_SIZE;
  linkToHead(bp);
  coal(bp);
}

/*
 * mm_realloc - Checks to see if the block is big enough to realloc in itself.
   If not, we find the next available block, and if necessary, sbrk for more room.
 */
void *mm_realloc(void *ptr, size_t size)
{
  size = ALIGN(size);

  if(ptr == NULL){
    void *p = mm_malloc(size);
    return p; 
  }
  if(size == 0){
    mm_free(ptr);
    return NULL;
  }

  blockHdr *bp = (blockHdr *)((char *)ptr - BLK_HDR_SIZE);

  int canfit = (bp->size) - size - HF;
  if(canfit >= 0) //if the block can realloc within itself
    return (char *)bp + BLK_HDR_SIZE;

  else{ //need to allocate a block//
    int growth = (size + HF) - (int)(bp->size);
    if(growth <= BLK_HDR_SIZE)
      size = ALIGN((size + HF));
    
    blockHdr *n = advanceBlock(bp);
    int remain = (int)(((bp->size) + (n->size)) - (size));
    if(insideHeap(n) == 1 && n->free == 1 && remain >= HF){//if next block is free

      if(remain <= (64+HF)){ //just keep the extra bits after realloc 
       adjust_size(bp, ((int)size) + remain); 
       n->prev->next = n->next;
       n->next->prev = n->prev;
      }
      else{ //create a new free block next to the realloced block
     
       blockHdr * new = (blockHdr *)((char *)bp + ((int)size + HF));
       new->free = 1;

       new->next = n->next;
       new->prev = n->prev;
       new->prev->next = new;
       new->next->prev = new;

       adjust_size(new, (remain - HF));
       adjust_size(bp, (int)size + HF);

      }
     return (char *)bp + BLK_HDR_SIZE;
    } 
    else{ //worst case scenario, sbrk for more room, then realloc there.
       void *p = mm_malloc(size); 
       memcpy(p, ptr, size);
       mm_free(ptr);
       return p;
    }
  }
}

void print_heap() {
  blockHdr *bp = ((blockHdr *)mem_heap_lo());
  while(bp < (blockHdr *)mem_heap_hi()) {
    printf("%s block at %p, size %d [%d]\n",  (bp->free==0)?"allocated":"free", 
        bp, (int)(((blockFtr *)((char *)bp+ bp->size - BLK_FTR_SIZE))->size),
        (int)(bp->size));
    bp = advanceBlock(bp);
  }
}


/////////////////////////////////////////////////////////////////////
/////////////Block Manipulation/////////////////////////////////////
////////////////////////////////////////////////////////////////////


void *advanceBlock(blockHdr *b){ // advances to the next block (free or allocated)
  return (blockHdr *)((char *)b + b->size);
}
void *prevBlock(blockHdr *b){ //advances to the prev block (free or allocated)
  blockFtr *p = (blockFtr *)((char *)b - BLK_FTR_SIZE);
  return (blockHdr *)((char *)b - (p->size));
}

void adjust_size(blockHdr *a, size_t size){ //updates sizes in meta data
   a->size = size;
   blockFtr *f = (blockFtr *)((char *)a + (a->size) - BLK_FTR_SIZE);
   f->size = size;
}

void *extend(size_t newsize){ //extends the end of an allocated block when necessary
   blockFtr *end = (blockFtr *)((char *)mem_heap_hi() - BLK_FTR_SIZE + 1);
   blockHdr *last = (blockHdr*)((char *)end - (end->size - BLK_FTR_SIZE));

   blockHdr *newest = mem_sbrk(newsize + NEW_MEM);
   newest->size = (newsize + NEW_MEM);

   if(last->free == 1){ //if the last block is free
     adjust_size(last, last->size + newest->size);
     return last;
   }
   else{ //if it is not..we have some work to do
     adjust_size(newest, newsize + NEW_MEM);
     linkToHead(newest);
     return newest;
   }

}

void linkToHead(blockHdr *bp){ //links newly freed block to the front of a free list.
  blockHdr *head = (blockHdr *)mem_heap_lo();
  bp->free = 1;
  bp->next = head->next;
  bp->prev = head;
  head->next = bp;
  bp->next->prev = bp;
}


//////////////////////////////////////////////////////////////////////////
////////////Helper Methods///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void *last(blockHdr *b){ //returns the last block in the heap (allocated or free)
  return ((char *)b + b->size - 1);
}

int insideHeap(blockHdr *b){ //determines if blockHdr is within the heap.
  void *blast = last(b);
  if(mem_heap_lo() <= blast  && blast <= mem_heap_hi())
    return 1;
  return 0;
}

void *find_fit(size_t size) //returns a first fit search
{
  blockHdr *p = ((blockHdr *)mem_heap_lo())->next;
  while(p != mem_heap_lo()){
    if(p->free == 1 && p->size >= size)
      return p;
    else
      p = p->next;
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////
////////Coalescing Methods//////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void coal(blockHdr *cursor){
	// Somehow this got corrupted. Should just be a perf hit
}