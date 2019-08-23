#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>

#include "effective_sample_size.h"
#include "memory.h"
#include "timer.h"

struct ess_s{
  pe_t *pe;               /* Parallel Environment*/
  acr_t *acr;             /* Autocorrelation */
  char ess_case[BUFSIZ];  /* Choise between max, min,
                           * median, mean ESS*/
  precision *ess;         /* ESS case out*/
  precision *max;         /* Max ESS */
  precision *min;         /* Min ESS */
  precision *median;      /* Median ESS */
  precision *mean;        /* Mean ESS */
  int dim;                /* Dimensionality of Samples */
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

int ess_create(pe_t *pe, acr_t *acr, ess_t **pess){

  ess_t *ess = NULL;

  assert(pe);
  assert(acr);

  ess = (ess_t *) calloc(1, sizeof(ess_t));
  assert(ess);
  if(ess == NULL) pe_fatal(pe, "calloc(ess_t) failed\n");

  ess->pe = pe;
  ess->acr = acr;

  ess_dim_set(ess, DIMX_DEFAULT);
  ess_case_set(ess, ESS_CASE_DEFAULT);

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

  mem_free((void **)&ess->ess);
  mem_free((void **)&ess->max);
  mem_free((void **)&ess->min);
  mem_free((void **)&ess->median);
  mem_free((void **)&ess->mean);

  mem_free((void **)&ess);

  return 0;
}

/*****************************************************************************
 *
 *  ess_init_rt
 *
 *****************************************************************************/

int ess_init_rt(rt_t *rt, ess_t *ess){

  int dim;
  char ess_case[BUFSIZ];

  assert(rt);
  assert(ess);

  if(rt_int_parameter(rt, "sample_dim", &dim))
  {
    ess_dim_set(ess, dim);
  }

  if(rt_string_parameter(rt, "ess", ess_case, BUFSIZ))
  {
    ess_case_set(ess, ess_case);
  }

  mem_malloc_precision(&ess->ess, ess->dim);

  if(!strcmp(ess_case, "all") || !strcmp(ess_case, "max"))
    mem_malloc_precision(&ess->max, 1);

  if(!strcmp(ess_case, "all") || !strcmp(ess_case, "min"))
    mem_malloc_precision(&ess->min, 1);

  if(!strcmp(ess_case, "all") || !strcmp(ess_case, "median"))
    mem_malloc_precision(&ess->median, 1);

  if(!strcmp(ess_case, "all") || !strcmp(ess_case, "mean"))
    mem_malloc_precision(&ess->mean, 1);

  return 0;
}

int ess_compute(ess_t *ess){

  int i, j, offset, N;
  precision *acrlagk = NULL;
  int *maxlag_act = NULL;

  assert(ess);

  TIMER_start(TIMER_ESS);

  acr_lagk(ess->acr, &acrlagk);
  acr_maxlag_act(ess->acr, &maxlag_act);
  acr_maxlag(ess->acr, &offset);  // allocated memory between each dimension
  acr_N(ess->acr, &N);

  for(i=0; i<ess->dim; i++)
  {
    ess->ess[i] = 0.0;
    for(j=0; j<maxlag_act[i]; j++)
    {
      ess->ess[i] += acrlagk[i*offset+j];
    }
    ess->ess[i] = N / (1 + 2 * ess->ess[i]);
  }

  if(ess->max) ess_max(ess);
  if(ess->min) ess_min(ess);
  if(ess->median) ess_median(ess);
  if(ess->mean) ess_mean(ess);

  TIMER_stop(TIMER_ESS);

  return 0;
}



/*****************************************************************************
 *
 *  ess_print_ess
 *
 *****************************************************************************/

int ess_print_ess(pe_t *pe, ess_t *ess){

  assert(pe);
  assert(ess);

  pe_info(pe, "Effective Sample Size Summary:\n");
  pe_info(pe, "------------------------------\n");

  if(ess->max) pe_info(pe, "\tMax:\t%f\n", ess->max[0]);
  if(ess->min) pe_info(pe, "\tMin:\t%f\n", ess->min[0]);
  if(ess->median) pe_info(pe, "\tMedian:\t%f\n", ess->median[0]);
  if(ess->mean) pe_info(pe, "\tMean:\t%f\n", ess->mean[0]);

  pe_info(pe, "\n");

  return 0;
}

/*****************************************************************************
 *
 *  ess_info
 *
 *****************************************************************************/

int ess_info(pe_t *pe, ess_t *ess){

  assert(pe);
  assert(ess);

  pe_info(pe, "\n");
  pe_info(pe, "Effective Sample Size Properties\n");
  pe_info(pe, "--------------------------------\n");
  pe_info(pe, "%30s\t\t%s\n", "Selected Case:", ess->ess_case);
  pe_info(pe, "%30s\t\t%d\n", "Dimensionality of Samples:", ess->dim);

  return 0;
}

/*****************************************************************************
 *
 *  ess_dim_set
 *
 *****************************************************************************/

int ess_dim_set(ess_t *ess, int dim){

  assert(ess);

  ess->dim = dim;

  return 0;
}


/*****************************************************************************
 *
 *  ess_case_set
 *
 *****************************************************************************/

int ess_case_set(ess_t *ess, const char *ess_case){

  assert(ess);

  sprintf(ess->ess_case, "%s", ess_case);

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

  for(i=0; i<ess->dim; i++)
  {
    if(ess->ess[i] > max)
    {
      max = ess->ess[i];
    }
  }

  ess->max[0] = max;

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

  for(i=0; i<ess->dim; i++)
  {
    if(ess->ess[i] < min)
    {
      min = ess->ess[i];
    }
  }

  ess->min[0] = min;

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
  int i;

  mem_malloc_precision(&sorted_ess, ess->dim);

  for(i=0; i<ess->dim; i++)
    sorted_ess[i] = ess->ess[i];

  mem_sort_precision(sorted_ess, ess->dim);

  /* In case of even number of elements average the two in the middle */
  if(ess->dim%2 == 0)
    median = (sorted_ess[(ess->dim-1)/2] + sorted_ess[ess->dim/2]) / 2.0;
  else
    median = sorted_ess[ess->dim/2];

  ess->median[0] = median;

  mem_free((void **)&sorted_ess);

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

  for(i=0; i<ess->dim; i++)
  {
    mean += ess->ess[i];
  }

  ess->mean[0] = mean / ess->dim;

  return 0;
}
