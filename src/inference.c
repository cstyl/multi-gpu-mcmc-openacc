#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "inference.h"
#include "logistic_regression.h"
#include "data_input.h"
#include "memory.h"

struct infr_s{
  pe_t *pe;
  ch_t *chain;
  data_t *data;
  precision *sum;
  int *labels;
  precision accuracy;
  char mc_case[BUFSIZ];
};

static const char MC_CASE_DEFAULT[BUFSIZ] = "logistic_regression";

static int infr_allocate_sum(infr_t *infr);
static int infr_allocate_labels(infr_t *infr);

/*****************************************************************************
 *
 * infr_create
 *
 *****************************************************************************/

int infr_create(pe_t *pe, ch_t *chain, infr_t **pinfr){

  infr_t *infr = NULL;

  assert(pe);
  assert(chain);

  infr = (infr_t *) calloc(1, sizeof(infr_t));
  assert(infr);
  if(infr == NULL) pe_fatal(pe, "calloc(infr_t) failed\n");

  infr->pe = pe;
  infr->chain = chain;

  data_create_test(pe, &infr->data);
  infr_mc_case_set(infr, MC_CASE_DEFAULT);

  *pinfr = infr;

  return 0;
}

/*****************************************************************************
 *
 * infr_free
 *
 *****************************************************************************/

int infr_free(infr_t *infr){

  assert(infr);

  mem_free((void **)&infr->sum);
  mem_free((void **)&infr->labels);

  if(infr->data) data_free(infr->data);

  mem_free((void **)&infr);

  return 0;
}

/*****************************************************************************
 *
 * infr_init_rt
 *
 *****************************************************************************/

int infr_init_rt(pe_t *pe, rt_t *rt, infr_t *infr){

  char mc_case[BUFSIZ];

  assert(pe);
  assert(rt);
  assert(infr);

  data_init_test_rt(rt, infr->data);
  data_input_test_info(pe, infr->data);

  rt_string_parameter(rt, "mc_integ", mc_case, BUFSIZ);
  if(strcmp(mc_case, "logistic_regression") == 0)
  {
   infr_mc_case_set(infr, mc_case);
  }

  infr_allocate_sum(infr);
  infr_allocate_labels(infr);

  return 0;
}

/*****************************************************************************
 *
 * infr_init_rt
 *
 *****************************************************************************/

int infr_init(pe_t *pe, infr_t *infr){

  assert(pe);
  assert(infr);

  /* Load data */
  data_read_file(pe, infr->data);

  return 0;
}

/*****************************************************************************
 *
 * infr_mc_integration_lr
 *
 *****************************************************************************/

int infr_mc_integration_lr(infr_t *infr){

  int dim, N_data, N_samples;
  precision *samples = NULL;
  precision *x = NULL;
  int *y = NULL;
  assert(infr);

  data_dimx(infr->data, &dim);
  data_N(infr->data, &N_data);
  data_x(infr->data, &x);
  data_y(infr->data, &y);
  ch_N(infr->chain, &N_samples);
  ch_samples(infr->chain, &samples);

  int i,j;
  precision acc_sum=0.0;
  /* Perform a logistic regression on each data point
   * going through all the generated samples.
   * Accumulate the sum (MC integration) for each data point to evaluate its class
  */
  for(i=0; i<N_data; i++)
  {
    infr->sum[i] = 0.0;
    for(j=0; j<N_samples; j++)
    {
      infr->sum[i] += lr_logistic_regression(&samples[j*dim], &x[i*dim], dim) / N_samples;
    }

    infr->labels[i] = (infr->sum[i]>=0.5) ? 1 : -1;
    acc_sum += (infr->labels[i] == y[i]) ? 1 : 0;
  }

  infr->accuracy = (precision)acc_sum / (precision)N_data;
  printf("accuracy: %.16f\n", infr->accuracy);
  return 0;
}

/*****************************************************************************
 *
 *  infr_mc_case_set
 *
 *****************************************************************************/

int infr_mc_case_set(infr_t *infr, const char *mc_case){

  assert(infr);

  sprintf(infr->mc_case, "%s", mc_case);

  return 0;
}

/*****************************************************************************
 *
 *  infr_mc_case
 *
 *****************************************************************************/

int infr_mc_case(infr_t *infr, char *mc_case){

  assert(infr);

  sprintf(mc_case, "%s", infr->mc_case);

  return 0;
}

/*****************************************************************************
 *
 *  infr_allocate_sum
 *
 *****************************************************************************/

static int infr_allocate_sum(infr_t *infr){

  assert(infr);

  int N;
  data_N(infr->data, &N);

  mem_malloc_precision(&infr->sum, N);

  return 0;
}

/*****************************************************************************
 *
 *  acr_allocate_mean
 *
 *****************************************************************************/

static int infr_allocate_labels(infr_t *infr){

  assert(infr);

  int N;
  data_N(infr->data, &N);

  mem_malloc_integers(&infr->labels, N);

  return 0;
}
