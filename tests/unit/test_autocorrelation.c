#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "pe.h"
#include "autocorrelation.h"
#include "tests.h"

static int test_autocorrelation_rt_default(pe_t *pe);
static int test_autocorrelation_rt(pe_t *pe);
static int test_autocorrelation_compute_lagk(pe_t *pe);

int test_autocorrelation_suite(void){

  pe_t *pe = NULL;

  pe_create(MPI_COMM_WORLD, PE_QUIET, &pe);
  assert(pe);
  test_assert(1);

  test_autocorrelation_rt_default(pe);
  test_autocorrelation_rt(pe);
  test_autocorrelation_compute_lagk(pe);

  pe_info(pe, "PASS\t./unit/test_autocorrelation\n");
  pe_free(pe);

  return 0;
}

static int test_autocorrelation_rt_default(pe_t *pe){

  int dim, N, maxlag, outfreq;
  precision threshold;
  char outdir[FILENAME_MAX];

  assert(pe);

  acr_t *acr = NULL;
  rt_t *rt = NULL;
  ch_t *chain = NULL;

  rt_create(pe, &rt);
  assert(rt);

  ch_create(pe, &chain);
  assert(chain);

  acr_create(pe, chain, &acr);

  acr_init_rt(rt, chain, acr);

  acr_dim(acr, &dim);
  test_assert(dim == 3);
  acr_N(acr, &N);
  test_assert(N == 500);
  acr_maxlag(acr, &maxlag);
  test_assert(maxlag == 249);
  acr_threshold(acr, &threshold);
  test_assert(fabs(threshold - 0.1) < TEST_PRECISION_TOLERANCE);
  acr_outfreq(acr, &outfreq);
  test_assert(outfreq == 50);
  acr_outdir(acr, outdir);
  test_assert(strcmp(outdir, "../out") == 0);

  acr_free(acr);

  return 0;
}

static int test_autocorrelation_rt(pe_t *pe){

  int i, dim, N, maxlag, outfreq;
  precision threshold;
  char outdir[FILENAME_MAX];
  precision *X = NULL;
  precision *mean = NULL;
  precision *variance = NULL;
  int *offset = NULL;
  int *maxlag_act = NULL;
  precision *lagk = NULL;

  assert(pe);

  acr_t *acr = NULL;
  rt_t *rt = NULL;
  ch_t *chain = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  ch_create(pe, &chain);
  assert(chain);
  ch_init_chain_rt(rt, chain);

  acr_create(pe, chain, &acr);
  assert(acr);
  test_assert(1);

  acr_init_rt(rt, chain, acr);

  acr_dim(acr, &dim);
  test_assert(dim == 3);
  acr_N(acr, &N);
  test_assert(N == 25);
  acr_maxlag(acr, &maxlag);
  test_assert(maxlag == 10);
  acr_threshold(acr, &threshold);
  test_assert(fabs(threshold - 0.2) < TEST_PRECISION_TOLERANCE);
  acr_outfreq(acr, &outfreq);
  test_assert(outfreq == 1000);
  acr_outdir(acr, outdir);
  test_assert(strcmp(outdir, "./test-out") == 0);

  acr_X(acr, &X);
  test_assert(X != NULL);
  acr_mean(acr, &mean);
  test_assert(mean != NULL);
  acr_variance(acr, &variance);
  test_assert(variance != NULL);
  acr_offset(acr, &offset);
  test_assert(offset != NULL);
  acr_maxlag_act(acr, &maxlag_act);
  test_assert(maxlag_act != NULL);
  acr_lagk(acr, &lagk);
  test_assert(lagk != NULL);

  for(i=0; i<dim; i++) test_assert(offset[i] == N*i);

  acr_free(acr);

  return 0;
}

static int test_autocorrelation_compute_lagk(pe_t *pe){

  assert(pe);
  int i, N=10, lag=2;
  precision acr_lag_ref=0.0, acr_lag_act;
  precision X[10] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  precision mean=0.0, var=0.0;
  precision threshold=0.1;

  for(i=0; i<N; i++) mean += X[i] / N;
  for(i=0; i<N; i++) var += pow(X[i]-mean, 2.0) / N;

  for(i=0; i< N-lag; i++)
  {
    acr_lag_ref += (X[i]-mean) * (X[i+lag]-mean);
  }
  acr_lag_ref *= 1.0 / ((N - lag) * var);

  acr_lag_act = acr_compute_lagk(X, mean, var, N, lag, threshold);

  test_assert(fabs(acr_lag_act - acr_lag_ref) < TEST_PRECISION_TOLERANCE);

  /* Check if returns 1.0 when lag is set to zero*/
  acr_lag_act = acr_compute_lagk(X, mean, var, N, 0, threshold);

  test_assert(fabs(acr_lag_act - 1.0) < TEST_PRECISION_TOLERANCE);

  /* Check that returns threshold value when the value is less than threshold */
  N=3;
  precision Z[3] = {-1.0, 1.0, -1.0};
  mean = -1.0/3.0;
  for(i=0; i<N; i++) var += pow(Z[i]-mean, 2.0) / N;
  acr_lag_act = acr_compute_lagk(Z, mean, var, N, 1, threshold);
  test_assert(fabs(acr_lag_act - threshold) < TEST_PRECISION_TOLERANCE);

  return 0;
}
