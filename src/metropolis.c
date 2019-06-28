#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "metropolis.h"
#include "sample.h"

struct met_s{
  cmd_t    *cmd;
  rng_t    *rng;
  data_t   *data;
  sample_t *current;
  sample_t *proposed;
  chain_t  *bchain;     /* Chain during burn in period */
  chain_t  *chain;      /* Chain during post-burn in in period */
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
    exit(1);
  }

  met->cmd = cmd;
  met->rng = rng;
  met->data = data;

  chain_create(cmd, &met->bchain, cmd->Nburn, cmd->dim);
  chain_create(cmd, &met->chain, cmd->Ns, cmd->dim);

  sample_create(met->cmd, &met->current);
  sample_create(met->cmd, &met->proposed);

  *pmet = met;

  return 0;
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

  if(met->chain) chain_free(met->chain);
  if(met->bchain) chain_free(met->bchain);

  free(met);

  assert(met->current != NULL);
  assert(met->proposed != NULL);
  assert(met->bchain != NULL);
  assert(met->chain != NULL);
  assert(met != NULL);

  return 0;
}

/*****************************************************************************
 *
 *  metropolis_init
 *
 *****************************************************************************/

int metropolis_init(met_t *met, int random){

  assert(met);

  cmd_t    *cmd    = NULL;
  sample_t *cur    = NULL;
  rng_t    *rng    = NULL;
  chain_t  *bchain = NULL;
  data_t   *data   = NULL;

  cmd    = met->cmd;
  cur    = met->current;
  rng    = met->rng;
  bchain = met->bchain;
  data   = met->data;

  sample_init(cmd, rng, data, bchain, cur, random);

  return 0;
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
  chain_t  *bchain = NULL;
  chain_t  *chain = NULL;
  data_t   *data  = NULL;
  int i;

  assert(met);

  cmd   = met->cmd;
  cur   = met->current;
  pro   = met->proposed;
  rng   = met->rng;
  data  = met->data;
  bchain = met->bchain;
  chain = met->chain;

  printf("\nStarting burn-in period of %d steps..\n", cmd->Nburn);
  chain_init_stats(0, bchain);
  for(i=1; i<=cmd->Nburn; i++)
  {
    sample_propose(cmd, rng, cur, pro);
    bchain->probability[i] = sample_evaluate(cmd, rng, data, cur, pro);
    sample_choose(i, cmd, rng, bchain, &cur, &pro);
  }

  printf("\nStarting post burn-in period of %d steps..\n", cmd->Ns);
  chain_init_stats(0, chain);
  for(i=1; i<=cmd->Ns; i++)
  {
    sample_propose(cmd, rng, cur, pro);
    chain->probability[i] = sample_evaluate(cmd, rng, data, cur, pro);
    sample_choose(i, cmd, rng, chain, &cur, &pro);
  }

  return 0;
}

/*****************************************************************************
 *
 *  metropolis_chain
 *
 *****************************************************************************/

int metropolis_chain(met_t *met, chain_t **pchain){

  assert(met);

  *pchain = met->chain;

  return 0;
}
