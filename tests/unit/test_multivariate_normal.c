#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "pe.h"
#include "multivariate_normal.h"
#include "tests.h"

static int test_mvn_block_check_rt_default(pe_t *pe);
static int test_mvn_block_check_rt(pe_t *pe);
static int test_mvn_block_check_init_covariance(pe_t *pe);
static int test_mvn_block_check_cholesky_decomposition(pe_t *pe);
static int test_mvn_block_check_init(pe_t *pe);
static int test_mvn_block_check_sample(pe_t *pe);

int test_mvn_suite(void){

  pe_t *pe = NULL;

  pe_create(MPI_COMM_WORLD, PE_QUIET, &pe);
  assert(pe);
  test_assert(1);

  test_mvn_block_check_rt_default(pe);
  test_mvn_block_check_rt(pe);
  test_mvn_block_check_init_covariance(pe);
  test_mvn_block_check_cholesky_decomposition(pe);
  test_mvn_block_check_init(pe);
  test_mvn_block_check_sample(pe);

  pe_info(pe, "PASS\t./unit/test_multivariate_normal\n");
  pe_free(pe);

  return 0;
}

static int test_mvn_block_check_rt_default(pe_t *pe){

  int dim_ref=3, tune_ref=0;
  int dim, tune;
  precision rwsd_ref = 2.38 / sqrt(dim_ref - 1);
  precision rwsd;

  rt_t *rt = NULL;
  mvnb_t *mvnb = NULL;

  rt_create(pe, &rt);
  assert(rt);

  mvn_block_create(pe, &mvnb);
  assert(mvnb);
  test_assert(1);

  mvn_block_dim(mvnb, &dim);
  test_assert(dim == dim_ref);
  mvn_block_tune(mvnb, &tune);
  test_assert(tune == tune_ref);
  mvn_block_rwsd(mvnb, &rwsd);
  test_assert(fabs(rwsd - rwsd_ref) < TEST_PRECISION_TOLERANCE);

  mvn_block_free(mvnb);
  rt_free(rt);

  return 0;
}

static int test_mvn_block_check_rt(pe_t *pe){

  int dim_ref=5, tune_ref=1;
  precision rwsd_ref = 2.38 / sqrt(dim_ref - 1);
  int dim, tune;
  precision rwsd;
  int dim_rt=3, tune_rt=0;
  precision rwsd_rt = 2.38 / sqrt(dim_rt - 1);
  precision *L = NULL;
  precision *covariance = NULL;

  rt_t *rt = NULL;
  mvnb_t *mvnb = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  mvn_block_create(pe, &mvnb);
  assert(mvnb);
  test_assert(1);

  mvn_block_dim_set(mvnb, dim_ref);
  mvn_block_tune_set(mvnb, tune_ref);
  mvn_block_rwsd_set(mvnb, rwsd_ref);

  mvn_block_init_rt(rt, mvnb);

  mvn_block_dim(mvnb, &dim);
  test_assert(dim_rt == dim);
  mvn_block_tune(mvnb, &tune);
  test_assert(tune_rt == tune);
  mvn_block_rwsd(mvnb, &rwsd);
  test_assert(fabs(rwsd_rt - rwsd) < TEST_PRECISION_TOLERANCE);
  mvn_block_covariance(mvnb, &covariance);
  test_assert(covariance != NULL);
  mvn_block_L(mvnb, &L);
  test_assert(L != NULL);

  mvn_block_free(mvnb);
  rt_free(rt);

  return 0;
}

static int test_mvn_block_check_init_covariance(pe_t *pe){

  int dim=3, i;
  precision rwsd = 2.38 / sqrt(dim - 1);
  precision *covariance = NULL;

  rt_t *rt = NULL;
  mvnb_t *mvnb = NULL;

  precision covariance_ref[9] = {rwsd, 0.0 , 0.0 ,
                                 0.0 , rwsd, 0.0 ,
                                 0.0 , 0.0 , rwsd};

  rt_create(pe, &rt);
  assert(rt);

  mvn_block_create(pe, &mvnb);
  assert(mvnb);
  test_assert(1);

  mvn_block_init_rt(rt, mvnb);
  mvn_block_init(mvnb);

  mvn_block_covariance(mvnb, &covariance);

  for(i=0; i<dim*dim; i++)
    test_assert(fabs(covariance[i] - covariance_ref[i]) < TEST_PRECISION_TOLERANCE);

  mvn_block_free(mvnb);

  return 0;
}

static int test_mvn_block_check_cholesky_decomposition(pe_t *pe){

  int dim=3, i;
  precision rwsd = 2.38 / sqrt(dim - 1);
  precision *L = NULL;

  rt_t *rt = NULL;
  mvnb_t *mvnb = NULL;

  rt_create(pe, &rt);
  assert(rt);

  precision L_ref[9] = {sqrt(rwsd), 0.0 , 0.0 ,
                        0.0 , sqrt(rwsd), 0.0 ,
                        0.0 , 0.0 , sqrt(rwsd)};

  mvn_block_create(pe, &mvnb);
  assert(mvnb);
  test_assert(1);

  mvn_block_init_rt(rt, mvnb);
  mvn_block_init(mvnb);

  mvn_block_L(mvnb, &L);

  for(i=0; i<dim*dim; i++)
    test_assert(fabs(L[i] - L_ref[i]) < TEST_PRECISION_TOLERANCE);

  mvn_block_free(mvnb);
  rt_free(rt);

  return 0;
}

static int test_mvn_block_check_init(pe_t *pe){

  int dim=3, i;
  precision rwsd = 2.38 / sqrt(dim - 1);
  precision *L = NULL;
  precision *covariance = NULL;

  rt_t *rt = NULL;
  mvnb_t *mvnb = NULL;

  rt_create(pe, &rt);
  assert(rt);

  precision covariance_ref[9] = {rwsd, 0.0 , 0.0 ,
                                 0.0 , rwsd, 0.0 ,
                                 0.0 , 0.0 , rwsd};

  precision L_ref[9] = {sqrt(rwsd), 0.0 , 0.0 ,
                        0.0 , sqrt(rwsd), 0.0 ,
                        0.0 , 0.0 , sqrt(rwsd)};

  mvn_block_create(pe, &mvnb);
  assert(mvnb);
  test_assert(1);

  mvn_block_init_rt(rt, mvnb);
  mvn_block_init(mvnb);

  mvn_block_covariance(mvnb, &covariance);

  for(i=0; i<dim*dim; i++)
    test_assert(fabs(covariance[i] - covariance_ref[i]) < TEST_PRECISION_TOLERANCE);

  mvn_block_L(mvnb, &L);

  for(i=0; i<dim*dim; i++)
    test_assert(fabs(L[i] - L_ref[i]) < TEST_PRECISION_TOLERANCE);

  mvn_block_free(mvnb);
  rt_free(rt);

  return 0;
}

static int test_mvn_block_check_sample(pe_t *pe){

  int dim=3, i, j;
  precision rwsd = 2.38 / sqrt(dim - 1);

  precision pro[3] = {0.0, 0.0, 0.0};
  precision cur[3] = {0.0, 0.0, 0.0};

  precision pro_ref[3] = {0.0, 0.0, 0.0};
  precision L_ref[9] = {sqrt(rwsd), 0.0 , 0.0 ,
                        0.0 , sqrt(rwsd), 0.0 ,
                        0.0 , 0.0 , sqrt(rwsd)};
  /*
   * Default gaussian samples generated by ran_serial_gaussian.
   * Assumes default seed of 7361237
   */
  precision gauss_ref[3] = {-0.2735724136879419, 0.2894534503883834, 0.2287926867838626};
  precision dot[3] = {0.0, 0.0, 0.0};

  rt_t *rt = NULL;
  mvnb_t *mvnb = NULL;

  rt_create(pe, &rt);
  assert(rt);
  mvn_block_create(pe, &mvnb);
  assert(mvnb);
  test_assert(1);

  mvn_block_init_rt(rt, mvnb);
  mvn_block_init(mvnb);

  /* Evaluate reference sample */
  for(i=0; i<dim; i++)
  {
      pro_ref[i] = cur[i] + gauss_ref[i];
  }

  for(i=0; i<dim; i++)
  {
    for(j=0; j<dim; j++)
    {
      dot[i] += pro_ref[j] * L_ref[i*dim+j];
    }
  }

  for(i=0; i<dim; i++)
  {
      pro_ref[i] = dot[i];
  }

  /* Generate actual sample*/
  mvn_block_sample(mvnb, cur, pro);

  for(i=0; i<dim; i++)
    test_assert(fabs(pro[i] - pro_ref[i]) < TEST_PRECISION_TOLERANCE);

  mvn_block_free(mvnb);
  rt_free(rt);


  return 0;
}
