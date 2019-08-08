#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sample.h"
#include "prior.h"
#include "memory.h"
#include "ran.h"
#include "timer.h"
#define VERBOSE 1

struct sample_s{
  precision *values;
  precision prior;
  precision likelihood;
  precision posterior;
  int dim;
};

static int sample_allocate_values(sample_t *sample);
static int sample_print_progress(int dec, int idx, precision u, int verbose,
                                  sample_t *cur, sample_t *pro, ch_t *chain);

/*****************************************************************************
 *
 *  sample_create
 *
 *****************************************************************************/

int sample_create(pe_t *pe, sample_t **psample){

  sample_t *sample = NULL;

  assert(pe);

  sample = (sample_t *) calloc(1, sizeof(sample_t));
  assert(sample);
  if(sample == NULL) pe_fatal(pe, "calloc(sample_t) failed\n");

  sample_dim_set(sample, DIMX_DEFAULT);

  *psample = sample;

  return 0;
}

/*****************************************************************************
 *
 *  sample_free
 *
 *****************************************************************************/

int sample_free(sample_t *sample){

  assert(sample);

  mem_free((void**)&sample->values);
  mem_free((void**)&sample);

  return 0;
}

/*****************************************************************************
 *
 *  sample_init_rt
 *
 *****************************************************************************/

int sample_init_rt(rt_t *rt, sample_t *sample){

  int dim;

  assert(rt);
  assert(sample);

  if(rt_int_parameter(rt, "sample_dim", &dim))
  {
    sample_dim_set(sample, dim);
  }

  sample_allocate_values(sample);

  return 0;
}


/*****************************************************************************
 *
 *  sample_propose_mvnb
 *
 *****************************************************************************/

int sample_propose_mvnb(mvnb_t *mvnb, sample_t *cur, sample_t *pro){

  precision *current = NULL;
  precision *proposed = NULL;

  assert(mvnb);
  assert(cur);
  assert(pro);

  TIMER_start(TIMER_PROPOSAL);

  sample_values(cur, &current);
  sample_values(pro , &proposed);

  mvn_block_sample(mvnb, current, proposed);

  TIMER_stop(TIMER_PROPOSAL);
  return 0;
}

/*****************************************************************************
 *
 *  sample_evaluate_lr
 *
 *****************************************************************************/

precision sample_evaluate_lr(lr_t *lr, sample_t *cur, sample_t *pro){

  precision lhood=0.0, prior=0.0, posterior=0.0;
  precision cposterior;
  int dim;
  double ratio;
  precision *values = NULL;

  assert(lr);
  assert(cur);
  assert(pro);

  TIMER_start(TIMER_EVALUATION);

  sample_posterior(cur, &cposterior);

  sample_values(pro, &values);
  sample_dim(pro, &dim);

  lhood = lr_lhood(lr, values);
  prior = pr_log_prob(values, dim);
  posterior = prior + lhood;

  sample_prior_set(pro, prior);
  sample_likelihood_set(pro, lhood);
  sample_posterior_set(pro, posterior);

  ratio = exp(posterior - cposterior);

  TIMER_stop(TIMER_EVALUATION);

  return (ratio>1) ? 1: ratio;
}

/*****************************************************************************
 *
 *  sample_choose
 *
 *****************************************************************************/

void sample_choose(int idx, ch_t *chain, sample_t **pcur, sample_t **ppro){

  precision u;
  int accepted = 0;
  sample_t *cur = NULL;
  sample_t *pro = NULL;
  precision *probability = NULL;
  int outfreq;

  assert(chain);
  assert(pcur);
  assert(ppro);

  TIMER_start(TIMER_ACCEPTANCE);

  pro = *ppro;
  cur = *pcur;
  ch_probability(chain, &probability);
  ch_outfreq(chain, &outfreq);

  /* to stochastically accept/reject the proposed sample*/
  u = (precision)ran_serial_uniform();

  if(u <= probability[idx])
  {
    /* accept the proposal, add the proposed sample to the chain */
    ch_append_sample(idx, pro->values, chain);
    accepted = 1;
    /* swap pointers of two samples instead of copying the contents of each other */
    mem_swap_ptrs((void**)&cur, (void**)&pro);
  }else{
    /* add the current sample to the chain */
    ch_append_sample(idx, cur->values, chain);
  }

  ch_append_stats(idx, accepted, chain);

  if((idx==1) || idx%outfreq==0)
    sample_print_progress(accepted, idx, u, VERBOSE, cur, pro, chain);

  TIMER_stop(TIMER_ACCEPTANCE);

  *ppro = pro;
  *pcur = cur;
}

/*****************************************************************************
 *
 *  sample_init_zero
 *
 *****************************************************************************/

int sample_init_zero(sample_t *sample){

  int i;
  assert(sample);

  for(i=0; i<sample->dim; i++)
    sample->values[i] = 0.0;

  sample->prior = 0.0;
  sample->likelihood = 0.0;
  sample->posterior = 0.0;

  return 0;
}


/*****************************************************************************
 *
 *  sample_dim_set
 *
 *****************************************************************************/

int sample_dim_set(sample_t *sample, int dim){

  assert(sample);

  sample->dim = dim;

  return 0;
}

/*****************************************************************************
 *
 *  sample_prior_set
 *
 *****************************************************************************/

int sample_prior_set(sample_t *sample, precision prior){

  assert(sample);

  sample->prior = prior;

  return 0;
}

/*****************************************************************************
 *
 *  sample_likelihood_set
 *
 *****************************************************************************/

int sample_likelihood_set(sample_t *sample, precision likelihood){

  assert(sample);

  sample->likelihood = likelihood;

  return 0;
}

/*****************************************************************************
 *
 *  sample_posterior_set
 *
 *****************************************************************************/

int sample_posterior_set(sample_t *sample, precision posterior){

  assert(sample);

  sample->posterior = posterior;

  return 0;
}

/*****************************************************************************
 *
 *  sample_copy_values
 *
 *****************************************************************************/

int sample_copy_values(sample_t *sample, precision *values){

  assert(sample);
  assert(values);

  int i;

  for(i=0; i<sample->dim; i++) sample->values[i] = values[i];

  return 0;
}

/*****************************************************************************
 *
 *  sample_values
 *
 *****************************************************************************/

int sample_values(sample_t *sample, precision **pvalues){

  assert(sample);

  *pvalues = sample->values;

  return 0;
}

/*****************************************************************************
 *
 *  sample_dim
 *
 *****************************************************************************/

int sample_dim(sample_t *sample, int *dim){

  assert(sample);

  *dim = sample->dim;

  return 0;
}

/*****************************************************************************
 *
 *  sample_prior
 *
 *****************************************************************************/

int sample_prior(sample_t *sample, precision *prior){

  assert(sample);

  *prior = sample->prior;

  return 0;
}

/*****************************************************************************
 *
 *  sample_likelihood
 *
 *****************************************************************************/

int sample_likelihood(sample_t *sample, precision *likelihood){

  assert(sample);

  *likelihood = sample->likelihood;

  return 0;
}

/*****************************************************************************
 *
 *  sample_posterior
 *
 *****************************************************************************/

int sample_posterior(sample_t *sample, precision *posterior){

  assert(sample);

  *posterior = sample->posterior;

  return 0;
}

/*****************************************************************************
 *
 *  sample_allocate_values
 *
 *****************************************************************************/

static int sample_allocate_values(sample_t *sample){

  assert(sample);

  mem_malloc_precision(&sample->values, sample->dim);

  return 0;
}

/*****************************************************************************
 *
 *  sample_print_progress
 *
 *****************************************************************************/

static int sample_print_progress(int dec, int idx, precision u, int verbose,
                                  sample_t *cur, sample_t *pro, ch_t *chain){

  precision *probability = NULL;
  precision *ratio = NULL;
  int *accepted = NULL;
  int dim;

  assert(cur);
  assert(pro);
  assert(chain);

  ch_probability(chain, &probability);
  ch_ratio(chain, &ratio);
  ch_accepted(chain, &accepted);
  ch_dim(chain, &dim);

  printf("Iteration %6d:\t", idx);
  printf("%20s%6d\t", "Accepted Samples = ", accepted[idx]);
  printf("%20s%4.3f%s\n", "Acceptance Ratio = ",ratio[idx]*100, "(%)");

  if(verbose)
  {
    int i;
    printf("\tCurrent Sample:\n\t\tValues:");
    for(i=0; i<dim; i++) printf("\t%.16f", (dec != 0) ? pro->values[i] : cur->values[i]);
    printf("\n\t\tStats:\tPrior = %.16f\tLikelihood = %.16f\tPosterior=%.16f\n",
            dec==1 ? pro->prior:cur->prior,
            dec==1 ? pro->likelihood:cur->likelihood,
            dec==1 ? pro->posterior:cur->posterior
          );

    printf("\tProposed Sample:\n\t\tValues:");
    for(i=0; i<dim; i++) printf("\t%.16f", dec==1 ? cur->values[i]:pro->values[i]);
    printf("\n\t\tStats:\tPrior = %.16f\tLikelihood = %.16f\tPosterior=%.16f\n",
            dec==1 ? cur->prior:pro->prior,
            dec==1 ? cur->likelihood:pro->likelihood,
            dec==1 ? cur->posterior:pro->posterior
          );

    printf("\tDecision: Proposed sample %s! (%.16f<%.16f)\n\n",
            dec==1?"accepted":"rejected", u, probability[idx]);
  }

  return 0;
}
