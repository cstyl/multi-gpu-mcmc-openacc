#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "definitions.h"

int mem_malloc_precision(precision **array, int elements);
int mem_malloc_integers(int **array, int elements);
int mem_sort_precision(precision *array, int n);
int mem_free(void **ptr);
int mem_swap_ptrs(void **ptr1, void **ptr2);
#endif // __MEMORY_H__
