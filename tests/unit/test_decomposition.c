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
  dc_ngpus(dc, &ngpus);
  test_assert(ngpus == DEFAULT_GPUS);

  dc_free(dc);
  rt_free(rt);

  return 0;

}

static int test_decomposition_rt(pe_t *pe){

  assert(pe);

  int nprocs, nthreads, ngpus;

  int *pxlow=NULL, *pxhi=NULL;
  int *pylow=NULL, *pyhi=NULL;
  int *txlow=NULL, *txhi=NULL;
  int *tylow=NULL, *tyhi=NULL;

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
  test_assert(nprocs == 2);
  dc_nthreads(dc, &nthreads);
  test_assert(nthreads == 4);
  dc_ngpus(dc, &ngpus);
  test_assert(ngpus == 4);

  dc_pxb(dc, &pxlow, &pxhi);
  test_assert(pxlow != NULL && pxhi != NULL);
  dc_pyb(dc, &pylow, &pyhi);
  test_assert(pylow != NULL && pyhi != NULL);
  dc_txb(dc, &txlow, &txhi);
  test_assert(txlow != NULL && txhi != NULL);
  dc_tyb(dc, &tylow, &tyhi);
  test_assert(tylow != NULL && tyhi != NULL);

  dc_free(dc);
  rt_free(rt);

  return 0;

}

static int test_decomposition_1_1_0(pe_t *pe){

  assert(pe);
  /* Test decomposition for 1 process 1 thread zero GPUs */
  int nprocs = 1, nthreads = 1, ngpus = 0;
  /* Test Case: N=500, dimx=3, dimy=1 */
  int N = 500, dimx = 3, dimy = 1;

  int *pxlow=NULL, *pxhi=NULL;
  int *pylow=NULL, *pyhi=NULL;
  int *txlow=NULL, *txhi=NULL;
  int *tylow=NULL, *tyhi=NULL;

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *data = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, &data);
  assert(data);
  test_assert(1);

  data_N_set(data, N);
  data_dimx_set(data, dimx);
  data_dimy_set(data, dimy);

  dc_nprocs_set(dc, nprocs);
  dc_nthreads_set(dc, nthreads);
  dc_ngpus_set(dc, ngpus);
  dc_init_rt(pe, rt, dc); /* Initialise to allocate memory for parameters */

  dc_size_set(dc, nprocs);

  int i, j;
  int ploadx = 0, ploady = 0;
  for(i=0; i<nprocs; i++)
  {
    dc_rank_set(dc, i);
    dc_decompose(N, dimx, dimy, dc);

    dc_pxb(dc, &pxlow, &pxhi);
    dc_pyb(dc, &pylow, &pyhi);
    dc_txb(dc, &txlow, &txhi);
    dc_tyb(dc, &tylow, &tyhi);

    ploadx += pxhi[0] - pxlow[0];
    ploady += pyhi[0] - pylow[0];

    int tloadx = 0, tloady = 0;
    for(j=0; j<nthreads; j++)
    {
      tloadx += txhi[j] - txlow[j];
      tloady += tyhi[j] - tylow[j];
    }
    /* Workload on threads here should really be equal
    *  to the load of the current process
    */
    test_assert(tloadx == pxhi[0] - pxlow[0]);
    test_assert(tloady == pyhi[0] - pylow[0]);
  }
  /* Workload on all processes here should be equal
  *  to the original load (N*dimx) and (N*dimy)
  */
  test_assert(ploadx == N*dimx);
  test_assert(ploady == N*dimy);

  dc_free(dc);
  data_free(data);
  rt_free(rt);

  return 0;

}

static int test_decomposition_4_1_0(pe_t *pe){

  assert(pe);
  /* Test decomposition for 4 processes 1 thread zero GPUs */
  int nprocs = 4, nthreads = 1, ngpus = 0;
  /* Test Case: N=500, dimx=3, dimy=1 */
  int N = 500, dimx = 3, dimy = 1;

  int *pxlow=NULL, *pxhi=NULL;
  int *pylow=NULL, *pyhi=NULL;
  int *txlow=NULL, *txhi=NULL;
  int *tylow=NULL, *tyhi=NULL;

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *data = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, &data);
  assert(data);
  test_assert(1);

  data_N_set(data, N);
  data_dimx_set(data, dimx);
  data_dimy_set(data, dimy);

  dc_nprocs_set(dc, nprocs);
  dc_nthreads_set(dc, nthreads);
  dc_ngpus_set(dc, ngpus);
  dc_init_rt(pe, rt, dc); /* Initialise to allocate memory for parameters */

  dc_size_set(dc, nprocs);

  int i, j;
  int ploadx = 0, ploady = 0;
  for(i=0; i<nprocs; i++)
  {
    dc_rank_set(dc, i);
    dc_decompose(N, dimx, dimy, dc);

    dc_pxb(dc, &pxlow, &pxhi);
    dc_pyb(dc, &pylow, &pyhi);
    dc_txb(dc, &txlow, &txhi);
    dc_tyb(dc, &tylow, &tyhi);

    ploadx += pxhi[0] - pxlow[0];
    ploady += pyhi[0] - pylow[0];

    int tloadx = 0, tloady = 0;
    for(j=0; j<nthreads; j++)
    {
      tloadx += txhi[j] - txlow[j];
      tloady += tyhi[j] - tylow[j];
    }
    /* Workload on threads here should really be equal
    *  to the load of the current process
    */
    test_assert(tloadx == pxhi[0] - pxlow[0]);
    test_assert(tloady == pyhi[0] - pylow[0]);
  }
  /* Workload on all processes here should be equal
  *  to the original load (N*dimx) and (N*dimy)
  */
  test_assert(ploadx == N*dimx);
  test_assert(ploady == N*dimy);

  dc_free(dc);
  data_free(data);
  rt_free(rt);

  return 0;

}

static int test_decomposition_1_4_0(pe_t *pe){

  assert(pe);
  /* Test decomposition for 1 processes 4 thread zero GPUs */
  int nprocs = 1, nthreads = 4, ngpus = 0;
  /* Test Case: N=500, dimx=3, dimy=1 */
  int N = 500, dimx = 3, dimy = 1;

  int *pxlow=NULL, *pxhi=NULL;
  int *pylow=NULL, *pyhi=NULL;
  int *txlow=NULL, *txhi=NULL;
  int *tylow=NULL, *tyhi=NULL;

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *data = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, &data);
  assert(data);
  test_assert(1);

  data_N_set(data, N);
  data_dimx_set(data, dimx);
  data_dimy_set(data, dimy);

  dc_nprocs_set(dc, nprocs);
  dc_nthreads_set(dc, nthreads);
  dc_ngpus_set(dc, ngpus);
  dc_init_rt(pe, rt, dc); /* Initialise to allocate memory for parameters */

  dc_size_set(dc, nprocs);

  int i, j;
  int ploadx = 0, ploady = 0;
  for(i=0; i<nprocs; i++)
  {
    dc_rank_set(dc, i);
    dc_decompose(N, dimx, dimy, dc);

    dc_pxb(dc, &pxlow, &pxhi);
    dc_pyb(dc, &pylow, &pyhi);
    dc_txb(dc, &txlow, &txhi);
    dc_tyb(dc, &tylow, &tyhi);

    ploadx += pxhi[0] - pxlow[0];
    ploady += pyhi[0] - pylow[0];

    int tloadx = 0, tloady = 0;
    for(j=0; j<nthreads; j++)
    {
      tloadx += txhi[j] - txlow[j];
      tloady += tyhi[j] - tylow[j];
    }
    /* Workload on threads here should really be equal
    *  to the load of the current process
    */
    test_assert(tloadx == pxhi[0] - pxlow[0]);
    test_assert(tloady == pyhi[0] - pylow[0]);
  }
  /* Workload on all processes here should be equal
  *  to the original load (N*dimx) and (N*dimy)
  */
  test_assert(ploadx == N*dimx);
  test_assert(ploady == N*dimy);

  dc_free(dc);
  data_free(data);
  rt_free(rt);

  return 0;

}

static int test_decomposition_4_3_0(pe_t *pe){

  assert(pe);
  /* Test decomposition for 4 processes 3 thread zero GPUs */
  int nprocs = 4, nthreads = 3, ngpus = 0;
  /* Test Case: N=500, dimx=3, dimy=1 */
  int N = 500, dimx = 3, dimy = 1;

  int *pxlow=NULL, *pxhi=NULL;
  int *pylow=NULL, *pyhi=NULL;
  int *txlow=NULL, *txhi=NULL;
  int *tylow=NULL, *tyhi=NULL;

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *data = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, &data);
  assert(data);
  test_assert(1);

  data_N_set(data, N);
  data_dimx_set(data, dimx);
  data_dimy_set(data, dimy);

  dc_nprocs_set(dc, nprocs);
  dc_nthreads_set(dc, nthreads);
  dc_ngpus_set(dc, ngpus);
  dc_init_rt(pe, rt, dc); /* Initialise to allocate memory for parameters */

  dc_size_set(dc, nprocs);

  int i, j;
  int ploadx = 0, ploady = 0;
  for(i=0; i<nprocs; i++)
  {
    dc_rank_set(dc, i);
    dc_decompose(N, dimx, dimy, dc);

    dc_pxb(dc, &pxlow, &pxhi);
    dc_pyb(dc, &pylow, &pyhi);
    dc_txb(dc, &txlow, &txhi);
    dc_tyb(dc, &tylow, &tyhi);

    ploadx += pxhi[0] - pxlow[0];
    ploady += pyhi[0] - pylow[0];

    int tloadx = 0, tloady = 0;
    for(j=0; j<nthreads; j++)
    {
      tloadx += txhi[j] - txlow[j];
      tloady += tyhi[j] - tylow[j];
    }
    /* Workload on threads here should really be equal
    *  to the load of the current process
    */
    test_assert(tloadx == pxhi[0] - pxlow[0]);
    test_assert(tloady == pyhi[0] - pylow[0]);
  }
  /* Workload on all processes here should be equal
  *  to the original load (N*dimx) and (N*dimy)
  */
  test_assert(ploadx == N*dimx);
  test_assert(ploady == N*dimy);

  dc_free(dc);
  data_free(data);
  rt_free(rt);

  return 0;

}

static int test_decomposition_4_4_4(pe_t *pe){

  assert(pe);
  /* Test decomposition for 4 processes 4 thread 4 GPUs */
  int nprocs = 4, nthreads = 4, ngpus = 4;
  /* Test Case: N=500, dimx=3, dimy=1 */
  int N = 500, dimx = 3, dimy = 1;

  int *pxlow=NULL, *pxhi=NULL;
  int *pylow=NULL, *pyhi=NULL;
  int *txlow=NULL, *txhi=NULL;
  int *tylow=NULL, *tyhi=NULL;

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *data = NULL;

  rt_create(pe, &rt);
  assert(rt);

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, &data);
  assert(data);
  test_assert(1);

  data_N_set(data, N);
  data_dimx_set(data, dimx);
  data_dimy_set(data, dimy);

  dc_nprocs_set(dc, nprocs);
  dc_nthreads_set(dc, nthreads);
  dc_ngpus_set(dc, ngpus);
  dc_init_rt(pe, rt, dc); /* Initialise to allocate memory for parameters */

  dc_size_set(dc, nprocs);

  int i, j;
  int ploadx = 0, ploady = 0;
  for(i=0; i<nprocs; i++)
  {
    dc_rank_set(dc, i);
    dc_decompose(N, dimx, dimy, dc);

    dc_pxb(dc, &pxlow, &pxhi);
    dc_pyb(dc, &pylow, &pyhi);
    dc_txb(dc, &txlow, &txhi);
    dc_tyb(dc, &tylow, &tyhi);

    ploadx += pxhi[0] - pxlow[0];
    ploady += pyhi[0] - pylow[0];

    int tloadx = 0, tloady = 0;
    for(j=0; j<nthreads; j++)
    {
      tloadx += txhi[j] - txlow[j];
      tloady += tyhi[j] - tylow[j];
    }
    /* Workload on threads here should really be equal
    *  to the load of the current process
    */
    test_assert(tloadx == pxhi[0] - pxlow[0]);
    test_assert(tloady == pyhi[0] - pylow[0]);
  }
  /* Workload on all processes here should be equal
  *  to the original load (N*dimx) and (N*dimy)
  */
  test_assert(ploadx == N*dimx);
  test_assert(ploady == N*dimy);

  dc_free(dc);
  data_free(data);
  rt_free(rt);

  return 0;

}
