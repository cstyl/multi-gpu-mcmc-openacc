#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cblas.h"
#include "lapacke.h"

#include "multivariate_normal.h"
#include "memory.h"
#include "ran.h"

struct mvnb_s{
  precision *covariance;
  precision *L;
  int dim;
  precision rwsd_block;
  int tune;
};

static int mvn_block_allocate_covariance(mvnb_t *mvnb);
static int mvn_block_allocate_L(mvnb_t *mvnb);

/*****************************************************************************
 *
 *  mvn_block_create
 *
 *****************************************************************************/

int mvn_block_create(pe_t *pe, mvnb_t **pmvnb){

  mvnb_t *mvnb = NULL;

  assert(pe);

  mvnb = (mvnb_t *) calloc(1, sizeof(mvnb_t));
  assert(mvnb);
  if(mvnb == NULL) pe_fatal(pe, "calloc(mvnb_t) failed\n");

  mvn_block_dim_set(mvnb, DIMX_DEFAULT);
  mvn_block_rwsd_set(mvnb, RWSD_DEFAULT);
  mvn_block_tune_set(mvnb, TUNE_DEFAULT);

  *pmvnb = mvnb;

  return 0;
}

/*****************************************************************************
 *
 *  mvn_block_free
 *
 *****************************************************************************/

int mvn_block_free(mvnb_t *mvnb){

  assert(mvnb);

  mem_free((void**)&mvnb->covariance);
  mem_free((void**)&mvnb->L);

  mem_free((void**)&mvnb);

  return(0);
}

int mvn_block_init_rt(rt_t *rt, mvnb_t *mvnb){

  int dim, tune;

  assert(rt);
  assert(mvnb);

  mvn_block_rwsd_set(mvnb, 2.38 / sqrt(DIMX_DEFAULT - 1));

  if(rt_int_parameter(rt, "sample_dim", &dim))
  {
    mvn_block_dim_set(mvnb, dim);
    mvn_block_rwsd_set(mvnb, 2.38 / sqrt(dim - 1));
  }

  if(rt_int_parameter(rt, "tune_sd", &tune))
  {
    mvn_block_tune_set(mvnb, tune);
  }

  mvn_block_allocate_covariance(mvnb);
  mvn_block_allocate_L(mvnb);

  return 0;
}

/*****************************************************************************
 *
 *  mvn_block_init
 *
 *****************************************************************************/

int mvn_block_init(mvnb_t *mvnb){

  assert(mvnb);
  /* Ensure the serial version of OpenBlas is used */
  openblas_set_num_threads(1);
  mvn_block_init_covariance(mvnb);
  mvn_block_cholesky_decomp(mvnb);

  return 0;
}

/*****************************************************************************
 *
 *  mvn_block_init_covariance
 *
 *****************************************************************************/

int mvn_block_init_covariance(mvnb_t *mvnb){

  int i;

  assert(mvnb);

  /* Generate Covariance Matrix
   */
   memset(mvnb->covariance, 0, mvnb->dim*mvnb->dim*sizeof(double));
   /* Copy std on the diagonal */
   for(i=0; i<mvnb->dim; i++) mvnb->covariance[i*mvnb->dim+i] = mvnb->rwsd_block;

  return 0;
}

/*****************************************************************************
 *
 *  mvn_block_cholesky_decomp
 *
 *****************************************************************************/

int mvn_block_cholesky_decomp(mvnb_t *mvnb){

  int i,j;

  assert(mvnb);

  for(i=0; i<mvnb->dim; i++)
    for(j=0; j<mvnb->dim; j++)
      mvnb->L[i*mvnb->dim+j] = mvnb->covariance[i*mvnb->dim+j];

  /* Perform cholesky decomposition
   * Factorize the symmetric, positive-definite covariance square matrix
   * Utilizes LAPACKE_dpotrf or LAPACKE_spotrf depending the precision setup
   */
   POTRF(LAPACK_ROW_MAJOR, 'L', mvnb->dim, mvnb->L, mvnb->dim);

  return 0;
}

/*****************************************************************************
 *
 *  mvn_block_sample
 *
 *****************************************************************************/

int mvn_block_sample(mvnb_t *mvnb, precision *cur, precision *pro){

  int i;

  assert(mvnb);
  assert(pro);
  assert(cur);

  int dim = mvnb->dim;
  precision *L = mvnb->L;

  /* Sample from standard normal distribution */
  for(i=0; i<dim; i++) pro[i] = ran_serial_gaussian();

  /* Triangular matrix to vector multiplication
  *  using the cholesky decomposed covariance matrix
  */
  TRMV(CblasRowMajor, CblasLower, CblasNoTrans, CblasNonUnit, dim, L, dim, pro, 1);

  for(i=0; i<dim; i++) pro[i] += cur[i];

  return 0;
}

int mvn_block_dim_set(mvnb_t *mvnb, int dim){

  assert(mvnb);

  mvnb->dim = dim;

  return 0;
}

int mvn_block_rwsd_set(mvnb_t *mvnb, precision rwsd){

  assert(mvnb);

  mvnb->rwsd_block = rwsd;

  return 0;
}

int mvn_block_tune_set(mvnb_t *mvnb, int tune){

  assert(mvnb);

  mvnb->tune = tune;

  return 0;
}

int mvn_block_dim(mvnb_t *mvnb, int *dim){

  assert(mvnb);

  *dim = mvnb->dim;

  return 0;
}

int mvn_block_rwsd(mvnb_t *mvnb, precision *rwsd){

  assert(mvnb);

  *rwsd = mvnb->rwsd_block;

  return 0;
}

int mvn_block_tune(mvnb_t *mvnb, int *tune){

  assert(mvnb);

  *tune = mvnb->tune;

  return 0;
}

int mvn_block_covariance(mvnb_t *mvnb, precision **covariance){

  assert(mvnb);

  *covariance = mvnb->covariance;

  return 0;
}

int mvn_block_L(mvnb_t *mvnb, precision **L){

  assert(mvnb);

  *L = mvnb->L;

  return 0;
}

static int mvn_block_allocate_covariance(mvnb_t *mvnb){

  assert(mvnb);

  mem_malloc_precision(&mvnb->covariance, mvnb->dim*mvnb->dim);

  return 0;
}

static int mvn_block_allocate_L(mvnb_t *mvnb){

   assert(mvnb);

   mem_malloc_precision(&mvnb->L, mvnb->dim*mvnb->dim);

   return 0;
 }
