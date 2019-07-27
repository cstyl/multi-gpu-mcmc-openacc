#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "definitions.h"
#include "pe.h"
#include "runtime.h"
#include "chain.h"
#include "tests.h"

static int test_chain_burn_rt_default(pe_t *pe);
static int test_chain_chain_rt_default(pe_t *pe);
static int test_chain_burn_rt(pe_t *pe);
static int test_chain_chain_rt(pe_t *pe);
static int test_chain_init_stats(pe_t *pe);
static int test_chain_burn_append(pe_t *pe);
static int test_chain_chain_append(pe_t *pe);

int test_chain_suite(void){

  pe_t *pe = NULL;

  pe_create(MPI_COMM_WORLD, PE_QUIET, &pe);
  assert(pe);
  test_assert(1);

  test_chain_burn_rt_default(pe);
  test_chain_chain_rt_default(pe);
  test_chain_burn_rt(pe);
  test_chain_chain_rt(pe);
  test_chain_init_stats(pe);
  test_chain_burn_append(pe);
  test_chain_chain_append(pe);

  pe_info(pe, "PASS\t./unit/test_chain\n");
  pe_free(pe);

  return 0;
}

static int test_chain_burn_rt_default(pe_t *pe){

  assert(pe);

  int dim, N, outfreq;
  char outdir[FILENAME_MAX];
  precision *samples = NULL;
  precision *probability = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;

  rt_t *rt = NULL;
  ch_t *burn = NULL;

  rt_create(pe, &rt);
  assert(rt);

  ch_create(pe, &burn);
  assert(burn);
  test_assert(1);

  ch_dim(burn, &dim);
  test_assert(dim == 3);
  ch_N(burn, &N);
  test_assert(N == 500);
  ch_outfreq(burn, &outfreq);
  test_assert(outfreq == 50);
  ch_outdir(burn, outdir);
  test_assert(strcmp(outdir, "../out") == 0);
  ch_samples(burn, &samples);
  test_assert(samples == NULL);
  ch_probability(burn, &probability);
  test_assert(probability == NULL);
  ch_ratio(burn, &ratio);
  test_assert(ratio == NULL);
  ch_accepted(burn, &accepted);
  test_assert(accepted == NULL);

  ch_free(burn);
  rt_free(rt);

  return 0;
}

static int test_chain_chain_rt_default(pe_t *pe){

  assert(pe);

  int dim, N, outfreq;
  char outdir[FILENAME_MAX];
  precision *samples = NULL;
  precision *probability = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;

  rt_t *rt = NULL;
  ch_t *chain = NULL;

  rt_create(pe, &rt);
  assert(rt);

  ch_create(pe, &chain);
  assert(chain);
  test_assert(1);

  ch_dim(chain, &dim);
  test_assert(dim == 3);
  ch_N(chain, &N);
  test_assert(N == 500);
  ch_outfreq(chain, &outfreq);
  test_assert(outfreq == 50);
  ch_outdir(chain, outdir);
  test_assert(strcmp(outdir, "../out") == 0);
  ch_samples(chain, &samples);
  test_assert(samples == NULL);
  ch_probability(chain, &probability);
  test_assert(probability == NULL);
  ch_ratio(chain, &ratio);
  test_assert(ratio == NULL);
  ch_accepted(chain, &accepted);
  test_assert(accepted == NULL);

  ch_free(chain);
  rt_free(rt);

  return 0;
}

static int test_chain_burn_rt(pe_t *pe){

  assert(pe);

  int dim, N, outfreq;
  char outdir[FILENAME_MAX];
  precision *samples = NULL;
  precision *probability = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;

  rt_t *rt = NULL;
  ch_t *burn = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  ch_create(pe, &burn);
  assert(burn);
  test_assert(1);

  /* testing burn-in chain runtime init */
  ch_init_burn_rt(rt, burn);

  ch_dim(burn, &dim);
  test_assert(dim == 3);
  ch_N(burn, &N);
  test_assert(N == 5);
  ch_outfreq(burn, &outfreq);
  test_assert(outfreq == 1000);
  ch_outdir(burn, outdir);
  test_assert(strcmp(outdir, "./test-out") == 0);
  ch_samples(burn, &samples);
  test_assert(samples != NULL);
  ch_probability(burn, &probability);
  test_assert(probability != NULL);
  ch_ratio(burn, &ratio);
  test_assert(ratio != NULL);
  ch_accepted(burn, &accepted);
  test_assert(accepted != NULL);

  ch_free(burn);
  rt_free(rt);

  return 0;
}

static int test_chain_chain_rt(pe_t *pe){

  assert(pe);

  int dim, N, outfreq;
  char outdir[FILENAME_MAX];
  precision *samples = NULL;
  precision *probability = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;

  rt_t *rt = NULL;
  ch_t *chain = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  ch_create(pe, &chain);
  assert(chain);
  test_assert(1);

  /* testing post burn-in chain runtime init */
  ch_init_chain_rt(rt, chain);

  ch_dim(chain, &dim);
  test_assert(dim == 3);
  ch_N(chain, &N);
  test_assert(N == 25);
  ch_outfreq(chain, &outfreq);
  test_assert(outfreq == 1000);
  ch_outdir(chain, outdir);
  test_assert(strcmp(outdir, "./test-out") == 0);
  ch_samples(chain, &samples);
  test_assert(samples != NULL);
  ch_probability(chain, &probability);
  test_assert(probability != NULL);
  ch_ratio(chain, &ratio);
  test_assert(ratio != NULL);
  ch_accepted(chain, &accepted);
  test_assert(accepted != NULL);

  ch_free(chain);
  rt_free(rt);

  return 0;
}

static int test_chain_init_stats(pe_t *pe){

  assert(pe);

  int dim = 3, N = 0;
  precision *probability = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;

  rt_t *rt = NULL;
  ch_t *burn = NULL;
  ch_t *chain = NULL;

  rt_create(pe, &rt);
  assert(rt);

  ch_create(pe, &burn);
  assert(burn);
  test_assert(1);

  ch_create(pe, &chain);
  assert(chain);
  test_assert(1);

  /* testing burn-in init when burn-in is disabled */
  ch_dim_set(burn, dim);
  ch_N_set(burn, N);

  ch_init_burn_rt(rt, burn);
  ch_init_stats(0, burn);
  ch_probability(burn, &probability);
  test_assert(fabs(probability[0] - 0.0) < TEST_PRECISION_TOLERANCE);
  ch_ratio(burn, &ratio);
  test_assert(fabs(ratio[0] - 0.0) < TEST_PRECISION_TOLERANCE);
  ch_accepted(burn, &accepted);
  test_assert(accepted[0] == 0);

  /* testing post burn-in init when post burn-in is disabled */
  ch_dim_set(chain, dim);
  ch_N_set(chain, N);

  ch_init_chain_rt(rt, chain);
  ch_init_stats(0, chain);
  ch_probability(chain, &probability);
  test_assert(fabs(probability[0] - 0.0) < TEST_PRECISION_TOLERANCE);
  ch_ratio(chain, &ratio);
  test_assert(fabs(ratio[0] - 0.0) < TEST_PRECISION_TOLERANCE);
  ch_accepted(chain, &accepted);
  test_assert(accepted[0] == 0);

  ch_free(burn);
  ch_free(chain);
  rt_free(rt);

  return 0;
}

static int test_chain_burn_append(pe_t *pe){

  assert(pe);

  int dim = 3, N = 2, i, j;
  precision *samples = NULL;
  precision *probability = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;

  rt_t *rt = NULL;
  ch_t *burn = NULL;

  precision samples_ref[2][3] = {{1.0, 2.0, 3.0},
                                 {4.0, 5.0, 6.0}
                                };

  precision probability_ref[2] = {0.1, 0.2};
  precision ratio_ref[2] = {1.0/1.0, 1.0/2.0};
  int accepted_ref[2] = {1, 0};
  int total_accepted = 0;

  rt_create(pe, &rt);
  assert(rt);

  ch_create(pe, &burn);
  assert(burn);
  test_assert(1);

  ch_dim_set(burn, dim);
  ch_N_set(burn, N);

  ch_init_burn_rt(rt, burn);
  ch_init_stats(0, burn);

  ch_samples(burn, &samples);
  ch_probability(burn, &probability);
  ch_ratio(burn, &ratio);
  ch_accepted(burn, &accepted);

  for(i=0; i<N; i++)
  {
    ch_append_sample(i+1, &samples_ref[i][0], burn);
    for(j=0; j<dim; j++)
    {
      test_assert(fabs(samples[(i+1)*dim+j] - samples_ref[i][j]) < TEST_PRECISION_TOLERANCE);
    }

    ch_append_probability(i+1, probability_ref[i], burn);
    test_assert(fabs(probability[i+1] - probability_ref[i]) < TEST_PRECISION_TOLERANCE);

    ch_append_stats(i+1, accepted_ref[i], burn);
    total_accepted += accepted_ref[i];
    test_assert(fabs(ratio[i+1] - ratio_ref[i]) < TEST_PRECISION_TOLERANCE);
    test_assert(accepted[i+1] == total_accepted);
  }

  ch_free(burn);
  rt_free(rt);

  return 0;
}

static int test_chain_chain_append(pe_t *pe){

  assert(pe);

  int dim = 3, N = 5, i, j;
  precision *samples = NULL;
  precision *probability = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;

  rt_t *rt = NULL;
  ch_t *chain = NULL;

  precision samples_ref[5][3] = {{1.0, 1.1, 1.2},
                                 {2.0, 2.1, 2.2},
                                 {3.0, 3.1, 3.2},
                                 {4.0, 4.1, 4.2},
                                 {5.0, 5.1, 5.2}
                                };

  precision probability_ref[5] = {0.1, 0.2, 0.3, 0.4, 0.5};
  precision ratio_ref[5] = {1.0/1.0, 1.0/2.0, 2.0/3.0, 3.0/4.0, 3.0/5.0};
  int accepted_ref[5] = {1, 0, 1, 1, 0};
  int total_accepted = 0;

  rt_create(pe, &rt);
  assert(rt);

  ch_create(pe, &chain);
  assert(chain);
  test_assert(1);

  ch_dim_set(chain, dim);
  ch_N_set(chain, N);

  ch_init_chain_rt(rt, chain);
  ch_init_stats(0, chain);

  ch_samples(chain, &samples);
  ch_probability(chain, &probability);
  ch_ratio(chain, &ratio);
  ch_accepted(chain, &accepted);

  for(i=0; i<N; i++)
  {
    ch_append_sample(i+1, &samples_ref[i][0], chain);
    for(j=0; j<dim; j++)
    {
      test_assert(fabs(samples[(i+1)*dim+j] - samples_ref[i][j]) < TEST_PRECISION_TOLERANCE);
    }

    ch_append_probability(i+1, probability_ref[i], chain);
    test_assert(fabs(probability[i+1] - probability_ref[i]) < TEST_PRECISION_TOLERANCE);

    ch_append_stats(i+1, accepted_ref[i], chain);
    total_accepted += accepted_ref[i];
    test_assert(fabs(ratio[i+1] - ratio_ref[i]) < TEST_PRECISION_TOLERANCE);
    test_assert(accepted[i+1] == total_accepted);
  }

  ch_free(chain);
  rt_free(rt);

  return 0;
}
