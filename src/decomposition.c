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
  int nprocs;      /* Number of MPI processes per node */
  int work;
  int plow;        /* Local lower MPI bound of a datapoint */
  int phi;         /* Local upper MPI bound of a datapoint */
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

  *pdc = dc;

  return 0;
}

int dc_free(dc_t *dc){

  assert(dc);

  mem_free((void**)&dc);

  return 0;
}

int dc_init_rt(pe_t *pe, rt_t *rt, dc_t *dc){

  int nnodes, nprocs;

  assert(pe);
  assert(rt);

  if(rt_int_parameter(rt, "nprocs", &nprocs))
  {
    dc_nprocs_set(dc, nprocs);
  }

  int gpuid = dc->rank%dc->nprocs;
  #pragma acc set device_num(gpuid) device_type(acc_device_nvidia)

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
  pe_info(pe, "%30s\t\t%d\n", "Number of Processes/node:", dc->nprocs);

  return 0;
}

int dc_decompose(dc_t *dc){

  assert(dc);

  /* Decompose over MPI-processes */
  dc_splitwork(dc->work, dc->size, dc->rank, &dc->plow, &dc->phi);

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
