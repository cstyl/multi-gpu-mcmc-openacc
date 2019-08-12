#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "decomposition.h"
#include "memory.h"

typedef struct bound_s bound_t;

struct bound_s{
  int *low;  /* Lower bound */
  int *hi;   /* Upper bound */
};

struct dc_s{
  pe_t *pe;
  rt_t *rt;
  int rank;
  int size;
  int nprocs;      /* Number of MPI processes (same as MPI_Size) */
  int nthreads;    /* Number of OMP threads per node */
  int ngpus;       /* Number of GPUs per node */
  bound_t pxb;     /* Local MPI bounds on input dataset */
  bound_t pyb;     /* Local MPI bounds on input labels */
  bound_t txb;    /* Bounds for each OMP thread on input dataset */
  bound_t tyb;    /* Bounds for each OMP thread on input labels */
};

static int dc_allocate_pxb(dc_t *dc);
static int dc_allocate_pyb(dc_t *dc);
static int dc_allocate_txb(dc_t *dc);
static int dc_allocate_tyb(dc_t *dc);
static int dc_setwork(int totalWork, int  workers, int id, int *low, int *hi);

int dc_create(pe_t *pe, dc_t **pdc){

  dc_t *dc = NULL;

  assert(pe);

  dc = (dc_t *) calloc(1, sizeof(dc_t));
  assert(dc);
  if(dc == NULL) pe_fatal(pe, "calloc(dc_t) failed\n");

  dc->pe = pe;

  dc_nprocs_set(dc, DEFAULT_PROCS);
  dc_nthreads_set(dc, DEFAULT_THREADS);
  dc_ngpus_set(dc, DEFAULT_GPUS);

  *pdc = dc;

  return 0;
}

int dc_free(dc_t *dc){

  assert(dc);

  mem_free((void**)&dc->pxb.low);
  mem_free((void**)&dc->pxb.hi);
  mem_free((void**)&dc->pyb.low);
  mem_free((void**)&dc->pyb.hi);
  mem_free((void**)&dc->txb.low);
  mem_free((void**)&dc->txb.hi);
  mem_free((void**)&dc->tyb.low);
  mem_free((void**)&dc->tyb.hi);
  mem_free((void**)&dc);

  return 0;
}

int dc_init_rt(pe_t *pe, rt_t *rt, dc_t *dc){

  int nprocs, nthreads, ngpus;

  assert(pe);
  assert(rt);

  if(rt_int_parameter(rt, "nprocs", &nprocs))
  {
    dc_nprocs_set(dc, nprocs);
  }

  if(rt_int_parameter(rt, "nthreads", &nthreads))
  {
    dc_nthreads_set(dc, nthreads);
  }

  if(rt_int_parameter(rt, "ngpus", &ngpus))
  {
    dc_ngpus_set(dc, ngpus);
  }

  dc->rank = pe_mpi_rank(dc->pe);
  dc->size = pe_mpi_size(dc->pe);

  dc_check_inputs(pe, dc->nprocs, dc->nthreads, dc->ngpus);

  dc_allocate_pxb(dc);
  dc_allocate_pyb(dc);
  dc_allocate_txb(dc);
  dc_allocate_tyb(dc);

  return 0;
}

/*****************************************************************************
 *
 *  dc_print_info
 *
 *****************************************************************************/

int dc_print_info(pe_t *pe, dc_t *dc){

  int nprocs, nthreads, ngpus;

  assert(pe);
  assert(dc);

  dc_nprocs(dc, &nprocs);
  dc_nthreads(dc, &nthreads);
  dc_ngpus(dc, &ngpus);


  pe_info(pe, "\n");
  pe_info(pe, "Decomposition Properties\n");
  pe_info(pe, "------------------------\n");
  pe_info(pe, "%30s\t\t%d\n", "Number of Processes:", nprocs);
  pe_info(pe, "%30s\t\t%d\n", "Number of Threads:", nthreads);
  pe_info(pe, "%30s\t\t%d\n", "Number of GPUs:", ngpus);

  return 0;
}

int dc_decompose(int N, int dimx, int dimy, dc_t *dc){

  assert(dc);

  /* Decompose over MPI-processes */
  dc_setwork(N*dimx, dc->size, dc->rank, &dc->pxb.low[0], &dc->pxb.hi[0]);
  dc_setwork(N*dimy, dc->size, dc->rank, &dc->pyb.low[0], &dc->pyb.hi[0]);

  int nthreads = dc->nthreads;
  /* Decompose over OMP threads */
  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    int low, hi;

    /* First on dataset */
    dc_setwork(dc->pxb.hi[0] - dc->pxb.low[0], omp_get_num_threads(), tid, &low, &hi);
    dc->txb.low[tid] = dc->pxb.low[0] + low;
    dc->txb.hi[tid] = dc->pxb.low[0] + hi;

    /*Then on labels */
    dc_setwork(dc->pyb.hi[0] - dc->pyb.low[0], omp_get_num_threads(), tid, &low, &hi);
    dc->tyb.low[tid] = dc->pyb.low[0] + low;
    dc->tyb.hi[tid] = dc->pyb.low[0] + hi;
  }

  return 0;
}

/*****************************************************************************
 *
 *  dc_check_inputs
 *  Only basic check is performed at the moment
 *  It is up to the user to correctly specify the available hardware
 *  Assumes homegeneous nodes (i.e only one type of gpu available on each node)
 *
 *****************************************************************************/

int dc_check_inputs(pe_t *pe, int nprocs, int nthreads, int ngpus){

  if(ngpus != 0 && nthreads != ngpus)
    pe_fatal(pe, "Select nthreads = ngpus when running on GPUs.");

  int nvidia_gpus = acc_get_num_devices(acc_device_nvidia);
  int amd_gpus = acc_get_num_devices(acc_device_radeon);

  if((acc_get_device_type() != acc_device_host) &&
    ((ngpus > nvidia_gpus)||(ngpus > amd_gpus)))
    pe_fatal(pe, "Please set a valid number of GPUs per node");

  if(nthreads <= 0)
    pe_fatal(pe, "Select nthreads to be a positive number. For serial set to 1.");

  return 0;
}

static int dc_setwork(int totalWork, int  workers, int id, int *low, int *hi){
  int part = totalWork/workers;
  int remainWork = totalWork - part*workers;

  if (id >= (workers - remainWork)){
  	part = part + 1;
  	*low = part*id - (workers-remainWork);
  	*hi = part*(1+id) - (workers-remainWork);
  }
  else
  {
  	*low = part*id;
  	*hi = part*(1+id);
  }

	return 0;
}

/*****************************************************************************
 *
 *  dc_nprocs_set
 *
 *****************************************************************************/

int dc_nprocs_set(dc_t *dc, int nprocs){

  assert(dc);

  dc->nprocs = nprocs;

  return 0;
}

/*****************************************************************************
 *
 *  dc_nprocs
 *
 *****************************************************************************/

int dc_nprocs(dc_t *dc, int *nprocs){

  assert(dc);

  *nprocs = dc->nprocs;

  return 0;
}

/*****************************************************************************
 *
 *  dc_threads_set
 *
 *****************************************************************************/

int dc_nthreads_set(dc_t *dc, int nthreads){

  assert(dc);

  dc->nthreads = nthreads;

  return 0;
}

/*****************************************************************************
 *
 *  dc_threads
 *
 *****************************************************************************/

int dc_nthreads(dc_t *dc, int *nthreads){

  assert(dc);

  *nthreads = dc->nthreads;

  return 0;
}

/*****************************************************************************
 *
 *  dc_ngpus_set
 *
 *****************************************************************************/

int dc_ngpus_set(dc_t *dc, int ngpus){

  assert(dc);

  dc->ngpus = ngpus;

  return 0;
}

/*****************************************************************************
 *
 *  dc_ngpus
 *
 *****************************************************************************/

int dc_ngpus(dc_t *dc, int *ngpus){

  assert(dc);

  *ngpus = dc->ngpus;

  return 0;
}

/*****************************************************************************
 *
 *  dc_pxb_set
 *
 *****************************************************************************/

int dc_pxb_set(dc_t *dc, int pxlow, int pxhi){

  assert(dc);

  dc->pxb.low[0] = pxlow;
  dc->pxb.hi[0]  = pxhi;

  return 0;
}

/*****************************************************************************
 *
 *  dc_pxb
 *
 *****************************************************************************/

int dc_pxb(dc_t *dc, int **ppxlow, int **ppxhi){

  assert(dc);

  *ppxlow = dc->pxb.low;
  *ppxhi = dc->pxb.hi;

  return 0;
}

/*****************************************************************************
 *
 *  dc_pyb_set
 *
 *****************************************************************************/

int dc_pyb_set(dc_t *dc, int pylow, int pyhi){

  assert(dc);

  dc->pyb.low[0] = pylow;
  dc->pyb.hi[0]  = pyhi;

  return 0;
}

/*****************************************************************************
 *
 *  dc_pyb
 *
 *****************************************************************************/

int dc_pyb(dc_t *dc, int **ppylow, int **ppyhi){

  assert(dc);

  *ppylow = dc->pyb.low;
  *ppyhi = dc->pyb.hi;

  return 0;
}

/*****************************************************************************
 *
 *  dc_txb
 *
 *****************************************************************************/

int dc_txb(dc_t *dc, int **ptxlow, int **ptxhi){

  assert(dc);

  *ptxlow = dc->txb.low;
  *ptxhi = dc->txb.hi;

  return 0;
}

/*****************************************************************************
 *
 *  dc_tyb
 *
 *****************************************************************************/

int dc_tyb(dc_t *dc, int **ptylow, int **ptyhi){

 assert(dc);

 *ptylow = dc->tyb.low;
 *ptyhi = dc->tyb.hi;

 return 0;
}

/*****************************************************************************
 *
 *  dc_rank_set
 *
 *****************************************************************************/

int dc_rank_set(dc_t *dc, int rank){

  assert(dc);

  dc->rank = rank;

  return 0;
}

/*****************************************************************************
 *
 *  dc_size_set
 *
 *****************************************************************************/

int dc_size_set(dc_t *dc, int size){

  assert(dc);

  dc->size = size;

  return 0;
}

static int dc_allocate_pxb(dc_t *dc){

  assert(dc);

  mem_malloc_integers(&dc->pxb.low, 1);
  mem_malloc_integers(&dc->pxb.hi, 1);

  return 0;
}

static int dc_allocate_pyb(dc_t *dc){

  assert(dc);

  mem_malloc_integers(&dc->pyb.low, 1);
  mem_malloc_integers(&dc->pyb.hi, 1);

  return 0;
}

static int dc_allocate_txb(dc_t *dc){

  assert(dc);

  mem_malloc_integers(&dc->txb.low, dc->nthreads);
  mem_malloc_integers(&dc->txb.hi, dc->nthreads);

  return 0;
}

static int dc_allocate_tyb(dc_t *dc){

  assert(dc);

  mem_malloc_integers(&dc->tyb.low, dc->nthreads);
  mem_malloc_integers(&dc->tyb.hi, dc->nthreads);

  return 0;
}
