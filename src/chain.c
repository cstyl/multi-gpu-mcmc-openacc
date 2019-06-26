#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "chain.h"
#include "memory.h"

enum chain_error {CHAIN_SUCCESS = 0,
                  CHAIN_ERROR
};

/*****************************************************************************
 *
 *  chain_create
 *
 *****************************************************************************/

int chain_create(cmd_t *cmd, chain_t **pchain){

  chain_t *chain = NULL;

  assert(cmd);

  chain = (chain_t *) calloc(1, sizeof(chain_t));
  assert(chain);
  if(chain == NULL)
  {
    printf("calloc(chain_t) failed\n");
    exit(CHAIN_ERROR);
  }

  chain->cmd = cmd;

  mem_malloc_precision(&chain->samples, (chain->cmd->Ns + chain->cmd->Nburn)
                                        * (chain->cmd->dim + 1));
  mem_malloc_precision(&chain->probability, chain->cmd->Ns + chain->cmd->Nburn);
  mem_malloc_precision(&chain->ratio, chain->cmd->Ns + chain->cmd->Nburn);
  mem_malloc_integers(&chain->accepted, chain->cmd->Ns + chain->cmd->Nburn);

  *pchain = chain;

  return CHAIN_SUCCESS;
}

/*****************************************************************************
 *
 *  chain_free
 *
 *****************************************************************************/

int chain_free(chain_t *chain){

  assert(chain);

  if(chain->samples) free(chain->samples);
  if(chain->probability) free(chain->probability);
  if(chain->ratio) free(chain->ratio);
  if(chain->accepted) free(chain->accepted);

  assert(chain->samples != NULL);
  assert(chain->probability != NULL);
  assert(chain->ratio != NULL);
  assert(chain->accepted != NULL);

  assert(chain != NULL);

  return CHAIN_SUCCESS;
}

/*****************************************************************************
 *
 *  chain_append_sample
 *
 *****************************************************************************/

int chain_append_sample(int i, precision *sample, chain_t *chain){

  assert(chain);
  assert(sample);

  int idx;
  cmd_t *cmd = NULL;
  cmd = chain->cmd;

  for(idx=0; idx<(cmd->dim+1); idx++)
  {
    chain->samples[i*(cmd->dim+1)+idx] = sample[idx];
  }

  return CHAIN_SUCCESS;
}

/*****************************************************************************
 *
 *  chain_append_stats
 *
 *****************************************************************************/

int chain_append_stats(int i, precision probability, int accepted, chain_t *chain){

  assert(chain);
  assert(i);

  chain->accepted[i] = accepted;
  chain->ratio[i] = accepted / i;
  chain->probability[i] = probability;

  return CHAIN_SUCCESS;
}
