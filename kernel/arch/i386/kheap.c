#include "kernel/kheap.h"

extern 32int kernel_end;
u32int placement_address = (u32int)&kernel_end;
extern page_directory_t *kernel_directory;
heap_t *kheap=0;

heap_t *create_heap(u32int start, u32int end, u32int max, u8int supervisor, u8int read_only)
{

}

/*
 * Allocates a contiguous region of memory 'size' in size. 
 * 
 * If page_align==1, it creates that block starting
 * on a page boundary.
*/
void *alloc(u32int size, u8int page_align, heap_t *heap)
{

}

void free(void *p, heap_t *heap)
{

}

/*
 * Internal Malloc
 *
 * Allocate a chunk of memory, sz in size. If align == 1,
 * the chunk must be page-aligned. If phys != 0, the physical
*/
u32int kmalloc_int(u32int size, int align, u32int *phys)
{

}

/* General-purpose allocator */
u32int kmalloc(u32int size)
{

}

/* General-purpose deallocator*/
u32int kfree(void *p)
{

}
