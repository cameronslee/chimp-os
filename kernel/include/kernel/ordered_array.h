#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include "system.h"

/**
   array is insertion sorted - always in a sorted state even between calls
   stores anything that can be cast to a void* -- so a u32int, or any pointer.
**/
typedef void* type_t;

/**
   A predicate should return nonzero if the first argument is less than the second. Else 
   it should return zero.
**/
typedef s8int (*lessthan_predicate_t)(type_t,type_t);

typedef struct
{
    type_t *array;
    u32int size;
    u32int max_size;
    lessthan_predicate_t less_than;
} ordered_array_t;

s8int standard_lessthan_predicate(type_t a, type_t b);

ordered_array_t create_ordered_array(u32int max_size, lessthan_predicate_t less_than);

ordered_array_t place_ordered_array(void *addr, u32int max_size, lessthan_predicate_t less_than);

void destroy_ordered_array(ordered_array_t *array);

void insert_ordered_array(type_t item, ordered_array_t *array);

type_t lookup_ordered_array(u32int i, ordered_array_t *array);

void remove_ordered_array(u32int i, ordered_array_t *array);

#endif
