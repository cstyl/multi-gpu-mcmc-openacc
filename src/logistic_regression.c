#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "logistic_regression.h"
#include "memory.h"
#include <gsl/gsl_cblas.h>    // to perform dot products and vector operations

struct lh_s{
  precision lhood;
  precision *dot;
  int N;
};

/*****************************************************************************
*
*  lr_lhood_create
*
*****************************************************************************/

int lr_lhood_create(int N, lh_t **plh){

  lh_t *lh = NULL;

  lh = (lh_t *) calloc(1, sizeof(lh_t));
  assert(lh);
  if(lh == NULL)
  {
    printf("calloc(lh_t) failed\n");
    exit(1);
  }

  lh->N = N;
  mem_malloc_precision(&lh->dot, lh->N);

  *plh = lh;

  return 0;
}

/*****************************************************************************
*
*  lr_lhood_free
*
*****************************************************************************/

int lr_lhood_free(lh_t *lh){

  assert(lh);

  if(lh->dot) free(lh->dot);

  free(lh);

  assert(lh->dot != NULL);
  assert(lh != NULL);

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

precision lr_lhood(lh_t *lh, precision *sample, data_t *data){

  cmd_data_t *params = NULL;

  assert(sample);
  assert(data);

  params = data->params;
  int i,j;

  lh->lhood = 0.0;
  for(i=0; i<params->N; i++)
  {
    lh->dot[i] = 0.0;
    for(j=0; j<(params->dim+1); j++)
    {
      lh->dot[i] += sample[j] * data->x[i*(params->dim+1)+j];
    }
    lh->lhood -= log(1 + exp(-data->y[i] * lh->dot[i]));
  }

  return lh->lhood;
}

/*****************************************************************************
*
*  lr_lhood_dot
*
*****************************************************************************/

precision lr_lhood_dot(lh_t *lh, precision *sample, data_t *data){

  cmd_data_t *params = NULL;

  assert(sample);
  assert(data);

  params = data->params;
  int i;

  lh->lhood = 0.0;
  for(i=0; i<params->N; i++)
  {
#if FLOAT
    lh->dot[i] = cblas_sdot((params->dim+1), &data->x[i*(params->dim+1)], 1, sample, 1);
#else
    lh->dot[i] = cblas_ddot((params->dim+1), &data->x[i*(params->dim+1)], 1, sample, 1);
#endif
    lh->lhood -= log(1 + exp(-data->y[i] * lh->dot[i]));
  }

  return lh->lhood;
}

/*****************************************************************************
*
*  lr_lhood_mv
*
*****************************************************************************/

precision lr_lhood_mv(lh_t *lh, precision *sample, data_t *data){

  cmd_data_t *params = NULL;

  assert(sample);
  assert(data);

  params = data->params;
  int i;
  precision alpha = 1.0, beta=0.0;
  int m=params->N, n=(params->dim+1), lda=params->N, incx=1, incy=1;

#if FLOAT
  cblas_sgemv(CblasColMajor, CblasNoTrans, m, n, alpha, data->x, lda,
              sample, incx, beta, lh->dot, incy);
#else
  cblas_dgemv(CblasColMajor, CblasNoTrans, m, n, alpha, data->x, lda,
              sample, incx, beta, lh->dot, incy);
#endif

  lh->lhood = 0.0;
  for(i=0; i<params->N; i++){
    lh->lhood -= log(1+exp(-data->y[i] * lh->dot[i]));
  }

  return lh->lhood;
}
