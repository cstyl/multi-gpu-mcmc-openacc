#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "memory.h"

enum mem_error {MEM_SUCCESS = 0,
                MEM_HELP,
                MEM_ZERO_MEM_FAILURE,
                MEM_ERROR
};

#define CHECK_VALID_MEM_REQUEST(size) \
({\
	if(size == 0)\
	{\
		fprintf(stderr, "ERROR::%s:%d:%s: Cannot allocate memory with negative or zero elements!\n", __FILE__, __LINE__,__func__);\
		exit(MEM_ZERO_MEM_FAILURE);\
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


	return MEM_SUCCESS;
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

	return MEM_SUCCESS;
}
