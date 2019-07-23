#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "logistic_regression.h"
#include "memory.h"

struct lr_s{
  data_t *data;
  precision *dot;
  precision lhood;
  int dim;
  int N;
};

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

  mem_malloc_precision(&lr->dot, lr->dim+1);
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

  int i,j;
  precision *x = NULL;
  int *y = NULL;

  data_x(lr->data, &x);
  data_y(lr->data, &y);

  assert(lr);

  // TIMER_start(TIMER_LIKELIHOOD);

  lr->lhood = 0.0;
  for(i=0; i<lr->N; i++)
  {
    lr->dot[i] = 0.0;
  }

  for(i=0; i<lr->N; i++)
  {
    for(j=0; j<lr->dim; j++)
    {
      lr->dot[i] += sample[j] * x[i*lr->dim+j];
    }
  }

  for(i=0; i<lr->N; i++)
  {
    lr->lhood -= log(1 + exp(-y[i] * lr->dot[i]));
  }

  // TIMER_stop(TIMER_LIKELIHOOD);

  return lr->lhood;
}
