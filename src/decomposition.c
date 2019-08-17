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
  int size;
  int nnodes;      /* Number of Nodes */
  int nprocs;      /* Number of MPI processes per node */
  int nthreads;    /* Number of OMP threads per node */
  int ngpus;       /* Number of GPUs per node */
  int plow;        /* Local lower MPI bound of a datapoint */
  int phi;         /* Local upper MPI bound of a datapoint */
  int *tlow;       /* Lower bounds for each OMP thread */
  int *thi;        /* Lower bounds for each OMP thread */
};

static int dc_setwork(int totalWork, int  workers, int id, int *low, int *hi);

int dc_create(pe_t *pe, dc_t **pdc){

  dc_t *dc = NULL;

  assert(pe);

  dc = (dc_t *) calloc(1, sizeof(dc_t));
  assert(dc);
  if(dc == NULL) pe_fatal(pe, "calloc(dc_t) failed\n");

  dc->pe = pe;

  dc_nnodes_set(dc, DEFAULT_NODES);
  dc_nprocs_set(dc, DEFAULT_PROCS);
  dc_nthreads_set(dc, DEFAULT_THREADS);
  dc_ngpus_set(dc, DEFAULT_GPUS);

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

  int nnodes, nprocs, nthreads, ngpus;

  assert(pe);
  assert(rt);

  if(rt_int_parameter(rt, "nnodes", &nnodes))
  {
    dc_nnodes_set(dc, nnodes);
  }

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

  // dc_check_inputs(pe, dc->nprocs, dc->nthreads, dc->ngpus);
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

  int nnodes, nprocs, nthreads, ngpus;

  assert(pe);
  assert(dc);

  dc_nnodes(dc, &nnodes);
  dc_nprocs(dc, &nprocs);
  dc_nthreads(dc, &nthreads);
  dc_ngpus(dc, &ngpus);

  pe_info(pe, "\n");
  pe_info(pe, "Decomposition Properties\n");
  pe_info(pe, "------------------------\n");
  pe_info(pe, "%30s\t\t%d\n", "Communicator size:", dc->size);
  pe_info(pe, "%30s\t\t%d\n", "Number of Nodes:", nnodes);
  pe_info(pe, "%30s\t\t%d\n", "Number of Processes/node:", nprocs);
  pe_info(pe, "%30s\t\t%d\n", "Number of Threads:", nthreads);
  pe_info(pe, "%30s\t\t%d\n", "Number of GPUs:", ngpus);

  return 0;
}

int dc_decompose(int N, dc_t *dc){

  assert(dc);

  /* Decompose over MPI-processes */
  // printf("[%d] Start decomposing", dc->rank);
  dc_setwork(N, dc->size, dc->rank, &dc->plow, &dc->phi);
  // printf("[%d] low:%d hi:%d\n", dc->rank, dc->plow, dc->phi);
  int nthreads = dc->nthreads;
  /* Decompose over OMP threads */
  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    int low, hi;

    dc_setwork(dc->phi - dc->plow, omp_get_num_threads(), tid, &low, &hi);
    dc->tlow[tid] = dc->plow + low;
    dc->thi[tid] = dc->plow + hi;
    // printf("[%d][%d] low:%d hi:%d\n", dc->rank, tid, dc->tlow[tid], dc->thi[tid]);
  }

  return 0;
}

/*****************************************************************************
 *
 *  dc_check_inputs
 *  Only basic check is performed at the moment
 *  It is up to the user to correctly specify the available hardware
 *  Assumes homegeneous nodes (i.e same number of gpus across nodes)
 *
 *****************************************************************************/

int dc_check_inputs(pe_t *pe, int nprocs, int nthreads, int ngpus){

  int nvidia_gpus = 0;
  int host = 1;

  if(ngpus != 0 && nthreads != ngpus)
    pe_fatal(pe, "Select nthreads = ngpus when running on GPUs.");

#if _OPENACC
  nvidia_gpus = acc_get_num_devices(acc_device_nvidia);
  if(acc_get_device_type() != acc_device_host) host=0;
#endif

  if(!host && (ngpus > nvidia_gpus))
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
 *  dc_nnodes_set
 *
 *****************************************************************************/

int dc_nnodes_set(dc_t *dc, int nnodes){

  assert(dc);

  dc->nnodes = nnodes;

  return 0;
}

/*****************************************************************************
 *
 *  dc_nnodes
 *
 *****************************************************************************/

int dc_nnodes(dc_t *dc, int *nnodes){

  assert(dc);

  *nnodes = dc->nnodes;

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
