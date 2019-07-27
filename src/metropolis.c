#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "metropolis.h"
#include "prior.h"
#include "ran.h"
#include "memory.h"

struct met_s{
  pe_t *pe;
  rt_t *rt;
  ch_t *burn;           /* Chain during burn in period */
  ch_t *chain;          /* Chain during post-burn in in period */
  data_t *data;         /* Data structure to be used during sampling */
  mvnb_t *mvnb;         /* Multivariate Normal Proposal kernel with block update */
  lr_t *lr;             /* Logistic Regression Likelihood */
  sample_t *current;    /* Current sample */
  sample_t *proposed;   /* Proposed sample */
  int random_init;
};

static const char KERNEL_DEFAULT[BUFSIZ] = "mvn_block";
static const char LHOOD_DEFAULT[BUFSIZ] = "logistic_regression";
static const int RAND_INIT_DEFAULT = 0;
// static const int SEED_DEFAULT = 7361237;

int met_create(pe_t *pe, ch_t *burn, ch_t *chain, met_t **pmet){

  met_t *met = NULL;

  assert(pe);
  assert(burn);
  assert(chain);

  met = (met_t *) calloc(1, sizeof(met_t));
  assert(met);
  if(met == NULL) pe_fatal(pe, "calloc(met_t) failed\n");

  met->pe = pe;
  met->burn = burn;
  met->chain = chain;

  met_random_init_set(met, RAND_INIT_DEFAULT); /* Default initialize to zero */

  *pmet = met;

  return 0;
}

int met_free(met_t *met){

  assert(met);

  if(met->mvnb) mvn_block_free(met->mvnb);
  if(met->lr) lr_lhood_free(met->lr);
  if(met->current) sample_free(met->current);
  if(met->proposed) sample_free(met->proposed);
  if(met->data) data_free(met->data);
  mem_free((void**)&met);

  return 0;
}

/* Assign data, choose and assign proposal kernel and lhood, assign samples*/
int met_init_rt(pe_t *pe, rt_t *rt, met_t *met){

  char kernel_value[BUFSIZ];
  char lhood_value[BUFSIZ];
  int rinit = 0;

  assert(rt);
  assert(met);

  sprintf(kernel_value, "%s", KERNEL_DEFAULT);
  sprintf(lhood_value, "%s", LHOOD_DEFAULT);

  ran_init_rt(pe, rt); /* initialize state of the random number generator */

  sample_create(pe, &met->current);
  sample_create(pe, &met->proposed);

  sample_init_rt(rt, met->current);
  sample_init_rt(rt, met->proposed);

  data_create_train(pe, &met->data);
  data_init_train_rt(rt, met->data);
  data_input_train_info(pe, met->data);

  rt_string_parameter(rt, "kernel", kernel_value, BUFSIZ);
  if(strcmp(kernel_value, "mvn_block") == 0)
  {
    mvn_block_create(pe, &met->mvnb);
    mvn_block_init_rt(rt, met->mvnb);
  }

  rt_string_parameter(rt, "lhood", lhood_value, BUFSIZ);
  if(strcmp(lhood_value, "logistic_regression") == 0)
  {
    lr_lhood_create(pe, met->data, &met->lr);
  }

  if(rt_switch(rt, "random_init"))
  {
    rinit = 1;
    met_random_init_set(met, rinit);
  }

  return 0;
}

int met_info_rt(pe_t *pe, met_t *met){

  int dim, random_init, tune_rw_sd;
  double rwsd;
  assert(pe);
  assert(met);

  sample_dim(met->current, &dim);
  mvn_block_rwsd(met->mvnb, &rwsd);
  mvn_block_tune(met->mvnb, &tune_rw_sd);
  met_random_init(met, &random_init);

  pe_info(pe, "\n");
  pe_info(pe, "Metropolis Properties\n");
  pe_info(pe, "---------------------\n");
  pe_info(pe, "%30s\t\t%d\n", "Sample Dimensionality:", dim);
  pe_info(pe, "%30s\t\t%s\n", "Random Initialisation:", random_init>0 ? "True" : "False");
  if(met->mvnb)
  {
    pe_info(pe, "%30s\n", "Proposal Kernel:");
    pe_info(pe, "%30s\t\t%s\n", "Type ", "Multivariate Normal (Block)");
    pe_info(pe, "%30s\t\t%f\n", "Step Size ", rwsd);
    pe_info(pe, "%30s\t\t%s\n", "Tune ", tune_rw_sd>0 ? "True" : "False");
  }
  if(met->lr) pe_info(pe, "%30s\t\t%s\n", "Likelihood:", "Logistic Regression");


  return 0;
}

int met_init(pe_t *pe, met_t *met){

  precision *sample = NULL;
  precision lhood=0.0, prior=0.0, posterior=0.0;
  int dim;

  assert(pe);
  assert(met);

  /* Load data */
  data_read_file(pe, met->data);

  /* Initialise first sample */
  sample_init_zero(met->current);

  if(met->random_init)
  {
    if(met->mvnb)
    {
      mvn_block_init(met->mvnb);
      sample_propose_mvnb(met->mvnb, met->current, met->current);
    }
  }

  sample_values(met->current, &sample);
  sample_dim(met->current, &dim);

  ch_append_sample(0, sample, met->burn);
  ch_init_stats(0, met->burn);

  if(met->lr) lhood = lr_lhood(met->lr, sample);
  prior = pr_log_prob(sample, dim);
  posterior = prior + lhood;

  sample_prior_set(met->current, prior);
  sample_likelihood_set(met->current, lhood);
  sample_posterior_set(met->current, posterior);

  return 0;
}

int met_run(pe_t *pe, met_t *met){

  int bsteps, psteps, i;
  precision probability=0.0;
  precision *sample = NULL;

  assert(pe);
  assert(met);

  /* Loop over burn-in */
  ch_N(met->burn, &bsteps);
  pe_info(pe, "\nStarting burn-in period of %d steps..\n", bsteps);
  for(i=1; i<bsteps+1; i++)
  {
    if(met->mvnb) sample_propose_mvnb(met->mvnb, met->current, met->proposed);
    if(met->lr) probability = sample_evaluate_lr(met->lr, met->current, met->proposed);
    ch_append_probability(i, probability, met->burn);
    sample_choose(i, met->burn, &met->current, &met->proposed);
  }

  /* Reset stats for post burn-in chain */
  sample_values(met->current, &sample);
  ch_append_sample(0, sample, met->chain);
  ch_init_stats(0, met->chain);

  /* Loop over post burn-in */
  ch_N(met->chain, &psteps);
  pe_info(pe, "\nStarting post burn-in period of %d steps..\n", psteps);
  for(i=1; i<psteps+1; i++)
  {
    if(met->mvnb) sample_propose_mvnb(met->mvnb, met->current, met->proposed);
    if(met->lr) probability = sample_evaluate_lr(met->lr, met->current, met->proposed);

    ch_append_probability(i, probability, met->chain);
    sample_choose(i, met->chain, &met->current, &met->proposed);
  }

  return 0;
}

int met_random_init_set(met_t *met, int random_init){

  assert(met);

  met->random_init = random_init;

  return 0;
}

int met_random_init(met_t *met, int *random_init){

  assert(met);

  *random_init = met->random_init;

  return 0;
}

int met_burn(met_t *met, ch_t **pburn){

  assert(met);

  *pburn = met->burn;

  return 0;
}

int met_chain(met_t *met, ch_t **pchain){

  assert(met);

  *pchain = met->chain;

  return 0;
}

int met_data(met_t *met, data_t **pdata){

  assert(met);

  *pdata = met->data;

  return 0;
}

int met_mvnb(met_t *met, mvnb_t **pmvnb){

  assert(met);

  *pmvnb = met->mvnb;

  return 0;
}

int met_lr(met_t *met, lr_t **plr){

  assert(met);

  *plr = met->lr;

  return 0;
}

int met_current(met_t *met, sample_t **pcurrent){

  assert(met);

  *pcurrent = met->current;

  return 0;
}

int met_proposed(met_t *met, sample_t **pproposed){

  assert(met);

  *pproposed = met->proposed;

  return 0;
}
