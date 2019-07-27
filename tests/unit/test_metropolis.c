#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "definitions.h"
#include "pe.h"
#include "runtime.h"
#include "metropolis.h"
#include "prior.h"
#include "tests.h"

static int test_metropolis_create(pe_t *pe);
static int test_metropolis_rt_default(pe_t *pe);
static int test_metropolis_rt(pe_t *pe);
static int test_metropolis_init_zero(pe_t *pe);
static int test_metropolis_init_rand(pe_t *pe);
static int test_metropolist_init_post_burn(pe_t *pe);
static int test_metropolis_run(pe_t *pe);

int test_metropolis_suite(void){

  pe_t *pe = NULL;

  pe_create(MPI_COMM_WORLD, PE_QUIET, &pe);
  assert(pe);
  test_assert(1);

  test_metropolis_create(pe);
  test_metropolis_rt_default(pe);
  test_metropolis_rt(pe);
  test_metropolis_init_zero(pe);
  test_metropolis_init_rand(pe);
  test_metropolist_init_post_burn(pe);
  test_metropolis_run(pe);

  pe_info(pe, "PASS\t./unit/test_metropolis\n");
  pe_free(pe);

  return 0;
}

static int test_metropolis_create(pe_t *pe){

  assert(pe);

  int rand_init;

  rt_t *rt = NULL;
  met_t *met = NULL;
  ch_t *chain = NULL;
  data_t *data = NULL;
  mvnb_t *mvnb = NULL;
  lr_t *lr = NULL;
  sample_t *cur = NULL;
  sample_t *pro = NULL;

  rt_create(pe, &rt);
  assert(rt);
  ch_create(pe, &chain);
  assert(chain);

  met_create(pe, chain, &met);
  assert(met);
  test_assert(1);

  met_random_init(met, &rand_init);
  test_assert(rand_init == 0);

  /* Check that upon met creation only the chain structures are assigned */
  met_chain(met, &chain);
  test_assert(chain != NULL);

  met_data(met, &data);
  test_assert(data == NULL);
  met_mvnb(met, &mvnb);
  test_assert(mvnb == NULL);
  met_lr(met, &lr);
  test_assert(lr == NULL);

  met_current(met, &cur);
  test_assert(cur == NULL);
  met_proposed(met, &pro);
  test_assert(pro == NULL);

  met_free(met);
  ch_free(chain);
  rt_free(rt);

  return 0;

}

static int test_metropolis_rt_default(pe_t *pe)
{

  assert(pe);

  int rand_init;

  rt_t *rt = NULL;
  met_t *met = NULL;
  ch_t *chain = NULL;
  data_t *data = NULL;
  mvnb_t *mvnb = NULL;
  lr_t *lr = NULL;
  sample_t *cur = NULL;
  sample_t *pro = NULL;

  rt_create(pe, &rt);
  assert(rt);
  ch_create(pe, &chain);
  assert(chain);

  met_create(pe, chain, &met);
  assert(met);
  test_assert(1);

  met_random_init(met, &rand_init);
  test_assert(rand_init == 0);

  met_init_rt(pe, rt, met);
  /* Check that the following structures were allocated upon initialising runtime */
  int cur_dim;
  precision *cur_values = NULL;

  met_current(met, &cur);
  test_assert(cur != NULL);
  sample_dim(cur, &cur_dim);
  test_assert(cur_dim == 3);
  sample_values(cur, &cur_values);
  test_assert(cur_values != NULL);

  int pro_dim;
  precision *pro_values = NULL;

  met_proposed(met, &pro);
  test_assert(pro != NULL);
  sample_dim(pro, &pro_dim);
  test_assert(pro_dim == 3);
  sample_values(pro, &pro_values);
  test_assert(pro_values != NULL);

  int d_dimx, d_dimy, d_N;
  char d_fx[FILENAME_MAX], d_fy[FILENAME_MAX];
  precision *d_x = NULL;
  int *d_y = NULL;

  met_data(met, &data);
  test_assert(data != NULL);
  data_dimx(data, &d_dimx);
  test_assert(d_dimx == 3);
  data_dimy(data, &d_dimy);
  test_assert(d_dimy == 1);
  data_N(data, &d_N);
  test_assert(d_N = 500);
  data_fx(data, d_fx);
  test_assert(strcmp(d_fx, "../data/synthetic/default/X_train.csv") == 0);
  data_fy(data, d_fy);
  test_assert(strcmp(d_fy, "../data/synthetic/default/Y_train.csv") == 0);
  data_x(data, &d_x);
  test_assert(d_x != NULL);
  data_y(data, &d_y);
  test_assert(d_y != NULL);

  int mvn_dim, mvn_tune;
  precision mvn_rwsd, rwsd_ref=2.38 / sqrt(3-1);
  met_mvnb(met, &mvnb);
  test_assert(mvnb != NULL);
  mvn_block_dim(mvnb, &mvn_dim);
  test_assert(mvn_dim == 3);
  mvn_block_tune(mvnb, &mvn_tune);
  test_assert(mvn_tune == 0);
  mvn_block_rwsd(mvnb, &mvn_rwsd);
  test_assert(fabs(mvn_rwsd - rwsd_ref) < TEST_PRECISION_TOLERANCE);

  int l_dim, l_N;
  precision *l_dot = NULL;
  data_t *l_data = NULL;

  met_lr(met, &lr);
  test_assert(lr != NULL);
  lr_dim(lr, &l_dim);
  test_assert(l_dim == 3);
  lr_N(lr, &l_N);
  test_assert(l_N == d_N);
  lr_data(lr, &l_data);
  test_assert(l_data != NULL);
  test_assert(l_data == data);
  lr_dot(lr, &l_dot);
  test_assert(l_dot != NULL);

  met_random_init(met, &rand_init); /* Default random init set to false */
  test_assert(rand_init == 0);

  met_free(met);
  ch_free(chain);
  rt_free(rt);

  return 0;
}

static int test_metropolis_rt(pe_t *pe)
{

  assert(pe);

  int rand_init;

  rt_t *rt = NULL;
  met_t *met = NULL;
  ch_t *chain = NULL;
  data_t *data = NULL;
  mvnb_t *mvnb = NULL;
  lr_t *lr = NULL;
  sample_t *cur = NULL;
  sample_t *pro = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  ch_create(pe, &chain);
  assert(chain);

  met_create(pe, chain, &met);
  assert(met);
  test_assert(1);

  met_random_init(met, &rand_init);
  test_assert(rand_init == 0);

  met_init_rt(pe, rt, met);
  /* Check that the following structures were allocated upon initialising runtime */
  int cur_dim;
  precision *cur_values = NULL;

  met_current(met, &cur);
  test_assert(cur != NULL);
  sample_dim(cur, &cur_dim);
  test_assert(cur_dim == 3);
  sample_values(cur, &cur_values);
  test_assert(cur_values != NULL);

  int pro_dim;
  precision *pro_values = NULL;

  met_proposed(met, &pro);
  test_assert(pro != NULL);
  sample_dim(pro, &pro_dim);
  test_assert(pro_dim == 3);
  sample_values(pro, &pro_values);
  test_assert(pro_values != NULL);

  int d_dimx, d_dimy, d_N;
  char d_fx[FILENAME_MAX], d_fy[FILENAME_MAX];
  precision *d_x = NULL;
  int *d_y = NULL;

  met_data(met, &data);
  test_assert(data != NULL);
  data_dimx(data, &d_dimx);
  test_assert(d_dimx == 3);
  data_dimy(data, &d_dimy);
  test_assert(d_dimy == 1);
  data_N(data, &d_N);
  test_assert(d_N = 10);
  data_fx(data, d_fx);
  test_assert(strcmp(d_fx, "./data/X_train.csv") == 0);
  data_fy(data, d_fy);
  test_assert(strcmp(d_fy, "./data/Y_train.csv") == 0);
  data_x(data, &d_x);
  test_assert(d_x != NULL);
  data_y(data, &d_y);
  test_assert(d_y != NULL);

  int mvn_dim, mvn_tune;
  precision mvn_rwsd, rwsd_ref=2.38 / sqrt(3-1);
  met_mvnb(met, &mvnb);
  test_assert(mvnb != NULL);
  mvn_block_dim(mvnb, &mvn_dim);
  test_assert(mvn_dim == 3);
  mvn_block_tune(mvnb, &mvn_tune);
  test_assert(mvn_tune == 0);
  mvn_block_rwsd(mvnb, &mvn_rwsd);
  test_assert(fabs(mvn_rwsd - rwsd_ref) < TEST_PRECISION_TOLERANCE);

  int l_dim, l_N;
  precision *l_dot = NULL;
  data_t *l_data = NULL;

  met_lr(met, &lr);
  test_assert(lr != NULL);
  lr_dim(lr, &l_dim);
  test_assert(l_dim == 3);
  lr_N(lr, &l_N);
  test_assert(l_N == d_N);
  lr_data(lr, &l_data);
  test_assert(l_data != NULL);
  test_assert(l_data == data);
  lr_dot(lr, &l_dot);
  test_assert(l_dot != NULL);

  met_random_init(met, &rand_init); /* Default random init set to false */
  test_assert(rand_init == 1);

  met_free(met);
  ch_free(chain);
  rt_free(rt);

  return 0;
}

static int test_metropolis_init_zero(pe_t *pe){

  assert(pe);

  int i,j;

  rt_t *rt = NULL;
  met_t *met = NULL;
  ch_t *chain = NULL;

  precision xref[30] = {1.0000000000000000,1.4648106502470808,-1.5701058145856399,
                        1.0000000000000000,1.5407782876057425,-0.5819483312184370,
                        1.0000000000000000,0.1415183428679135,0.5459865569896000,
                        1.0000000000000000,0.4670300843306030,1.1185017644967441,
                        1.0000000000000000,1.6517639463377001,1.5968332608405691,
                        1.0000000000000000,0.2072717426154768,-0.1663456615821221,
                        1.0000000000000000,1.4538306269217489,-0.0559761017281283,
                        1.0000000000000000,-1.2885503864798247,1.2163849967988494,
                        1.0000000000000000,1.6410274182494682,0.6013758387036505,
                        1.0000000000000000,1.5014949152091088,-1.4987275632154795};

  int yref[10] = {-1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000};

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");


  ch_create(pe, &chain);
  assert(chain);
  ch_init_chain_rt(rt, chain);

  met_create(pe, chain, &met);
  assert(met);
  test_assert(1);

  met_init_rt(pe, rt, met);
  /* Set random init to zero */
  met_random_init_set(met, 0);
  met_init(pe, met);

  /* Check if data are loaded */
  data_t *data = NULL;
  precision *x = NULL;
  int *y = NULL;
  int dim, N;
  met_data(met, &data);
  data_dimx(data, &dim);
  data_N(data, &N);
  data_x(data, &x);
  data_y(data, &y);

  for(i=0; i<N; i++){
    for(j=0; j<dim; j++){
        test_assert(fabs(x[i*dim+j] - xref[i*dim+j]) < TEST_PRECISION_TOLERANCE);
    }
    test_assert(fabs(y[i] - yref[i]) < TEST_PRECISION_TOLERANCE);
  }

  /* Check if sample initialised to zero */
  int s_dim;
  precision s_prior, s_lhood, s_posterior;
  precision *s_values = NULL;
  sample_t *sample = NULL;
  met_current(met, &sample);
  sample_dim(sample, &s_dim);
  sample_values(sample, &s_values);
  for(i=0; i<s_dim; i++)
    test_assert(fabs(s_values[i] - 0.0) < TEST_PRECISION_TOLERANCE);

  /* Check if the correct prior, lhood and posterior are appended to the sample */
  sample_prior(sample, &s_prior);
  test_assert(fabs(s_prior - -37.2955919945248056) < TEST_PRECISION_TOLERANCE);
  sample_likelihood(sample, &s_lhood);
  test_assert(fabs(s_lhood - -6.9314718055994531) < TEST_PRECISION_TOLERANCE);
  sample_posterior(sample, &s_posterior);
  test_assert(fabs(s_posterior - -44.2270638001242560) < TEST_PRECISION_TOLERANCE);

  /* Check if sample appended to chain and stats are set to zero for idx 0 */
  precision *samples = NULL;
  precision *probability = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;

  ch_samples(chain, &samples);
  for(i=0; i<s_dim; i++)
    test_assert(fabs(samples[0*s_dim+i] - 0.0) < TEST_PRECISION_TOLERANCE);

  ch_probability(chain, &probability);
  test_assert(fabs(probability[0] - 0.0) < TEST_PRECISION_TOLERANCE);
  ch_ratio(chain, &ratio);
  test_assert(fabs(ratio[0] - 0.0) < TEST_PRECISION_TOLERANCE);
  ch_accepted(chain, &accepted);
  test_assert(fabs(accepted[0] - 0.0) < TEST_PRECISION_TOLERANCE);

  met_free(met);
  ch_free(chain);
  rt_free(rt);

  return 0;
}

static int test_metropolis_init_rand(pe_t *pe){

  assert(pe);

  rt_t *rt = NULL;
  met_t *met = NULL;
  ch_t *chain = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  ch_create(pe, &chain);
  assert(chain);
  ch_init_chain_rt(rt, chain);

  met_create(pe, chain, &met);
  assert(met);
  test_assert(1);

  met_init_rt(pe, rt, met);

  met_init(pe, met);

  /* Check if sample is not set to zero */
  precision *s_values = NULL;
  sample_t *sample = NULL;
  met_current(met, &sample);
  sample_values(sample, &s_values);

  test_assert(fabs(s_values[0] - 0.6955185073873762) < TEST_PRECISION_TOLERANCE);
  test_assert(fabs(s_values[1] - -0.6888605195854478) < TEST_PRECISION_TOLERANCE);
  test_assert(fabs(s_values[2] - 0.7482249943117055) < TEST_PRECISION_TOLERANCE);

  met_free(met);
  ch_free(chain);
  rt_free(rt);

  return 0;
}

static int test_metropolist_init_post_burn(pe_t *pe){

  assert(pe);

  int i, dim;
  sample_t *sample = NULL;
  precision *values = NULL;
  precision *chain_samples = NULL;
  precision *probability = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;

  rt_t *rt = NULL;
  met_t *met = NULL;
  ch_t *burn = NULL;
  ch_t *chain = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  ch_create(pe, &burn);
  assert(burn);
  ch_init_burn_rt(rt, burn);

  ch_create(pe, &chain);
  assert(chain);
  ch_init_chain_rt(rt, chain);

  met_create(pe, burn, &met);
  assert(met);
  test_assert(1);

  met_init_rt(pe, rt, met);
  /* Assuming metropolis has been initialised with burn chain
  *  when post burn-in chain is initialised it should contain
  *  the current sample in met, and stats should be set to zero
  */
  met_init(pe, met);
  met_current(met, &sample);
  sample_dim(sample, &dim);
  sample_values(sample, &values);

  met_chain_set(met, chain);
  met_init_post_burn(pe, met);
  ch_samples(chain, &chain_samples);

  for(i=0; i<dim; i++)
    test_assert(fabs(chain_samples[0*dim+i] - values[i]) < TEST_PRECISION_TOLERANCE);

  ch_probability(chain, &probability);
  test_assert(fabs(probability[0] - 0.0) < TEST_PRECISION_TOLERANCE);
  ch_ratio(chain, &ratio);
  test_assert(fabs(ratio[0] - 0.0) < TEST_PRECISION_TOLERANCE);
  ch_accepted(chain, &accepted);
  test_assert(fabs(accepted[0] - 0.0) < TEST_PRECISION_TOLERANCE);

  met_free(met);
  ch_free(chain);
  ch_free(burn);
  rt_free(rt);

  return 0;
}

static int test_metropolis_run(pe_t *pe){

  assert(pe);

  int i, j, dim, N;
  precision ratio_ref;
  precision *samples = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;

  rt_t *rt = NULL;
  met_t *met = NULL;
  ch_t *burn = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  ch_create(pe, &burn);
  assert(burn);
  ch_init_burn_rt(rt, burn);

  met_create(pe, burn, &met);
  assert(met);
  test_assert(1);

  met_init_rt(pe, rt, met);
  met_init(pe, met);

  met_run(pe, met);

  ch_dim(burn, &dim);
  ch_N(burn, &N);
  ch_samples(burn, &samples);
  ch_ratio(burn, &ratio);
  ch_accepted(burn, &accepted);

  /* An accepted sample should be different from the previous one */
  for(i=1; i<N+1; i++)
  {
    if(accepted[i] != accepted[i-1])
    {
      for(j=0; j<dim; j++)
      {
          test_assert(fabs(samples[i*dim+j] - samples[(i-1)*dim+j]) > TEST_PRECISION_TOLERANCE);
      }
    }else{
      for(j=0; j<dim; j++)
      {
          test_assert(fabs(samples[i*dim+j] - samples[(i-1)*dim+j]) < TEST_PRECISION_TOLERANCE);
      }
    }
    ratio_ref = (precision)accepted[i] / (precision)i;
    test_assert(fabs(ratio[i] - ratio_ref) < TEST_PRECISION_TOLERANCE);
  }

  met_free(met);
  ch_free(burn);
  rt_free(rt);

  return 0;
}
