#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "definitions.h"

int mem_malloc_precision(precision **array, int elements);
int mem_malloc_integers(int **array, int elements);
int mem_copy_precision(precision *original, precision *copy, int n);
int mem_sort_precision(precision *array, int n);

#endif // __MEMORY_H__
