#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "decomposition.h"
#include "memory.h"

struct dc_s{
  pe_t *pe;
  rt_t *rt;
  int rank;
  int size;        /* Communicator size */
  int nprocs;      /* Number of MPI processes per node */
  int work;        /* Total size of the problem */
  int plow;        /* Local lower MPI bound of a datapoint */
  int phi;         /* Local upper MPI bound of a datapoint */
  int nthreads;    /* Number of OMP threads per node */
  int *tlow;       /* Lower bounds for each OMP thread */
  int *thi;        /* Lower bounds for each OMP thread */
};

static int dc_splitwork(int totalWork, int  workers, int id, int *low, int *hi);

int dc_create(pe_t *pe, dc_t **pdc){

  dc_t *dc = NULL;

  assert(pe);

  dc = (dc_t *) calloc(1, sizeof(dc_t));
  assert(dc);
  if(dc == NULL) pe_fatal(pe, "calloc(dc_t) failed\n");

  dc->pe = pe;

  dc->rank = pe_mpi_rank(pe);
  dc->size = pe_mpi_size(pe);
  dc_nprocs_set(dc, DEFAULT_PROCS);
  dc_nthreads_set(dc, DEFAULT_THREADS);

  *pdc = dc;

  return 0;
}

int dc_free(dc_t *dc){

  assert(dc);

  mem_free((void**)&dc->tlow);
  mem_free((void**)&dc->thi);
  mem_free((void**)&dc);

  return 0;
}

int dc_init_rt(pe_t *pe, rt_t *rt, dc_t *dc){

  int nprocs, nthreads;

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

  mem_malloc_integers(&dc->tlow, dc->nthreads);
  mem_malloc_integers(&dc->thi, dc->nthreads);

  return 0;
}

/*****************************************************************************
 *
 *  dc_print_info
 *
 *****************************************************************************/

int dc_print_info(pe_t *pe, dc_t *dc){

  assert(pe);
  assert(dc);

  pe_info(pe, "\n");
  pe_info(pe, "Decomposition Properties\n");
  pe_info(pe, "------------------------\n");
  pe_info(pe, "%30s\t\t%d\n", "Communicator size:", dc->size);
  pe_info(pe, "%30s\t\t%d\n", "Number of Nodes:", (int)ceil((precision)dc->size/dc->nprocs));
  pe_info(pe, "%30s\t\t%d\n", "Number of Processes/node:", dc->nprocs);
  pe_info(pe, "%30s\t\t%d\n", "Number of Threads/process:", dc->nthreads);

  return 0;
}

int dc_decompose(dc_t *dc){

  assert(dc);

  /* Decompose over MPI-processes */
  dc_splitwork(dc->work, dc->size, dc->rank, &dc->plow, &dc->phi);

  int nthreads = dc->nthreads;
  /* Decompose over OMP threads (sort of static scheduling) */
  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    int low, hi;

    dc_splitwork(dc->phi - dc->plow, omp_get_num_threads(), tid, &low, &hi);
    dc->tlow[tid] = dc->plow + low;
    dc->thi[tid] = dc->plow + hi;
  }

  return 0;
}

static int dc_splitwork(int totalWork, int  workers, int id, int *low, int *hi){
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
 *  dc_pbound_set
 *
 *****************************************************************************/

int dc_pbound_set(dc_t *dc, int plow, int phi){

  assert(dc);

  dc->plow = plow;
  dc->phi  = phi;

  return 0;
}

/*****************************************************************************
 *
 *  dc_pbound
 *
 *****************************************************************************/

int dc_pbound(dc_t *dc, int *plow, int *phi){

  assert(dc);

  *plow = dc->plow;
  *phi = dc->phi;

  return 0;
}

/*****************************************************************************
 *
 *  dc_tbound
 *
 *****************************************************************************/

int dc_tbound(dc_t *dc, int **ptlow, int **pthi){

  assert(dc);

  *ptlow = dc->tlow;
  *pthi = dc->thi;

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

/*****************************************************************************
 *
 *  dc_work_set
 *
 *****************************************************************************/

int dc_work_set(dc_t *dc, int work){

  assert(dc);

  dc->work = work;

  return 0;
}
