#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "pe.h"
#include "memory.h"
#include "tests.h"

static int test_mem_alloc(void);
static int test_swap_ptrs(void);
static int test_mem_sort(void);

int test_memory_suite(void){

  pe_t *pe = NULL;

  pe_create(MPI_COMM_WORLD, PE_QUIET, &pe);
  assert(pe);
  test_assert(1);
  
  test_mem_alloc();
  test_swap_ptrs();
  test_mem_sort();

  pe_info(pe, "PASS\t./unit/test_memory\n");
  pe_free(pe);

  return 0;
}


static int test_mem_alloc(void){

  /* Data pointers */
  precision *parr1 = NULL;
  precision *parr2 = NULL;
  int *iarr1 = NULL;
  int *iarr2 = NULL;

  int N = 5;

  mem_malloc_precision(&parr1, N);
  test_assert(parr1 != NULL);
  mem_malloc_precision(&parr2, N);
  test_assert(parr2 != NULL);
  mem_malloc_integers(&iarr1, N);
  test_assert(iarr1 != NULL);
  mem_malloc_integers(&iarr2, N);
  test_assert(iarr2 != NULL);


  mem_free((void**)&parr1);
  test_assert(parr1 == NULL);
  mem_free((void**)&parr2);
  test_assert(parr2 == NULL);
  mem_free((void**)&iarr1);
  test_assert(iarr1 == NULL);
  mem_free((void**)&iarr2);
  test_assert(iarr2 == NULL);

  return 0;
}

static int test_swap_ptrs(void){

  /* Temporary pointers for swap */
  precision *parr1 = NULL;
  precision *parr2 = NULL;

  precision arr1[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
  precision arr2[5] = {-1.0, -2.0, -3.0, -4.0, -5.0};

  parr1 = arr1;
  parr2 = arr2;

  mem_swap_ptrs((void**) &parr1, (void**) &parr2);

  test_assert(parr1 == arr2);
  test_assert(parr2 == arr1);

  return 0;
}

static int test_mem_sort(void){

  int N = 5;
  precision arr[5] = {1.0, 5.0, -1.0, 2.5, 0.1};
  precision arr_sort[5] = {-1.0, 0.1, 1.0, 2.5, 5.0};

  mem_sort_precision(arr, N);

  test_assert(fabs(arr[0] - arr_sort[0]) < TEST_DOUBLE_TOLERANCE);
  test_assert(fabs(arr[1] - arr_sort[1]) < TEST_DOUBLE_TOLERANCE);
  test_assert(fabs(arr[2] - arr_sort[2]) < TEST_DOUBLE_TOLERANCE);
  test_assert(fabs(arr[3] - arr_sort[3]) < TEST_DOUBLE_TOLERANCE);
  test_assert(fabs(arr[4] - arr_sort[4]) < TEST_DOUBLE_TOLERANCE);

  return 0;
}
