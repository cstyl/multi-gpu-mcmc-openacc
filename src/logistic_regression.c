#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "logistic_regression.h"
#include "decomposition.h"
#include "memory.h"
#include "timer.h"

#define REST __restrict__

#ifdef _FLOAT_
  MPI_Datatype MPI_PRECISION = MPI_FLOAT;
#else
  MPI_Datatype MPI_PRECISION = MPI_DOUBLE;
#endif

struct lr_s{
  pe_t *pe;
  dc_t *dc;
  data_t *data;
  precision *dot;
  precision lhood;
  int dim;
  int N;
  MPI_Comm comm;
  int rank;
  int nprocs;
  int nthreads;
};

void lr_create_device_dot(lr_t *lr);
void lr_free_device_dot(lr_t *lr);

void mvmul(lr_t *REST lr, precision *REST x, precision *REST sample);
precision reduce_lhood(lr_t *REST lr, int *REST y);

/*****************************************************************************
*
*  lr_lhood_create
*
*****************************************************************************/

int lr_lhood_create(pe_t *pe, data_t *data, lr_t **plr){

  lr_t *lr = NULL;

  assert(pe);
  assert(data);

  lr = (lr_t *) calloc(1, sizeof(lr_t));
  assert(lr);
  if(lr == NULL) pe_fatal(pe, "calloc(lr_t) failed\n");

  lr->data = data;
  lr->pe = pe;

  data_dimx(data, &lr->dim);
  data_N(data, &lr->N);
  data_dc(lr->data, &lr->dc);

  dc_nprocs(lr->dc, &lr->nprocs);
  dc_nthreads(lr->dc, &lr->nthreads);

  lr->rank = pe_mpi_rank(lr->pe);
  pe_mpi_comm(lr->pe, &lr->comm);

  mem_malloc_precision(&lr->dot, lr->N);
  lr_create_device_dot(lr);

  *plr = lr;

  return 0;
}

/*****************************************************************************
*
*  lr_lhood_free
*
*****************************************************************************/

int lr_lhood_free(lr_t *lr){

  assert(lr);

  lr_free_device_dot(lr);
  mem_free((void**)&lr->dot);

  mem_free((void**)&lr);

  return 0;
}

/*****************************************************************************
*
*  lr_lhood
*  evaluates logistic regression likelihood in log-domain
*  L_n(theta) = 1 / (1 + exp(-y_n * theta^T * x_n)) where y_n={-1,1}
*  log(L_n(theta)) = - log(1 + exp(-y_n * theta^T * x_n)
*
*****************************************************************************/

precision lr_lhood(lr_t *lr, precision *sample){

  assert(lr);

  precision lhood = 0.0f;
  precision *x = NULL;
  int * y = NULL;

  data_x(lr->data, &x);
  data_y(lr->data, &y);

  TIMER_start(TIMER_LIKELIHOOD);
  // pe_verbose(lr->pe, "starting mvmul\n");
  mvmul(lr, x, sample);
  // pe_verbose(lr->pe, "mvmul completed\n");
  lhood = reduce_lhood(lr, y);
  // pe_verbose(lr->pe, "red completed\n");
  TIMER_stop(TIMER_LIKELIHOOD);

  return lhood;
}

void mvmul(lr_t *REST lr, precision *REST x, precision *REST sample){

  precision *REST dot = lr->dot;
  int *tlow = NULL, *thi = NULL;
  int dim = lr->dim;
  int i,j;

  dc_tbound(lr->dc, &tlow, &thi);
  int nthreads=lr->nthreads;

  TIMER_start(TIMER_MATVECMUL);

  #pragma omp parallel default(shared) private(i, j) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    int gpuid = tid + lr->nthreads*(lr->rank%lr->nprocs);
    int low = tlow[tid], hi = thi[tid];
    /* Switch to the appropriate device and allocate memory on it */
    #pragma acc set device_num(gpuid) device_type(acc_device_nvidia)
    #pragma acc kernels present(dot[low:hi]) \
                        present(sample[:dim]) \
                        present(x[low*dim:hi*dim])
    {
      #pragma acc loop
      for(i=low; i<hi; i++)
      {
        precision dot_local = 0.0f;
        #pragma acc loop seq
        for(j=0; j<dim; j++)
        {
          dot_local += sample[j] * x[i*dim+j];
        }
        dot[i] = dot_local;
      }
    }
  }

  TIMER_stop(TIMER_MATVECMUL);
}

precision reduce_lhood(lr_t *REST lr, int *REST y){

  precision *REST dot = lr->dot;
  int *tlow = NULL, *thi = NULL;
  precision global_lhood = 0.0f, lhood = 0.0f;
  int i;

  dc_tbound(lr->dc, &tlow, &thi);
  int nthreads=lr->nthreads;

  TIMER_start(TIMER_REDUCE);

  #pragma omp parallel default(shared) private(i) num_threads(nthreads) reduction(+:lhood)
  {
    int tid = omp_get_thread_num();
    int gpuid = tid + lr->nthreads*(lr->rank%lr->nprocs);
    int low = tlow[tid], hi = thi[tid];
    precision lhood_d = 0.0;
    /* Switch to the appropriate device and allocate memory on it */
    #pragma acc set device_num(gpuid) device_type(acc_device_nvidia)
    #pragma acc kernels present(dot[low:hi], y[low:hi]) \
                        copyout(lhood_d)
    {
      lhood_d = 0.0f;
      #pragma acc loop reduction(+:lhood_d)
      for(i=low; i<hi; i++)
      {
        lhood_d -= log(1.0f + exp(-(precision)y[i] * dot[i]));
      }
    }
    lhood += lhood_d;
  }
  MPI_Allreduce(&lhood, &global_lhood, 1, MPI_PRECISION, MPI_SUM, lr->comm);

  TIMER_stop(TIMER_REDUCE);

  return global_lhood;
}

/*****************************************************************************
*
*  lr_create_device_dot
*
*****************************************************************************/

void lr_create_device_dot(lr_t *lr){

  TIMER_start(TIMER_CREATE_DOT);

  int *tlow = NULL, *thi = NULL;
  precision *dot = lr->dot;
  int nthreads = lr->nthreads;

  dc_tbound(lr->dc, &tlow, &thi);

  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    int gpuid = tid + lr->nthreads*(lr->rank%lr->nprocs);
    int low = tlow[tid], hi = thi[tid];
    /* Switch to the appropriate device and allocate memory on it */
    #pragma acc set device_num(gpuid) device_type(acc_device_nvidia)
    #pragma acc enter data create(dot[low:hi])
  }

  TIMER_stop(TIMER_CREATE_DOT);
}

/*****************************************************************************
*
*  lr_free_device_dot
*
*****************************************************************************/

void lr_free_device_dot(lr_t *lr){

  precision *dot = lr->dot;
  int nthreads=lr->nthreads;

  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    int gpuid = tid + lr->nthreads*(lr->rank%lr->nprocs);
    /* Switch to the appropriate device and allocate memory on it */
    #pragma acc set device_num(gpuid) device_type(acc_device_nvidia)
    #pragma acc exit data delete(dot[:1])
  }

}

/*****************************************************************************
*
*  lr_logistic_regression
*
*****************************************************************************/

precision lr_logistic_regression(precision *sample, precision *x, int dim){

  assert(sample);
  assert(x);

  int i;
  precision probability, dot=0.0;

  TIMER_start(TIMER_LOGISTIC_REGRESSION);

  for(i=0; i<dim; i++)
  {
    dot += sample[i] * x[i];
  }

  probability = 1.0 / (1.0 + exp(-dot));

  TIMER_stop(TIMER_LOGISTIC_REGRESSION);

  return probability;
}

int lr_data(lr_t *lr, data_t **pdata){

  assert(lr);

  *pdata = lr->data;

  return 0;
}

int lr_dot(lr_t *lr, precision **pdot){

  assert(lr);

  *pdot = lr->dot;

  return 0;
}

int lr_dim(lr_t *lr, int *dim){

  assert(lr);

  *dim = lr->dim;

  return 0;
}

int lr_N(lr_t *lr, int *N){

  assert(lr);

  *N = lr->N;

  return 0;
}
