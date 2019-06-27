#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sample.h"
#include "logistic_regression.h"
#include "prior.h"
#include "memory.h"

#define PRINT_PROGRESS 1
#define PRINT_FREQUENCY 5000
#define VERBOSE 1

struct sample_s{
  lh_t      *lhood;
  precision *values;
  precision prior;
  precision likelihood;
  precision posterior;
  int dim;
};

enum sample_error {SAMPLE_SUCCESS = 0,
                   SAMPLE_ERROR
};

static void sample_swap_ptrs(void **ptr1, void **ptr2);
static int sample_print_progress(cmd_t *cmd, int dec, int idx, precision u,
                                  sample_t *cur, sample_t *pro, chain_t *chain,
                                  int verbose);

/*****************************************************************************
 *
 *  sample_create
 *
 *****************************************************************************/

int sample_create(cmd_t *cmd, sample_t **psample){

  sample_t *sample = NULL;

  assert(cmd);

  sample = (sample_t *) calloc(1, sizeof(sample_t));
  assert(sample);
  if(sample == NULL)
  {
    printf("calloc(sample_t) failed\n");
    exit(SAMPLE_ERROR);
  }

  sample->dim = cmd->dim;
  mem_malloc_precision(&sample->values, sample->dim + 1);
  lr_lhood_create(cmd->train.N, &sample->lhood);

  *psample = sample;

  return SAMPLE_SUCCESS;
}

/*****************************************************************************
 *
 *  sample_free
 *
 *****************************************************************************/

int sample_free(sample_t *sample){

  assert(sample);

  if(sample->values) free(sample->values);
  lr_lhood_free(sample->lhood);

  free(sample);

  assert(sample->values != NULL);
  assert(sample != NULL);

  return SAMPLE_SUCCESS;
}

/*****************************************************************************
 *
 *  sample_init
 *
 *****************************************************************************/

int sample_init(cmd_t *cmd, rng_t *rng, data_t *data,
                chain_t *chain, sample_t *cur, int random){

  assert(cmd);
  assert(rng);
  assert(data);
  assert(chain);
  assert(cur);

  int i;

  /* initialise to zero */
  for(i=0; i<cmd->dim+1; i++)
    cur->values[i] = 0.0;

  if(random) sample_propose(cmd, rng, cur, cur);

  chain_append_sample(0, cur->values, chain);

  cur->likelihood = lr_lhood(cur->lhood, cur->values, data);
  cur->prior = pr_log_prob(cmd, cur->values);
  cur->posterior = cur->prior + cur->likelihood;

  return SAMPLE_SUCCESS;
}

/*****************************************************************************
 *
 *  sample_propose
 *
 *****************************************************************************/

int sample_propose(cmd_t *cmd, rng_t *rng, sample_t *cur, sample_t *pro){

  assert(cmd);
  assert(rng);
  assert(cur);
  assert(pro);

  int i;

  for(i=0; i<pro->dim+1; i++)
    pro->values[i] = cur->values[i] + rng_normal(rng, i, cmd->rwsd);


  return SAMPLE_SUCCESS;
}

/*****************************************************************************
 *
 *  sample_evaluate
 *
 *****************************************************************************/

precision sample_evaluate(cmd_t *cmd, rng_t *rng, data_t *data,
                          sample_t *cur, sample_t *pro){

  assert(cmd);
  assert(rng);
  assert(data);
  assert(cur);
  assert(pro);

  pro->likelihood = lr_lhood(pro->lhood, pro->values, data);
  pro->prior = pr_log_prob(cmd, pro->values);
  pro->posterior = pro->prior + pro->likelihood;

  return pro->posterior - cur->posterior;
}

/*****************************************************************************
 *
 *  sample_choose
 *
 *****************************************************************************/

void sample_choose(int idx, cmd_t *cmd, rng_t *rng,
                   chain_t *chain, sample_t **pcur, sample_t **ppro){

  precision u;
  int accepted = 0;
  sample_t *cur = NULL;
  sample_t *pro = NULL;

  assert(cmd);
  assert(rng);
  assert(chain);
  assert(pcur);
  assert(ppro);

  pro = *ppro;
  cur = *pcur;

  /* to stochastically accept/reject the proposed sample*/
  u = rng_uniform_prob(rng, cmd->dim+1);

  if(log(u) <= chain->probability[idx])
  {
    /* accept the proposal, add the proposed sample to the chain */
    chain_append_sample(idx, pro->values, chain);
    accepted = 1;
    /* swap pointers of two samples instead of copying the contents of each other */
    sample_swap_ptrs((void**)&cur, (void**)&pro);
  }else{
    /* add the current sample to the chain */
    chain_append_sample(idx, cur->values, chain);
  }

  chain_append_stats(idx, accepted, chain);

#if PRINT_PROGRESS
  if((idx==1) || idx%PRINT_FREQUENCY==0)
    sample_print_progress(cmd, accepted, idx, log(u), cur, pro, chain, VERBOSE);
#endif

  *ppro = pro;
  *pcur = cur;
}

/*****************************************************************************
 *
 *  sample_swap_ptrs
 *
 *****************************************************************************/

static void sample_swap_ptrs(void **ptr1, void **ptr2)
{
  void *ptr_temp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = ptr_temp;
}

/*****************************************************************************
 *
 *  sample_print_progress
 *
 *****************************************************************************/

static int sample_print_progress(cmd_t *cmd, int dec, int idx, precision u,
                                  sample_t *cur, sample_t *pro, chain_t *chain,
                                  int verbose){

  assert(cmd);
  assert(cur);
  assert(pro);
  assert(chain);

  printf("Iteration %6d:\t", idx);
  printf("%20s%6d\t", "Accepted Samples = ", chain->accepted[idx]);
  printf("%20s%4.3f%s\n", "Acceptance Ratio = ",chain->ratio[idx]*100, "(%)");

  if(verbose)
  {
    int i;
    printf("\tCurrent Sample:\n\t\tValues:");
    for(i=0; i<cmd->dim+1; i++) printf("\t%f", (dec != 0) ? pro->values[i] : cur->values[i]);
    printf("\n\t\tStats:\tPrior = %f\tLikelihood = %f\tPosterior=%f\n",
            dec==1 ? pro->prior:cur->prior,
            dec==1 ? pro->likelihood:cur->likelihood,
            dec==1 ? pro->posterior:cur->posterior
          );

    printf("\tProposed Sample:\n\t\tValues:");
    for(i=0; i<cmd->dim+1; i++) printf("\t%f", dec==1 ? cur->values[i]:pro->values[i]);
    printf("\n\t\tStats:\tPrior = %f\tLikelihood = %f\tPosterior=%f\n",
            dec==1 ? cur->prior:pro->prior,
            dec==1 ? cur->likelihood:pro->likelihood,
            dec==1 ? cur->posterior:pro->posterior
          );

    printf("\tDecision: Proposed sample %s! (%f<%f)\n\n",
            dec==1?"accepted":"rejected", u, chain->probability[idx]);
  }

  return SAMPLE_SUCCESS;
}
