#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command_line_parser.h"
#include "data_input.h"
#include "random_number_generator.h"

#include "metropolis.h"

#include "mcmc.h"

#define RANDOM 0

struct mcmc_s{
  cmd_t  *cmd;             /* Command line arguments */
  data_t *train;           /* Input data for training */
  data_t *test;            /* Data for inferencing */
  rng_t  *rng;             /* Random Number Generator */
  met_t  *met;             /* Metropolis Sampling */
};

enum mcmc_error {MCMC_SUCCESS = 0,
                 MCMC_ERROR
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
      exit(MCMC_ERROR);
    }

    *pmcmc = mcmc;

    return MCMC_SUCCESS;
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

  return MCMC_SUCCESS;
}

/*****************************************************************************
 *
 *  mcmc_setup
 *
 *****************************************************************************/

int mcmc_setup(int an, char *av[], mcmc_t *mcmc){

   cmd_create(&mcmc->cmd);
   cmd_parse(an, av, mcmc->cmd);
   cmd_print_status(mcmc->cmd);

   data_create(&mcmc->cmd->train, &mcmc->train);
   data_read_file(mcmc->train);
   // data_print_file(mcmc->train);

   rng_create(mcmc->cmd, &mcmc->rng);
   rng_setup(mcmc->rng);

   metropolis_create(mcmc->cmd, mcmc->rng, mcmc->train, &mcmc->met);

   return MCMC_SUCCESS;
}

/*****************************************************************************
 *
 *  mcmc_disassemble
 *
 *****************************************************************************/

int mcmc_disassemble(mcmc_t *mcmc){

  assert(mcmc);

  if(mcmc->train) data_free(mcmc->train);
  if(mcmc->test)  data_free(mcmc->test);
  if(mcmc->met)   metropolis_free(mcmc->met);

  rng_free(mcmc->rng);
  cmd_free(mcmc->cmd);
  return MCMC_SUCCESS;
}

 /*****************************************************************************
  *
  *  mcmc_sample
  *
  *****************************************************************************/

int mcmc_sample(mcmc_t *mcmc){

  metropolis_init(mcmc->met, RANDOM);
  metropolis_run(mcmc->met);

  return MCMC_SUCCESS;
}

 /*****************************************************************************
  *
  *  mcmc_infer
  *
  *****************************************************************************/

int mcmc_infer(mcmc_t *mcmc){

  return MCMC_SUCCESS;
}
