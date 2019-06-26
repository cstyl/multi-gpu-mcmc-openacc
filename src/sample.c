#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sample.h"
#include "logistic_regression.h"
#include "prior.h"
#include "memory.h"

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
  sample_t *cur = NULL;
  sample_t *pro = NULL;

  assert(cmd);
  assert(rng);
  assert(chain);
  assert(pcur);
  assert(ppro);

  pro = *ppro;
  cur = *pcur;

  printf("iter:%d | theta:", idx);
  int l;
  for(l=0; l<cmd->dim+1; l++)
    printf("\t%f", pro->values[l]);
  printf("\n");
  printf("        | current  \t prior=%f \t lhood=%f \t posterior=%f\n",
          cur->prior, cur->likelihood, cur->posterior);
  printf("        | proposed \t prior=%f \t lhood=%f \t posterior=%f\n",
          pro->prior, pro->likelihood, pro->posterior);

  /* to stochastically accept/reject the proposed sample*/
  u = rng_uniform_prob(rng, cmd->dim+1);

  if(log(u) <= chain->probability[idx])
  {
    /* accept the proposal, add the proposed sample to the chain */
    chain_append_sample(idx, pro->values, chain);
    chain->accepted[idx] = chain->accepted[idx-1] + 1;
    /* swap pointers of two samples instead of copying the contents of each other */
    sample_swap_ptrs((void**)&cur, (void**)&pro);
  }else{
    /* add the current sample to the chain */
    chain_append_sample(idx, cur->values, chain);
  }

  printf("%f < %f ? %s\n", log(u), chain->probability[idx], log(u)<=chain->probability[idx]? "accepted":"rejected");
  printf("accepted samples = %d\tacceptance ratio = %f\n", chain->accepted[idx], chain->ratio[idx]);

  *ppro = pro;
  *pcur = cur;
}

static void sample_swap_ptrs(void **ptr1, void **ptr2)
{
  void *ptr_temp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = ptr_temp;
}
