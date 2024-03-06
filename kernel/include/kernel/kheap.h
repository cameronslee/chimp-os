#ifndef KHEAP_H
#define KHEAP_H

#include <system.h>
#include <ordered_array.h>

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000

#define HEAP_INDEX_SIZE   0x20000
#define HEAP_MAGIC        0x123890AB
#define HEAP_MIN_SIZE     0x70000

typedef struct
{
    u32int magic;
    u8int is_hole;
    u32int size;
} header_t;

typedef struct
{
    u32int magic;
    u8int is_hole;
    u32int size;
} footer_t;

typedef struct
{
    ordered_array_t index;
    u32int start_address;
    u32int end_address;
    u32int max_address;
    u8int supervisor;
    u8int read_only;
} heap_t;

/* Create New Heap */
heap_t *create_heap(u32int start, u32int end, u32int max, u8int supervisor, u8int read_only);

/*
 * Allocates a contiguous region of memory 'size' in size. 
 * 
 * If page_align==1, it creates that block starting
 * on a page boundary.
*/
void *alloc(u32int size, u8int page_align, heap_t *heap);

void free(void *p, heap_t *heap);


/*
 * Internal Malloc
 *
 * Allocate a chunk of memory, sz in size. If align == 1,
 * the chunk must be page-aligned. If phys != 0, the physical
*/
u32int kmalloc_int(u32int size, int align, u32int *phys);

/* General-purpose allocator */
u32int kmalloc(u32int size);

/* General-purpose deallocator*/
u32int kfree(void *p);

#endif
