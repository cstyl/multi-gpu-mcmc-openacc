#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "chain.h"
#include "memory.h"

/*****************************************************************************
 *
 *  chain_create
 *
 *****************************************************************************/

int chain_create(cmd_t *cmd, chain_t **pchain, int size, int dim){

  chain_t *chain = NULL;

  assert(cmd);

  chain = (chain_t *) calloc(1, sizeof(chain_t));
  assert(chain);
  if(chain == NULL)
  {
    printf("calloc(chain_t) failed\n");
    exit(1);
  }

  chain->cmd = cmd;

  mem_malloc_precision(&chain->samples, (size+1) * (dim+1));
  mem_malloc_precision(&chain->probability, size+1);
  mem_malloc_precision(&chain->ratio, size+1);
  mem_malloc_integers(&chain->accepted, size+1);

  *pchain = chain;

  return 0;
}

/*****************************************************************************
 *
 *  chain_free
 *
 *****************************************************************************/

int chain_free(chain_t *chain){

  assert(chain);

  free(chain->samples);
  free(chain->probability);
  free(chain->ratio);
  free(chain->accepted);
  free(chain);

  assert(chain->samples != NULL);
  assert(chain->probability != NULL);
  assert(chain->ratio != NULL);
  assert(chain->accepted != NULL);

  assert(chain != NULL);

  return 0;
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
  int size = chain->cmd->dim+1;

  for(idx=0; idx<size; idx++) chain->samples[i*size+idx] = sample[idx];

  return 0;
}

/*****************************************************************************
 *
 *  chain_append_stats
 *
 *****************************************************************************/

int chain_append_stats(int i, int accepted, chain_t *chain){

  assert(chain);
  assert(i);

  chain->accepted[i] = chain->accepted[i-1] + accepted;
  chain->ratio[i] = (precision)chain->accepted[i] / (precision)i;

  return 0;
}

/*****************************************************************************
 *
 *  chain_init_stats
 *
 *****************************************************************************/

int chain_init_stats(int i, chain_t *chain){

  assert(chain);

  chain->probability[i] = 0.0;
  chain->accepted[i] = 0;
  chain->ratio[i] = 0.0;

  return 0;
}
