#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "definitions.h"
#include "pe.h"
#include "runtime.h"
#include "data_input.h"
#include "decomposition.h"
#include "tests.h"

static int test_decomposition_rt_default(pe_t *pe);
static int test_decomposition_rt(pe_t *pe);
static int test_decomposition_1_1_0(pe_t *pe);
static int test_decomposition_4_1_0(pe_t *pe);
static int test_decomposition_1_4_0(pe_t *pe);
static int test_decomposition_4_3_0(pe_t *pe);
static int test_decomposition_4_4_4(pe_t *pe);
static int run_decomposition(dc_t *dc, int N, int nprocs, int nthreads);

int test_decomposition_suite(void){

  pe_t *pe = NULL;

  pe_create(MPI_COMM_WORLD, PE_QUIET, &pe);
  assert(pe);
  test_assert(1);

  test_decomposition_rt_default(pe);
  test_decomposition_rt(pe);
  test_decomposition_1_1_0(pe);
  test_decomposition_4_1_0(pe);
  test_decomposition_1_4_0(pe);
  test_decomposition_4_3_0(pe);
  test_decomposition_4_4_4(pe);

  pe_info(pe, "PASS\t./unit/test_decomposition\n");
  pe_free(pe);

  return 0;
}

static int test_decomposition_rt_default(pe_t *pe){

  assert(pe);

  int nprocs, nthreads, ngpus;

  rt_t *rt = NULL;
  dc_t *dc = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);
  test_assert(1);

  dc_nprocs(dc, &nprocs);
  test_assert(nprocs == DEFAULT_PROCS);
  dc_nthreads(dc, &nthreads);
  test_assert(nthreads == DEFAULT_THREADS);

  dc_free(dc);
  rt_free(rt);

  return 0;

}

static int test_decomposition_rt(pe_t *pe){

  assert(pe);

  int nprocs, nthreads, ngpus;

  int *tlow=NULL, *thi=NULL;

  rt_t *rt = NULL;
  dc_t *dc = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  dc_create(pe, &dc);
  assert(dc);
  test_assert(1);

  /* testing dc runtime init */
  dc_init_rt(pe, rt, dc);

  dc_nprocs(dc, &nprocs);
  test_assert(nprocs == 1);
  dc_nthreads(dc, &nthreads);
  test_assert(nthreads == 1);

  dc_tbound(dc, &tlow, &thi);
  test_assert(tlow != NULL && thi != NULL);

  dc_free(dc);
  rt_free(rt);

  return 0;

}

static int test_decomposition_1_1_0(pe_t *pe){

  assert(pe);
  /* Test decomposition for 1 processes 1 thread 0 GPUs */
  int i, nprocs = 1, nthreads = 1, ngpus = 0;
  /* Test Case: N=500 */
  int N[5] = {211, 500, 1007, 11113, 1200456};

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *data = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, dc, &data);
  assert(data);
  test_assert(1);

  dc_nprocs_set(dc, nprocs);
  dc_nthreads_set(dc, nthreads);
  dc_init_rt(pe, rt, dc); /* Initialise to allocate memory for parameters */
  dc_size_set(dc, nprocs);

  for(i=0; i<5; i++)
  {
    data_N_set(data, N[i]);
    run_decomposition(dc, N[i], nprocs, nthreads);
  }

  dc_free(dc);
  data_free(data);
  rt_free(rt);

  return 0;

}

static int test_decomposition_4_1_0(pe_t *pe){

  assert(pe);
  /* Test decomposition for 4 processes 1 thread 0 GPUs */
  int i, nprocs = 4, nthreads = 1, ngpus = 0;
  /* Test Case: N=500 */
  int N[5] = {211, 500, 1007, 11113, 1200456};

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *data = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, dc, &data);
  assert(data);
  test_assert(1);

  dc_nprocs_set(dc, nprocs);
  dc_nthreads_set(dc, nthreads);
  dc_init_rt(pe, rt, dc); /* Initialise to allocate memory for parameters */
  dc_size_set(dc, nprocs);

  for(i=0; i<5; i++)
  {
    data_N_set(data, N[i]);
    run_decomposition(dc, N[i], nprocs, nthreads);
  }

  dc_free(dc);
  data_free(data);
  rt_free(rt);

  return 0;

}

static int test_decomposition_1_4_0(pe_t *pe){

  assert(pe);
  /* Test decomposition for 1 processes 4 thread 0 GPUs */
  int i, nprocs = 1, nthreads = 4, ngpus = 0;
  /* Test Case: N=500 */
  int N[5] = {211, 500, 1007, 11113, 1200456};

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *data = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, dc, &data);
  assert(data);
  test_assert(1);

  dc_nprocs_set(dc, nprocs);
  dc_nthreads_set(dc, nthreads);
  dc_init_rt(pe, rt, dc); /* Initialise to allocate memory for parameters */
  dc_size_set(dc, nprocs);

  for(i=0; i<5; i++)
  {
    data_N_set(data, N[i]);
    run_decomposition(dc, N[i], nprocs, nthreads);
  }

  dc_free(dc);
  data_free(data);
  rt_free(rt);

  return 0;

}

static int test_decomposition_4_3_0(pe_t *pe){

  assert(pe);
  /* Test decomposition for 4 processes 3 thread 0 GPUs */
  int i, nprocs = 4, nthreads = 3, ngpus = 0;
  /* Test Case: N=500 */
  int N[5] = {211, 500, 1007, 11113, 1200456};

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *data = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, dc, &data);
  assert(data);
  test_assert(1);

  dc_nprocs_set(dc, nprocs);
  dc_nthreads_set(dc, nthreads);
  dc_init_rt(pe, rt, dc); /* Initialise to allocate memory for parameters */
  dc_size_set(dc, nprocs);

  for(i=0; i<5; i++)
  {
    data_N_set(data, N[i]);
    run_decomposition(dc, N[i], nprocs, nthreads);
  }

  dc_free(dc);
  data_free(data);
  rt_free(rt);

  return 0;

}

static int test_decomposition_4_4_4(pe_t *pe){

  assert(pe);
  /* Test decomposition for 4 processes 4 thread 4 GPUs */
  int i, nprocs = 4, nthreads = 4, ngpus = 4;
  /* Test Case: N=500 */
  int N[5] = {211, 500, 1007, 11113, 1200456};

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *data = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, dc, &data);
  assert(data);
  test_assert(1);

  dc_nprocs_set(dc, nprocs);
  dc_nthreads_set(dc, nthreads);
  dc_init_rt(pe, rt, dc); /* Initialise to allocate memory for parameters */
  dc_size_set(dc, nprocs);

  for(i=0; i<5; i++)
  {
    data_N_set(data, N[i]);
    run_decomposition(dc, N[i], nprocs, nthreads);
  }

  dc_free(dc);
  data_free(data);
  rt_free(rt);

  return 0;

}

static int run_decomposition(dc_t *dc, int N, int nprocs, int nthreads){

  assert(dc);

  int plow, phi;
  int *tlow=NULL, *thi=NULL;

  int i, j;
  int pload = 0;
  for(i=0; i<nprocs; i++)
  {
    dc_rank_set(dc, i);
    dc_work_set(dc, N);
    dc_decompose(dc);

    dc_pbound(dc, &plow, &phi);
    dc_tbound(dc, &tlow, &thi);

    pload += phi - plow;

    int tload = 0;
    for(j=0; j<nthreads; j++)
    {
      tload += thi[j] - tlow[j];
    }
    /* Workload on threads here should really be equal
    *  to the load of the current process
    */
    test_assert(tload == phi - plow);
  }
  /* Workload on all processes here should
  *  be equal to the original load N
  */
  test_assert(pload == N);

  return 0;

}
