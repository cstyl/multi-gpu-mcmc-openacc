#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "autocorrelation.h"
#include "chain.h"
#include "memory.h"
#include "util.h"

static int acr_create_1d(int N, int maxlag, acr_1d_t **pacr_1d);
static precision acr_free_1d(acr_1d_t *acr1d);
static int acr_load_X_array_1d(chain_t *chain, acr_1d_t *acr_1d, int N, int dim, int idx);
static int acr_compute_acr_1d(acr_1d_t *acr_1d);
static precision acr_compute_mean(precision *X, int N);
static precision acr_compute_variance(precision *X, precision mean, int N);
static precision acr_compute_acr_lagk(acr_1d_t *acr_1d, int lag);

/*****************************************************************************
 *
 *  acr_create
 *
 *****************************************************************************/

int acr_create(cmd_t *cmd, met_t *met, acr_t **pacr){

  acr_t    *acr   = NULL;
  acr_1d_t **acr1d = NULL;
  int i;
  assert(cmd);
  assert(met);

  acr = (acr_t *) calloc(1, sizeof(acr_t));
  assert(acr);
  if(acr == NULL)
  {
    printf("calloc(acr_t) failed\n");
    exit(1);
  }

  acr->cmd   = cmd;
  metropolis_chain(met, &acr->chain);

  acr1d = (acr_1d_t **) malloc((cmd->dim+1) * sizeof(acr_1d_t *));
  assert(acr1d);
  if(acr1d == NULL)
  {
    printf("malloc(acr_1d_t) failed\n");
    exit(1);
  }

  for(i=0; i<cmd->dim+1; i++) acr_create_1d(cmd->Ns, cmd->maxlag, &acr1d[i]);

  acr->acr_1d = acr1d;
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

  int i;

  for(i=0; i<acr->cmd->dim+1; i++)
    acr_free_1d(acr->acr_1d[i]);

  free(acr->acr_1d);
  free(acr);

  assert(acr->acr_1d != NULL);
  assert(acr != NULL);

  return 0;
}

/*****************************************************************************
 *
 *  acr_compute_acr
 *
 *****************************************************************************/

int acr_compute_acr(acr_t *acr){

  assert(acr);

  int i;

  for(i=0; i<acr->cmd->dim+1; i++)
  {
    acr_load_X_array_1d(acr->chain, acr->acr_1d[i], acr->cmd->Ns, acr->cmd->dim+1, i);
    acr_compute_acr_1d(acr->acr_1d[i]);
  }

  return 0;
}

/*****************************************************************************
 *
 *  acr_print_acr
 *
 *****************************************************************************/

int acr_print_acr(acr_t *acr){

  assert(acr);

  int i;

  printf("Autocorrelation Summary:\n");
  printf("\t%10s\t%10s\t%10s\t%10s\n", "Dim", "ESS", "Mean", "Variance");

  for(i=0; i<acr->cmd->dim+1; i++)
  {
    printf("\t%10d", i);
    printf("\t%10.4f", acr->acr_1d[i]->ess);
    printf("\t%10.4f", acr->acr_1d[i]->mean);
    printf("\t%10.4f\n", acr->acr_1d[i]->variance);
  }
  printf("\n");

  return 0;
}

/*****************************************************************************
 *
 *  acr_write_acr
 *
 *****************************************************************************/

int acr_write_acr(acr_t *acr){

  cmd_t *cmd = NULL;
  assert(acr);

  cmd = acr->cmd;

  char outdir[BUFSIZ], filename[BUFSIZ];
  FILE *fp;

  sprintf(outdir, "%s/%s", cmd->outdir, "autocorrelation");
  util_create_dir(outdir);

  printf("Writing autocorrelation files...");

  int i,j;
  for(i=0; i<cmd->dim+1; i++)
  {
    sprintf(filename, "%s/%s_%d.csv", outdir, "rho", i);
    fp = fopen(filename, "w+");
    assert(fp);

    for(j=0; j<cmd->Ns; j++)
    {
#ifdef FLOAT
      fprintf(fp, "%.7f\n", acr->acr_1d[i]->acr_lagk[j]);
#else
      fprintf(fp, "%.16f\n", acr->acr_1d[i]->acr_lagk[j]);
#endif
    }

    fclose(fp);
  }

  printf("\tDone\n");

  return 0;

}

/*****************************************************************************
 *
 *  acr_create_1d
 *
 *****************************************************************************/

static int acr_create_1d(int N, int maxlag, acr_1d_t **pacr_1d){

  acr_1d_t *acr1d = NULL;

  acr1d = (acr_1d_t *) calloc(1, sizeof(acr_1d_t));
  assert(acr1d);
  if(acr1d == NULL)
  {
    printf("calloc(acr_1d_t) failed\n");
    exit(1);
  }

  acr1d->N = N;
  acr1d->maxlag = maxlag;
  mem_malloc_precision(&acr1d->X, acr1d->N);
  mem_malloc_precision(&acr1d->acr_lagk, acr1d->maxlag);

  *pacr_1d = acr1d;

  return 0;
}

/*****************************************************************************
 *
 *  acr_free_1d
 *
 *****************************************************************************/

static precision acr_free_1d(acr_1d_t *acr1d){

  assert(acr1d);

  free(acr1d->X);
  free(acr1d->acr_lagk);
  free(acr1d);

  assert(acr1d->X != NULL);
  assert(acr1d->acr_lagk != NULL);
  assert(acr1d != NULL);

  return 0;
}

/*****************************************************************************
 *
 *  acr_load_array_1d
 *
 *****************************************************************************/

static int acr_load_X_array_1d(chain_t *chain, acr_1d_t *acr_1d, int N, int dim, int idx){

  assert(chain);
  assert(acr_1d);

  int i;
  for(i=0; i<N; i++)
    acr_1d->X[i] = chain->samples[i*dim+idx];

  return 0;
}

/*****************************************************************************
 *
 *  acr_compute_acr_1d
 *
 *****************************************************************************/

static int acr_compute_acr_1d(acr_1d_t *acr_1d){

  int i;

  assert(acr_1d);

  acr_1d->mean = acr_compute_mean(acr_1d->X, acr_1d->N);
  acr_1d->variance = acr_compute_variance(acr_1d->X, acr_1d->mean, acr_1d->N);

  for(i=0; i<acr_1d->maxlag; i++)
    acr_1d->acr_lagk[i] = acr_compute_acr_lagk(acr_1d, i);

  acr_1d->ess = 0.0;
  for(i=0; i<acr_1d->maxlag; i++)
    acr_1d->ess += acr_1d->acr_lagk[i];

  acr_1d->ess = acr_1d->N / (1 + 2 * acr_1d->ess);

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

/*****************************************************************************
 *
 *  acr_compute_acr_lagk
 *
 *****************************************************************************/

static precision acr_compute_acr_lagk(acr_1d_t *acr_1d, int lag){

  int i;
  precision acrk = 0.0;

  assert(acr_1d);

  for(i=0; i<acr_1d->N-lag; i++)
  {
    acrk += (acr_1d->X[i]-acr_1d->mean) * (acr_1d->X[i+lag]-acr_1d->mean);
  }
  acrk *= 1.0 / (acr_1d->N - lag);

  /* Interested in monotonically decreasing positive autocorrelations
   * Once values reach to -ve can be considered as noise and discarded
   */
  return (acrk >= 0) ? (acrk / acr_1d->variance) : 0.0;
}
