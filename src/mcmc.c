#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command_line_parser.h"
#include "data_input.h"
#include "random_number_generator.h"

#include "metropolis.h"
#include "chain.h"
#include "inference.h"
#include "effective_sample_size.h"
#include "mcmc.h"
#include "memory.h"
#include "timer.h"

#define RANDOM 0

struct mcmc_s{
  cmd_t  *cmd;             /* Command line arguments */
  data_t *train;           /* Input data for training */
  data_t *test;            /* Data for inferencing */
  rng_t  *rng;             /* Random Number Generator */
  met_t  *met;             /* Metropolis Sampling */
  ess_t  *ess;             /* Effective Sample Size statistics */
  infr_t *infr;            /* Inference Statistics */
};

/*****************************************************************************
 *
 * mcmc_create
 *
 *****************************************************************************/

int mcmc_create(mcmc_t **pmcmc){

    mcmc_t *mcmc = NULL;

    mcmc = (mcmc_t *) calloc(1, sizeof(mcmc_t));
    assert(mcmc);
    if(mcmc == NULL)
    {
      printf("calloc(mcmc) failed\n");
      exit(1);
    }

    *pmcmc = mcmc;

    return 0;
}

/*****************************************************************************
 *
 *  mcmc_free
 *
 *****************************************************************************/

int mcmc_free(mcmc_t *mcmc){

  assert(mcmc);
  free(mcmc);

  assert(mcmc != NULL);

  return 0;
}

/*****************************************************************************
 *
 *  mcmc_setup
 *
 *****************************************************************************/

int mcmc_setup(int an, char *av[], mcmc_t *mcmc){

  assert(mcmc);

  TIMER_start(TIMER_MCMC_SETUP);

  cmd_create(&mcmc->cmd);
  cmd_parse(an, av, mcmc->cmd);
  cmd_print_status(mcmc->cmd);

  data_create(&mcmc->cmd->train, &mcmc->train);
  data_read_file(mcmc->train);

  rng_create(mcmc->cmd, &mcmc->rng);
  rng_setup(mcmc->rng);

  metropolis_create(mcmc->cmd, mcmc->rng, mcmc->train, &mcmc->met);

  ess_create(mcmc->cmd, mcmc->met, &mcmc->ess);

  TIMER_stop(TIMER_MCMC_SETUP);

  return 0;
}

/*****************************************************************************
 *
 *  mcmc_disassemble
 *
 *****************************************************************************/

int mcmc_disassemble(mcmc_t *mcmc){

  assert(mcmc);

  TIMER_start(TIMER_MCMC_DISSASEMBLE);

  if(mcmc->train) data_free(mcmc->train);
  if(mcmc->test)  data_free(mcmc->test);
  if(mcmc->met)   metropolis_free(mcmc->met);
  if(mcmc->ess)   ess_free(mcmc->ess);
  if(mcmc->infr)  infr_free(mcmc->infr);

  rng_free(mcmc->rng);
  cmd_free(mcmc->cmd);

  TIMER_stop(TIMER_MCMC_DISSASEMBLE);

  return 0;
}

 /*****************************************************************************
  *
  *  mcmc_sample
  *
  *****************************************************************************/

int mcmc_sample(mcmc_t *mcmc){

  assert(mcmc);

  TIMER_start(TIMER_MCMC_SAMPLER);

  metropolis_init(mcmc->met, RANDOM);
  metropolis_run(mcmc->met);

  TIMER_stop(TIMER_MCMC_SAMPLER);

  return 0;
}

int mcmc_statistics(mcmc_t *mcmc){

  assert(mcmc);

  TIMER_start(TIMER_MCMC_STATISTICS);

  ess_compute(mcmc->ess);
  ess_print(mcmc->ess);

  TIMER_stop(TIMER_MCMC_STATISTICS);

  return 0;
}

 /*****************************************************************************
  *
  *  mcmc_infer
  *
  *****************************************************************************/

int mcmc_infer(mcmc_t *mcmc){

  chain_t *chain = NULL;
  cmd_t *cmd = NULL;
  assert(mcmc);

  TIMER_start(TIMER_MCMC_INFERENCE);

  data_create(&mcmc->cmd->test, &mcmc->test);
  data_read_file(mcmc->test);

  metropolis_chain(mcmc->met, &chain);
  cmd = chain->cmd;

  infr_create(cmd, chain, mcmc->test, &mcmc->infr);

  infr_mc_integration_lr(mcmc->infr);
  infr_print(mcmc->infr, LOGISTIC_REGRESSION);

  TIMER_stop(TIMER_MCMC_INFERENCE);

  return 0;
}
