#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "autocorrelation.h"
#include "memory.h"
#include "timer.h"

struct acr_s{
  pe_t *pe;
  ch_t *chain;
  int N;                      /* Number of samples per dimension */
  int dim;                    /* Dimensionality of samples */
  int maxlag;                 /* Default Max lag limit */
  precision threshold;        /* Maximum autocorrelation threshold */
  precision *X;
  precision *mean;
  precision *variance;
  int *offset;
  int *maxlag_act;            /* Actual maxlag per dimension */
  precision *lagk;        /* Autocorrelations for k-lags. Contiguous per dimensionality */
  int outfreq;
  char outdir[FILENAME_MAX];
};

static const int DIM_AUTO_DEFAULT = 3;
static const int N_AUTO_DEFAULT = 500;
static const int MAXLAG_AUTO_DEFAULT = 249;
static const precision THRESHOLD_AUTO_DEFAULT = 0.1;
static const int OUTFREQ_AUTO_DEFAULT = 50;
static const char OUTDIR_AUTO_DEFAULT[FILENAME_MAX] = "../out";

static int acr_allocate_X(acr_t *acr);
static int acr_allocate_mean(acr_t *acr);
static int acr_allocate_variance(acr_t *acr);
static int acr_allocate_offset(acr_t *acr);
static int acr_allocate_maxlag_act(acr_t *acr);
static int acr_allocate_lagk(acr_t *acr);
static int acr_load_X(ch_t *chain, int N, int dim, int idx, precision *X);
static precision acr_compute_mean(precision *X, int N);
static precision acr_compute_variance(precision *X, precision mean, int N);

/*****************************************************************************
 *
 *  acr_create
 *
 *****************************************************************************/

int acr_create(pe_t *pe, ch_t *chain, acr_t **pacr){

  acr_t *acr = NULL;

  assert(pe);
  assert(chain);

  acr = (acr_t *) calloc(1, sizeof(acr_t));
  assert(acr);
  if(acr == NULL) pe_fatal(pe, "calloc(acr_t) failed\n");

  acr->pe   = pe;
  acr->chain = chain;

  acr_dim_set(acr, DIM_AUTO_DEFAULT);
  acr_N_set(acr, N_AUTO_DEFAULT);
  acr_maxlag_set(acr, MAXLAG_AUTO_DEFAULT);
  acr_threshold_set(acr, THRESHOLD_AUTO_DEFAULT);
  acr_outfreq_set(acr, OUTFREQ_AUTO_DEFAULT);
  acr_outdir_set(acr, OUTDIR_AUTO_DEFAULT);

  *pacr = acr;

  return 0;
}

/*****************************************************************************
 *
 *  acr_free
 *
 *****************************************************************************/

int acr_free(acr_t *acr){

  assert(acr);

  mem_free((void**)&acr->X);
  mem_free((void**)&acr->mean);
  mem_free((void**)&acr->variance);
  mem_free((void**)&acr->offset);
  mem_free((void**)&acr->maxlag_act);
  mem_free((void**)&acr->lagk);

  mem_free((void**)&acr);

  return 0;
}

/*****************************************************************************
 *
 *  acr_init_rt
 *
 *****************************************************************************/

int acr_init_rt(rt_t *rt, ch_t *chain, acr_t *acr){

  int i, dim, N, outfreq, maxlag;
  double threshold;
  char outdir[FILENAME_MAX];

  assert(rt);
  assert(chain);
  assert(acr);

  ch_dim(chain, &dim);
  acr_dim_set(acr, dim);

  ch_N(chain, &N);
  acr_N_set(acr, N);

  if(rt_int_parameter(rt, "max_lag", &maxlag))
  {
    acr_maxlag_set(acr, maxlag);
  }

  if(rt_double_parameter(rt, "lag_threshold", &threshold))
  {
    acr_threshold_set(acr, (precision)threshold);
  }

  if(rt_int_parameter(rt, "freq_autocorr", &outfreq))
  {
    acr_outfreq_set(acr, outfreq);
  }

  if(rt_string_parameter(rt, "outdir", outdir, FILENAME_MAX))
  {
    acr_outdir_set(acr, outdir);
  }

  acr_allocate_X(acr);
  acr_allocate_mean(acr);
  acr_allocate_variance(acr);
  acr_allocate_offset(acr);
  acr_allocate_maxlag_act(acr);
  acr_allocate_lagk(acr);

  for(i=0; i<acr->dim; i++) acr->offset[i] = acr->N * i;

  return 0;
}

/*****************************************************************************
 *
 *  acr_compute
 *
 *****************************************************************************/

int acr_compute(acr_t *acr){

  assert(acr);
  precision lagk;
  int i, j, offset;
  int N=acr->N, dim=acr->dim;

  TIMER_start(TIMER_AUTOCORRELATION);
  /* Create a contiguous block of data per dimension */
  for(i=0; i<dim; i++)
  {
    lagk = 0.0;
    offset = acr->offset[i];
    acr_load_X(acr->chain, N, dim, i, &acr->X[offset]);

    acr->mean[i] = acr_compute_mean(&acr->X[offset], N);
    acr->variance[i] = acr_compute_variance(&acr->X[offset], acr->mean[i], N);

    for(j=0; j<acr->maxlag; j++)
    {
      lagk = acr_compute_lagk(&acr->X[offset], acr->mean[i], acr->variance[i], N, j);

      if(lagk < acr->threshold){
        acr->maxlag_act[i] = j-1;
        break;
      }
      acr->lagk[i*acr->maxlag+j] = lagk;
    }
  }
  TIMER_stop(TIMER_AUTOCORRELATION);

  return 0;
}

/*****************************************************************************
 *
 *  acr_compute_lagk
 *
 *****************************************************************************/

precision acr_compute_lagk(precision *X, precision mu, precision var, int N, int lag){

  int i;
  precision acrk = 0.0;

  assert(X);

  for(i=0; i< N-lag; i++)
  {
    acrk += (X[i]-mu) * (X[i+lag]-mu);
  }
  acrk *= 1.0 / (N - lag);

  /* Interested in monotonically decreasing positive autocorrelations
   * Once values reach to -ve can be considered as noise and discarded
   */
  return (acrk >= 0) ? (acrk / var) : 0.0;
}

/*****************************************************************************
 *
 *  acr_info
 *
 *****************************************************************************/

int acr_info(pe_t *pe, acr_t *acr){

  int dim, N, outfreq, maxlag;
  precision threshold;
  char outdir[FILENAME_MAX];

  assert(pe);
  assert(acr);

  acr_dim(acr, &dim);
  acr_N(acr, &N);
  acr_maxlag(acr, &maxlag);
  acr_threshold(acr, &threshold);
  acr_outfreq(acr, &outfreq);
  acr_outdir(acr, outdir);

  pe_info(pe, "\n");
  pe_info(pe, "Autocorrelation Properties\n");
  pe_info(pe, "--------------------------\n");
  pe_info(pe, "%30s\t\t%d\n", "Length:", N);
  pe_info(pe, "%30s\t\t%d\n", "Dimensionality of Samples:", dim);
  pe_info(pe, "%30s\t\t%d\n", "Max Lag:", maxlag);
  pe_info(pe, "%30s\t\t%f\n", "Autocorrelation Threshold:", threshold);
  pe_info(pe, "%30s\t\t%d %s\n", "Output Frequency:", outfreq, "iterations");
  pe_info(pe, "%30s\t\t%s\n", "Output directory:", outdir);

  return 0;
}

/*****************************************************************************
 *
 *  acr_print_acr
 *
 *****************************************************************************/

int acr_print_acr(pe_t *pe, acr_t *acr){

  assert(acr);

  int i;

  pe_info(pe, "Autocorrelation Summary:\n");
  pe_info(pe, "\t%10s\t%10s\t%10s\t%10s\n", "Dim", "Mean", "Variance", "MaxLag");

  for(i=0; i<acr->dim; i++)
  {
    pe_info(pe, "\t%10d", i);
    pe_info(pe, "\t%10.4f", acr->mean[i]);
    pe_info(pe, "\t%10.4f", acr->variance[i]);
    pe_info(pe, "\t%10d", acr->maxlag_act[i]);
    pe_info(pe, "\n");
  }
  pe_info(pe, "\n");

  return 0;
}

/*****************************************************************************
 *
 *  acr_dim_set
 *
 *****************************************************************************/

int acr_dim_set(acr_t *acr, int dim){

  assert(acr);

  acr->dim = dim;

  return 0;
}

/*****************************************************************************
 *
 *  acr_dim
 *
 *****************************************************************************/

int acr_dim(acr_t *acr, int *dim){

  assert(acr);

  *dim = acr->dim;

  return 0;
}

/*****************************************************************************
 *
 *  acr_N_set
 *
 *****************************************************************************/

int acr_N_set(acr_t *acr, int N){

  assert(acr);

  acr->N = N;

  return 0;
}

/*****************************************************************************
 *
 *  acr_N
 *
 *****************************************************************************/

int acr_N(acr_t *acr, int *N){

  assert(acr);

  *N = acr->N;

  return 0;
}

/*****************************************************************************
 *
 *  acr_maxlag_set
 *
 *****************************************************************************/

int acr_maxlag_set(acr_t *acr, int maxlag){

  assert(acr);

  acr->maxlag = maxlag;

  return 0;
}

/*****************************************************************************
 *
 *  acr_maxlag
 *
 *****************************************************************************/

int acr_maxlag(acr_t *acr, int *maxlag){

  assert(acr);

  *maxlag = acr->maxlag;

  return 0;
}

/*****************************************************************************
 *
 *  acr_threshold_set
 *
 *****************************************************************************/

int acr_threshold_set(acr_t *acr, precision threshold){

  assert(acr);

  acr->threshold = threshold;

  return 0;
}

/*****************************************************************************
 *
 *  acr_threshold
 *
 *****************************************************************************/

int acr_threshold(acr_t *acr, precision *threshold){

  assert(acr);

  *threshold = acr->threshold;

  return 0;
}

/*****************************************************************************
 *
 *  acr_outfreq_set
 *
 *****************************************************************************/

int acr_outfreq_set(acr_t *acr, int outfreq){

  assert(acr);

  acr->outfreq = outfreq;

  return 0;
}

/*****************************************************************************
 *
 *  acr_outfreq
 *
 *****************************************************************************/

int acr_outfreq(acr_t *acr, int *outfreq){

  assert(acr);

  *outfreq = acr->outfreq;

  return 0;
}

/*****************************************************************************
 *
 *  acr_outdir_set
 *
 *****************************************************************************/

int acr_outdir_set(acr_t *acr, const char *outdir){

  assert(acr);

  sprintf(acr->outdir, "%s", outdir);

  return 0;
}

/*****************************************************************************
 *
 *  acr_outdir
 *
 *****************************************************************************/

int acr_outdir(acr_t *acr, char *outdir){

  assert(acr);

  sprintf(outdir, "%s", acr->outdir);

  return 0;
}

/*****************************************************************************
 *
 *  acr_X
 *
 *****************************************************************************/

int acr_X(acr_t *acr, precision **pX){

  assert(acr);

  *pX = acr->X;

  return 0;
}

/*****************************************************************************
 *
 *  acr_mean
 *
 *****************************************************************************/

int acr_mean(acr_t *acr, precision **pmean){

  assert(acr);

  *pmean = acr->mean;

  return 0;
}

/*****************************************************************************
 *
 *  acr_variance
 *
 *****************************************************************************/

int acr_variance(acr_t *acr, precision **pvariance){

  assert(acr);

  *pvariance = acr->variance;

  return 0;
}

/*****************************************************************************
 *
 *  acr_offset
 *
 *****************************************************************************/

int acr_offset(acr_t *acr, int **poffset){

  assert(acr);

  *poffset = acr->offset;

  return 0;
}

/*****************************************************************************
 *
 *  acr_maxlag_act
 *
 *****************************************************************************/

int acr_maxlag_act(acr_t *acr, int **pmaxlag_act){

  assert(acr);

  *pmaxlag_act = acr->maxlag_act;

  return 0;
}

/*****************************************************************************
 *
 *  acr_lagk
 *
 *****************************************************************************/

int acr_lagk(acr_t *acr, precision **plagk){

  assert(acr);

  *plagk = acr->lagk;

  return 0;
}

/*****************************************************************************
 *
 *  acr_allocate_X
 *
 *****************************************************************************/

static int acr_allocate_X(acr_t *acr){

  assert(acr);

  mem_malloc_precision(&acr->X, acr->N*acr->dim);

  return 0;
}

/*****************************************************************************
 *
 *  acr_allocate_mean
 *
 *****************************************************************************/

static int acr_allocate_mean(acr_t *acr){

  assert(acr);

  mem_malloc_precision(&acr->mean, acr->dim);

  return 0;
}

/*****************************************************************************
 *
 *  acr_allocate_variance
 *
 *****************************************************************************/

static int acr_allocate_variance(acr_t *acr){

  assert(acr);

  mem_malloc_precision(&acr->variance, acr->dim);

  return 0;
}

/*****************************************************************************
 *
 *  acr_allocate_offset
 *
 *****************************************************************************/

static int acr_allocate_offset(acr_t *acr){

  assert(acr);

  mem_malloc_integers(&acr->offset, acr->dim);

  return 0;
}

/*****************************************************************************
 *
 *  acr_allocate_maxlag_act
 *
 *****************************************************************************/

static int acr_allocate_maxlag_act(acr_t *acr){

  assert(acr);

  mem_malloc_integers(&acr->maxlag_act, acr->dim);

  return 0;
}


/*****************************************************************************
*
*  acr_allocate_X
*
*****************************************************************************/

static int acr_allocate_lagk(acr_t *acr){

assert(acr);

mem_malloc_precision(&acr->lagk, (acr->maxlag+1)*acr->dim);

return 0;
}

/*****************************************************************************
 *
 *  acr_load_X
 *
 *****************************************************************************/

static int acr_load_X(ch_t *chain, int N, int dim, int idx, precision *X){

  assert(chain);

  int i;
  precision *samples = NULL;

  ch_samples(chain, &samples);

  for(i=0; i<N; i++)
    X[i] = samples[i*dim+idx];

  return 0;
}

/*****************************************************************************
 *
 *  acr_compute_mean
 *
 *****************************************************************************/

static precision acr_compute_mean(precision *X, int N){

  int i;
  precision mean = 0.0;
  assert(X);

  for(i=0; i<N; i++)
    mean += X[i];

  return (mean / N);
}

/*****************************************************************************
 *
 *  acr_compute_variance
 *
 *****************************************************************************/

static precision acr_compute_variance(precision *X, precision mean, int N){

  int i;
  precision var = 0.0;
  assert(X);

  for(i=0; i<N; i++)
    var += pow((X[i]-mean), 2.0);

  return (var / N);
}
