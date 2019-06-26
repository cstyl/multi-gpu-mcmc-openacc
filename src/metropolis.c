#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "metropolis.h"
#include "sample.h"
#include "chain.h"

struct met_s{
  cmd_t    *cmd;
  rng_t    *rng;
  data_t   *data;
  sample_t *current;
  sample_t *proposed;
  chain_t  *chain;
};

enum metropolis_error {METROPOLIS_SUCCESS = 0,
                       METROPOLIS_ERROR
};

/*****************************************************************************
 *
 *  metropolis_create
 *
 *****************************************************************************/

int metropolis_create(cmd_t *cmd, rng_t *rng, data_t *data, met_t **pmet){

  met_t *met = NULL;

  assert(cmd);
  assert(rng);
  assert(data);

  met = (met_t *) calloc(1, sizeof(met_t));
  assert(met);
  if(met == NULL)
  {
    printf("calloc(met_t) failed\n");
    exit(METROPOLIS_ERROR);
  }

  met->cmd = cmd;
  met->rng = rng;
  met->data = data;

  // lh_create(&cmd->params, &met->lhood);
  chain_create(met->cmd, &met->chain);
  sample_create(met->cmd, &met->current);
  sample_create(met->cmd, &met->proposed);

  *pmet = met;

  return METROPOLIS_SUCCESS;
}

/*****************************************************************************
 *
 *  metropolis_free
 *
 *****************************************************************************/

int metropolis_free(met_t *met){

  assert(met);

  sample_free(met->current);
  sample_free(met->proposed);
  chain_free(met->chain);

  free(met);

  assert(met->current != NULL);
  assert(met->proposed != NULL);
  assert(met->chain != NULL);
  assert(met != NULL);

  return METROPOLIS_SUCCESS;
}

/*****************************************************************************
 *
 *  metropolis_init
 *
 *****************************************************************************/

int metropolis_init(met_t *met, int random){

  assert(met);

  cmd_t    *cmd   = NULL;
  sample_t *cur   = NULL;
  rng_t    *rng   = NULL;
  chain_t  *chain = NULL;
  data_t   *data  = NULL;

  cmd   = met->cmd;
  cur   = met->current;
  rng   = met->rng;
  chain = met->chain;
  data  = met->data;

  sample_init(cmd, rng, data, chain, cur, random);

  return METROPOLIS_SUCCESS;
}

/*****************************************************************************
 *
 *  metropolis_run
 *
 *****************************************************************************/

int metropolis_run(met_t *met){

  cmd_t    *cmd   = NULL;
  sample_t *cur   = NULL;
  sample_t *pro   = NULL;
  rng_t    *rng   = NULL;
  chain_t  *chain = NULL;
  data_t   *data  = NULL;
  int i;

  assert(met);

  cmd   = met->cmd;
  cur   = met->current;
  pro   = met->proposed;
  rng   = met->rng;
  data  = met->data;
  chain = met->chain;

  chain->accepted[0] = 0;
  for(i=1; i<cmd->Nburn; i++)
  {
    sample_propose(cmd, rng, cur, pro);
    chain->probability[i] = sample_evaluate(cmd, rng, data, cur, pro);
    sample_choose(i, cmd, rng, chain, &cur, &pro);
  }

  for(i=cmd->Nburn; i<(cmd->Nburn + cmd->Ns); i++)
  {
    sample_propose(cmd, rng, cur, pro);
    chain->probability[i] = sample_evaluate(cmd, rng, data, cur, pro);
    sample_choose(i, cmd, rng, chain, &cur, &pro);
  }

  return METROPOLIS_SUCCESS;
}
