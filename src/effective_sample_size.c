#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>

#include "effective_sample_size.h"
#include "autocorrelation.h"
#include "chain.h"
#include "memory.h"

struct ess_s{
  cmd_t *cmd;
  chain_t *chain;
  acr_t *acr;
  precision max;
  precision min;
  precision median;
  precision mean;
};

static int ess_max(ess_t *ess);
static int ess_min(ess_t *ess);
static int ess_median(ess_t *ess);
static int ess_mean(ess_t *ess);

/*****************************************************************************
 *
 *  ess_create
 *
 *****************************************************************************/

int ess_create(cmd_t *cmd, met_t *met, ess_t **pess){

  ess_t *ess = NULL;
  assert(cmd);
  assert(met);

  ess = (ess_t *) calloc(1, sizeof(ess_t));
  assert(ess);
  if(ess == NULL)
  {
    printf("calloc(ess_t) failed\n");
    exit(1);
  }

  ess->cmd   = cmd;
  metropolis_chain(met, &ess->chain);
  acr_create(ess->cmd, met, &ess->acr);

  *pess = ess;

  return 0;
}

/*****************************************************************************
 *
 *  ess_free
 *
 *****************************************************************************/

int ess_free(ess_t *ess){

  assert(ess);

  if(ess->acr)   acr_free(ess->acr);

  free(ess);

  assert(ess->acr);
  assert(ess);

  return 0;
}

/*****************************************************************************
 *
 *  ess_compute
 *
 *****************************************************************************/

int ess_compute(ess_t *ess){

  acr_compute_acr(ess->acr);
  ess_max(ess);
  ess_min(ess);
  ess_median(ess);
  ess_mean(ess);

  return 0;
}

/*****************************************************************************
 *
 *  ess_print
 *
 *****************************************************************************/

int ess_print(ess_t *ess){

  assert(ess);

  acr_print_acr(ess->acr);

  printf("ESS Summary:\n");
  printf("\t%10s\t%10s\t%10s\t%10s\n", "Max", "Min", "Median", "Mean");
  printf("\t%10.4f", ess->max);
  printf("\t%10.4f", ess->min);
  printf("\t%10.4f", ess->median);
  printf("\t%10.4f\n\n", ess->mean);

  return 0;
}

/*****************************************************************************
 *
 *  ess_max
 *
 *****************************************************************************/

static int ess_max(ess_t *ess){

  assert(ess);
  precision max = FLT_MIN;
  int i;

  for(i=0; i<ess->cmd->dim+1; i++)
  {
    if(ess->acr->acr_1d[i]->ess > max)
    {
      max = ess->acr->acr_1d[i]->ess;
    }
  }

  ess->max = max;

  return 0;
}

/*****************************************************************************
 *
 *  ess_min
 *
 *****************************************************************************/

static int ess_min(ess_t *ess){

  assert(ess);
  precision min = FLT_MAX;
  int i;

  for(i=0; i<ess->cmd->dim+1; i++)
  {
    if(ess->acr->acr_1d[i]->ess < min)
    {
      min = ess->acr->acr_1d[i]->ess;
    }
  }

  ess->min = min;

  return 0;
}

/*****************************************************************************
 *
 *  ess_median
 *
 *****************************************************************************/

static int ess_median(ess_t *ess){

  assert(ess);
  precision *sorted_ess = NULL;
  precision median = 0.0;
  int i, n = ess->cmd->dim+1;

  mem_malloc_precision(&sorted_ess, n);

  for(i=0; i<n; i++)
    sorted_ess[i] = ess->acr->acr_1d[i]->ess;

  mem_sort_precision(sorted_ess, n);

  /* In case of even number of elements average the two in the middle */
  if(n%2 == 0)
    median = (sorted_ess[(n-1)/2] + sorted_ess[n/2]) / 2.0;
  else
    median = sorted_ess[n/2];

  ess->median = median;

  free(sorted_ess);
  assert(sorted_ess != NULL);

  return 0;
}

/*****************************************************************************
 *
 *  ess_mean
 *
 *****************************************************************************/

static int ess_mean(ess_t *ess){

  assert(ess);
  precision mean = 0.0;
  int i;

  for(i=0; i<ess->cmd->dim+1; i++)
  {
    mean += ess->acr->acr_1d[i]->ess;
  }

  ess->mean = mean / (ess->cmd->dim+1);

  return 0;
}
