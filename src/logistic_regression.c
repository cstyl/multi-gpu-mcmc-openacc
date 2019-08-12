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

struct lr_s{
  data_t *data;
  precision *dot;
  precision lhood;
  int dim;
  int N;
};

// extern void GEMV(char trans, int m, int n,
//                  const precision alpha, const precision *A, int lda,
//                  const precision *x, int incx, const precision  beta,
//                  precision *y, int incy);

void lr_create_device_dot(precision *dot, int start, int end);
void mvmul(int rows, int cols, precision *REST mat, precision *REST vec, precision *REST dot);
precision reduce_lhood(precision *REST dot, int *REST y, int n);
// void nvidia_mvmul(int rows, int cols, precision *REST mat, precision *REST vec, precision *REST dot);

void lr_create_device_dot(precision *dot, int start, int end){
  TIMER_start(TIMER_CREATE_DOT);
  #pragma acc enter data create(dot[start:end])
  TIMER_stop(TIMER_CREATE_DOT);
}

void lr_free_device_dot(precision *dot){
  #pragma acc exit data delete(dot[:1])
}

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

  data_dimx(data, &lr->dim);
  data_N(data, &lr->N);

  mem_malloc_precision(&lr->dot, lr->N);

  int nthreads;
  int *tlow = NULL, *thi = NULL;
  dc_t *dc = NULL;
  data_dc(lr->data, &dc);
  dc_nthreads(dc, &nthreads);
  dc_tbound(dc, &tlow, &thi);

  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    int size = thi[tid] - tlow[tid];
    /* Switch to the appropriate device and allocate memory on it */
    #pragma acc set device_num(tid) device_type(acc_device_nvidia)
    lr_create_device_dot(lr->dot, 0, size);
  }

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

  dc_t *dc = NULL;
  data_dc(lr->data, &dc);
  int nthreads;

  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    /* Switch to the appropriate device and allocate memory on it */
    #pragma acc set device_num(tid) device_type(acc_device_nvidia)
    lr_free_device_dot(lr->dot);
  }

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

  precision *x = NULL;
  int * y = NULL;
  int dim = lr->dim, N = lr->N;
  precision lhood = 0.0f;
  dc_t *dc = NULL;

  data_x(lr->data, &x);
  data_y(lr->data, &y);
  data_dc(lr->data, &dc);

  int nthreads;
  int *tlow = NULL, *thi = NULL;
  dc_nthreads(dc, &nthreads);
  dc_tbound(dc, &tlow, &thi);

  TIMER_start(TIMER_LIKELIHOOD);

  TIMER_start(TIMER_MATVECMUL);

  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    int size = thi[tid] - tlow[tid];
    /* Switch to the appropriate device and allocate memory on it */
    #pragma acc set device_num(tid) device_type(acc_device_nvidia)
    mvmul(size, dim, &x[tlow[tid]*dim], sample, lr->dot);
  }

  // mvmul(lr->N, lr->dim, x, sample, lr->dot);
  // nvidia_mvmul(lr->N, lr->dim, x, sample, lr->dot);

  TIMER_stop(TIMER_MATVECMUL);

  TIMER_start(TIMER_REDUCE);
  #pragma omp parallel default(shared) num_threads(nthreads) reduction(+:lhood)
  {
    int tid = omp_get_thread_num();
    int size = thi[tid] - tlow[tid];
    /* Switch to the appropriate device and allocate memory on it */
    #pragma acc set device_num(tid) device_type(acc_device_nvidia)
    lhood = reduce_lhood(lr->dot, &y[tlow[tid]], size);
  }
  // lhood = reduce_lhood(lr->dot, y, lr->N);
  TIMER_stop(TIMER_REDUCE);

  TIMER_stop(TIMER_LIKELIHOOD);

  return lhood;
}

void mvmul(int rows, int cols, precision *REST mat, precision *REST vec, precision *REST dot){

  int i, j;

  #pragma acc kernels present(dot[:rows]) \
                      present(vec[:cols]) \
                      present(mat[:rows*cols])
  {
    #pragma acc loop
    for(i=0; i<rows; i++)
    {
      precision dot_local = 0.0f;
      #pragma acc loop seq
      for(j=0; j<cols; j++)
      {
        dot_local += vec[j] * mat[i*cols+j];
      }
      dot[i] = dot_local;
    }
  }

}

// void nvidia_mvmul(int rows, int cols, precision *REST mat, precision *REST vec, precision *REST dot){
//
//   precision alpha = 1.0f, beta = 0.0f;
//
//   #pragma acc data present(mat[:rows*cols],vec[:cols],dot[:rows])
//   #pragma acc host_data use_device(mat, vec, dot)
//   GEMV('T', cols, rows, &alpha, mat, cols, vec, 1, &beta, dot, 1);
// }

precision reduce_lhood(precision *REST dot, int *REST y, int n){

  int i;
  precision lhood = 0.0f;

  #pragma acc kernels present(dot[:n], y[:n]) \
                      copyout(lhood)
  {
    lhood = 0.0f;
    #pragma acc loop reduction(+:lhood)
    for(i=0; i<n; i++)
    {
      lhood -= log(1.0f + exp(-(precision)y[i] * dot[i]));
    }
  }

  return lhood;
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
