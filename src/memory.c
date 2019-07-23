#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "memory.h"

#define CHECK_VALID_MEM_REQUEST(size) \
({\
	if(size == 0)\
	{\
		fprintf(stderr, "ERROR::%s:%d:%s: Cannot allocate memory with negative or zero elements!\n", __FILE__, __LINE__,__func__);\
		exit(1);\
	}\
})

/*****************************************************************************
 *
 *  mem_malloc_precision
 *
 *****************************************************************************/

int mem_malloc_precision(precision **array, int elements){

	*array = NULL;

	CHECK_VALID_MEM_REQUEST(elements);

	*array = (precision *) malloc(elements * sizeof(precision));
	assert(*array);

	return 0;
}

/*****************************************************************************
 *
 *  mem_malloc_integers
 *
 *****************************************************************************/

int mem_malloc_integers(int **array, int elements){

	*array = NULL;

	CHECK_VALID_MEM_REQUEST(elements);

	*array = (int *) malloc(elements * sizeof(int));
  assert(*array);

	return 0;
}

/*****************************************************************************
 *
 *  mem_sort
 *
 *****************************************************************************/

int mem_sort_precision(precision *array, int n){

  assert(array);
  int i,j;
  precision temp = 0.0;

  for(i=0; i<n; i++){
    for(j=0; j<n-1; j++){
      if(array[j] > array[j+1])
      {
        temp = array[j];
        array[j] = array[j+1];
        array[j+1] = temp;
      }
    }
  }

  return 0;
}

/*****************************************************************************
 *
 *  mem_free
 *
 *****************************************************************************/
int mem_free(void **ptr){

	if(*ptr!=NULL)
	{
		free(*ptr);
		*ptr=NULL;
	}

	return 0;
}

/*****************************************************************************
 *
 *  mem_swap_ptrs
 *
 *****************************************************************************/

int mem_swap_ptrs(void **ptr1, void **ptr2)
{
  void *ptr_temp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = ptr_temp;

	return 0;
}
