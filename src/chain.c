#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "chain.h"
#include "memory.h"
#include "util.h"

struct ch_s{
  int dim;
  int N;
  precision *samples;
  precision *probability;
  precision *ratio;
  int *accepted;
  int outfreq;
  char outdir[FILENAME_MAX];
};

static int ch_allocate_samples(ch_t *chain);
static int ch_allocate_probability(ch_t *chain);
static int ch_allocate_ratio(ch_t *chain);
static int ch_allocate_accepted(ch_t *chain);

/*****************************************************************************
 *
 *  ch_create
 *
 *****************************************************************************/

int ch_create(pe_t *pe, ch_t **pchain){

  ch_t *chain = NULL;

  assert(pe);

  chain = (ch_t *) calloc(1, sizeof(ch_t));
  assert(chain);
  if(chain == NULL) pe_fatal(pe, "calloc(ch_t) failed\n");

  ch_dim_set(chain, DIMX_DEFAULT);
  ch_N_set(chain, N_CHAIN_DEFAULT);
  ch_outfreq_set(chain, OUTFREQ_CHAIN_DEFAULT);
  ch_outdir_set(chain, OUTDIR_DEFAULT);

  *pchain = chain;

  return 0;
}

/*****************************************************************************
 *
 *  ch_free
 *
 *****************************************************************************/

int ch_free(ch_t *chain){

  assert(chain);

  mem_free((void**)&chain->samples);
  mem_free((void**)&chain->probability);
  mem_free((void**)&chain->ratio);
  mem_free((void**)&chain->accepted);
  mem_free((void**)&chain);

  return 0;
}

/*****************************************************************************
 *
 *  ch_init_burn_rt
 *
 *****************************************************************************/

int ch_init_burn_rt(rt_t *rt, ch_t *burn){

  int dim, N, outfreq;
  char outdir[FILENAME_MAX];

  assert(rt);
  assert(burn);

  if(rt_int_parameter(rt, "sample_dim", &dim))
  {
    ch_dim_set(burn, dim);
  }

  if(rt_int_parameter(rt, "burn_N", &N))
  {
    ch_N_set(burn, N);
  }

  if(rt_int_parameter(rt, "freq_burn", &outfreq))
  {
    ch_outfreq_set(burn, outfreq);
  }

  if(rt_string_parameter(rt, "outdir", outdir, FILENAME_MAX))
  {
    ch_outdir_set(burn, outdir);
  }

  ch_allocate_samples(burn);
  ch_allocate_probability(burn);
  ch_allocate_ratio(burn);
  ch_allocate_accepted(burn);

  return 0;
}

/*****************************************************************************
 *
 *  ch_init_chain_rt
 *
 *****************************************************************************/

int ch_init_chain_rt(rt_t *rt, ch_t *chain){

  int dim, N, outfreq;
  char outdir[FILENAME_MAX];

  assert(rt);
  assert(chain);

  if(rt_int_parameter(rt, "sample_dim", &dim))
  {
    ch_dim_set(chain, dim);
  }

  if(rt_int_parameter(rt, "postburn_N", &N))
  {
    ch_N_set(chain, N);
  }

  if(rt_int_parameter(rt, "freq_burn", &outfreq))
  {
    ch_outfreq_set(chain, outfreq);
  }

  if(rt_string_parameter(rt, "outdir", outdir, FILENAME_MAX))
  {
    ch_outdir_set(chain, outdir);
  }

  ch_allocate_samples(chain);
  ch_allocate_probability(chain);
  ch_allocate_ratio(chain);
  ch_allocate_accepted(chain);

  return 0;
}

/*****************************************************************************
 *
 *  ch_burn_info
 *
 *****************************************************************************/

int ch_burn_info(pe_t *pe, ch_t *burn){

  int dim, N, outfreq;
  char outdir[FILENAME_MAX];

  assert(pe);
  assert(burn);

  ch_dim(burn, &dim);
  ch_N(burn, &N);
  ch_outfreq(burn, &outfreq);
  ch_outdir(burn, outdir);

  pe_info(pe, "\n");
  pe_info(pe, "Burn Chain Properties\n");
  pe_info(pe, "---------------------\n");
  pe_info(pe, "%30s\t\t%d\n", "Length:", N);
  pe_info(pe, "%30s\t\t%d\n", "Dimensionality of Samples:", dim);
  pe_info(pe, "%30s\t\t%d %s\n", "Output Frequency:", outfreq, "iterations");
  pe_info(pe, "%30s\t\t%s\n", "Output directory:", outdir);

  return 0;
}

/*****************************************************************************
 *
 *  ch_chain_info
 *
 *****************************************************************************/

int ch_chain_info(pe_t *pe, ch_t *chain){

  int dim, N, outfreq;
  char outdir[FILENAME_MAX];

  assert(pe);
  assert(chain);

  ch_dim(chain, &dim);
  ch_N(chain, &N);
  ch_outfreq(chain, &outfreq);
  ch_outdir(chain, outdir);

  pe_info(pe, "\n");
  pe_info(pe, "Chain Properties\n");
  pe_info(pe, "---------------------\n");
  pe_info(pe, "%30s\t\t%d\n", "Lenght:", N);
  pe_info(pe, "%30s\t\t%d\n", "Dimensionality of Samples:", dim);
  pe_info(pe, "%30s\t\t%d %s\n", "Output Frequency:", outfreq, "iterations");
  pe_info(pe, "%30s\t\t%s\n", "Output directory:", outdir);

  return 0;
}

int ch_write_files(ch_t *chain, const char *chain_type){

  assert(chain);

  printf("%30s\t%50s", "Creating output directory:", chain->outdir);
  rw_create_dir(chain->outdir);
  printf("\tDone\n");

  util_write_array_precision(chain->samples, chain->N, chain->dim,
                             chain->outdir, chain_type, "samples");

  util_write_array_precision(chain->probability, chain->N, 1,
                             chain->outdir, chain_type, "probability");

  util_write_array_precision(chain->ratio, chain->N, 1,
                             chain->outdir, chain_type, "ratio");

  util_write_array_int(chain->accepted, chain->N, 1,
                       chain->outdir, chain_type, "accepted");

  return 0;
}

/*****************************************************************************
 *
 *  ch_dim_set
 *
 *****************************************************************************/

int ch_dim_set(ch_t *chain, int dim){

  assert(chain);

  chain->dim = dim;

  return 0;
}

/*****************************************************************************
 *
 *  ch_N_set
 *
 *****************************************************************************/

int ch_N_set(ch_t *chain, int N){

  assert(chain);

  chain->N = N;

  return 0;
}

/*****************************************************************************
 *
 *  ch_outfreq_set
 *
 *****************************************************************************/

int ch_outfreq_set(ch_t *chain, int outfreq){

  assert(chain);

  chain->outfreq = outfreq;

  return 0;
}

/*****************************************************************************
 *
 *  ch_outdir_set
 *
 *****************************************************************************/

int ch_outdir_set(ch_t *chain, const char *outdir){

  assert(chain);

  sprintf(chain->outdir, "%s", outdir);

  return 0;
}

/*****************************************************************************
 *
 *  ch_dim
 *
 *****************************************************************************/

int ch_dim(ch_t *chain, int *dim){

  assert(chain);

  *dim = chain->dim;

  return 0;
}

/*****************************************************************************
 *
 *  ch_N
 *
 *****************************************************************************/

int ch_N(ch_t *chain, int *N){

  assert(chain);

  *N = chain->N;

  return 0;
}

/*****************************************************************************
 *
 *  ch_outfreq
 *
 *****************************************************************************/

int ch_outfreq(ch_t *chain, int *outfreq){

  assert(chain);

  *outfreq = chain->outfreq;

  return 0;
}

/*****************************************************************************
 *
 *  ch_outdir
 *
 *****************************************************************************/

int ch_outdir(ch_t *chain, char *outdir){

  assert(chain);

  sprintf(outdir, "%s", chain->outdir);

  return 0;
}

/*****************************************************************************
 *
 *  ch_samples
 *
 *****************************************************************************/

int ch_samples(ch_t *chain, precision **psamples){

  assert(chain);

  *psamples = chain->samples;

  return 0;
}

/*****************************************************************************
 *
 *  ch_probability
 *
 *****************************************************************************/

int ch_probability(ch_t *chain, precision **pprobability){

  assert(chain);

  *pprobability = chain->probability;

  return 0;
}

/*****************************************************************************
 *
 *  ch_ratio
 *
 *****************************************************************************/

int ch_ratio(ch_t *chain, precision **pratio){

  assert(chain);

  *pratio = chain->ratio;

  return 0;
}

/*****************************************************************************
 *
 *  ch_accepted
 *
 *****************************************************************************/

int ch_accepted(ch_t *chain, int **paccepted){

  assert(chain);

  *paccepted = chain->accepted;

  return 0;
}

/*****************************************************************************
 *
 *  ch_allocate_samples
 *
 *****************************************************************************/

static int ch_allocate_samples(ch_t *chain){

  assert(chain);

  mem_malloc_precision(&chain->samples, chain->dim * (chain->N+1));

  return 0;
}

/*****************************************************************************
 *
 *  ch_allocate_probability
 *
 *****************************************************************************/

static int ch_allocate_probability(ch_t *chain){

  assert(chain);

  mem_malloc_precision(&chain->probability, chain->N+1);

  return 0;
}

/*****************************************************************************
 *
 *  ch_allocate_ratio
 *
 *****************************************************************************/

static int ch_allocate_ratio(ch_t *chain){

  assert(chain);

  mem_malloc_precision(&chain->ratio, chain->N+1);

  return 0;
}

/*****************************************************************************
 *
 *  ch_allocate_accepted
 *
 *****************************************************************************/

static int ch_allocate_accepted(ch_t *chain){

  assert(chain);

  mem_malloc_integers(&chain->accepted, chain->N+1);

  return 0;
}

/*****************************************************************************
 *
 *  ch_append_sample
 *
 *****************************************************************************/

int ch_append_sample(int idx, precision *sample, ch_t *chain){

  assert(chain);
  assert(sample);

  int i;
  int size = chain->dim;

  for(i=0; i<size; i++) chain->samples[idx*size+i] = sample[i];

  return 0;
}

/*****************************************************************************
 *
 *  ch_append_probability
 *
 *****************************************************************************/

int ch_append_probability(int idx, precision probability, ch_t *chain){

  assert(chain);

  chain->probability[idx] = probability;

  return 0;
}

/*****************************************************************************
 *
 *  ch_append_stats
 *
 *****************************************************************************/

int ch_append_stats(int idx, int accepted, ch_t *chain){

  assert(chain);

  chain->accepted[idx] = chain->accepted[idx-1] + accepted;
  chain->ratio[idx] = (precision)chain->accepted[idx] / (precision)idx;

  return 0;
}

/*****************************************************************************
 *
 *  ch_init_stats
 *
 *****************************************************************************/

int ch_init_stats(int idx, ch_t *chain){

  assert(chain);

  chain->probability[idx] = 0.0;
  chain->accepted[idx] = 0;
  chain->ratio[idx] = 0.0;

  return 0;
}
