#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sample.h"
#include "prior.h"
#include "memory.h"
#include "ran.h"
#include "timer.h"
#include "util.h"

#define VERBOSE 1

struct sample_s{
  pe_t *pe;
  precision *values;
  precision prior;
  precision likelihood;
  precision posterior;
  int dim;
  int rank;
  int nprocs;
};

static int sample_allocate_values(sample_t *sample);
static int sample_print_progress_screen(pe_t *pe, int idx, ch_t *chain);
static int sample_save_progress(char *outdir, int dec, int idx, sample_t *cur, sample_t *pro);

void sample_create_device_values(sample_t *sample);
void sample_update_device_values(sample_t *sample);
void sample_free_device_values(sample_t *sample);

/*****************************************************************************
 *
 *  sample_create
 *
 *****************************************************************************/

int sample_create(pe_t *pe, sample_t **psample){

  sample_t *sample = NULL;

  assert(pe);

  sample = (sample_t *) calloc(1, sizeof(sample_t));
  assert(sample);
  if(sample == NULL) pe_fatal(pe, "calloc(sample_t) failed\n");

  sample->pe = pe;

  sample_dim_set(sample, DIMX_DEFAULT);
  sample_nprocs_set(sample, DEFAULT_PROCS);

  *psample = sample;

  return 0;
}

/*****************************************************************************
 *
 *  sample_free
 *
 *****************************************************************************/

int sample_free(sample_t *sample){

  assert(sample);

  sample_free_device_values(sample);
  mem_free((void**)&sample->values);

  mem_free((void**)&sample);

  return 0;
}

/*****************************************************************************
 *
 *  sample_init_rt
 *
 *****************************************************************************/

int sample_init_rt(rt_t *rt, sample_t *sample){

  int dim, nthreads, nprocs;

  assert(rt);
  assert(sample);

  if(rt_int_parameter(rt, "sample_dim", &dim))
  {
    sample_dim_set(sample, dim);
  }

  if(rt_int_parameter(rt, "nprocs", &nprocs))
  {
    sample_nprocs_set(sample, nprocs);
  }

  sample->rank = pe_mpi_rank(sample->pe);

  sample_allocate_values(sample);

  return 0;
}

/*****************************************************************************
 *
 *  sample_propose_mvnb
 *
 *****************************************************************************/

int sample_propose_mvnb(mvnb_t *mvnb, sample_t *cur, sample_t *pro){

  precision *current = NULL;
  precision *proposed = NULL;

  assert(mvnb);
  assert(cur);
  assert(pro);

  TIMER_start(TIMER_PROPOSAL);

  sample_values(cur, &current);
  sample_values(pro , &proposed);

  mvn_block_sample(mvnb, current, proposed);

  sample_update_device_values(pro);

  TIMER_stop(TIMER_PROPOSAL);
  return 0;
}

/*****************************************************************************
 *
 *  sample_evaluate_lr
 *
 *****************************************************************************/

precision sample_evaluate_lr(lr_t *lr, sample_t *cur, sample_t *pro){

  precision lhood=0.0, prior=0.0, posterior=0.0;
  precision cposterior;
  int dim;
  double ratio;
  precision *values = NULL;

  assert(lr);
  assert(cur);
  assert(pro);

  TIMER_start(TIMER_EVALUATION);

  sample_posterior(cur, &cposterior);

  sample_values(pro, &values);
  sample_dim(pro, &dim);

  lhood = lr_lhood(lr, values);
  prior = pr_log_prob(values, dim);
  posterior = prior + lhood;

  sample_prior_set(pro, prior);
  sample_likelihood_set(pro, lhood);
  sample_posterior_set(pro, posterior);

  ratio = exp(posterior - cposterior);

  TIMER_stop(TIMER_EVALUATION);

  return (ratio>1) ? 1: ratio;
}

/*****************************************************************************
 *
 *  sample_choose
 *
 *****************************************************************************/

void sample_choose(int idx, ch_t *chain, sample_t **pcur, sample_t **ppro){

  precision u;
  int accepted = 0;
  sample_t *cur = NULL;
  sample_t *pro = NULL;
  precision *probability = NULL;
  int outfreq;

  assert(chain);
  assert(pcur);
  assert(ppro);

  TIMER_start(TIMER_ACCEPTANCE);

  pro = *ppro;
  cur = *pcur;
  ch_probability(chain, &probability);
  ch_outfreq(chain, &outfreq);

  /* to stochastically accept/reject the proposed sample*/
  u = (precision)ran_serial_uniform();

  if(u <= probability[idx])
  {
    /* accept the proposal, add the proposed sample to the chain */
    ch_append_sample(idx, pro->values, chain);
    accepted = 1;
    /* swap pointers of two samples instead of copying the contents of each other */
    mem_swap_ptrs((void**)&cur, (void**)&pro);
  }else{
    /* add the current sample to the chain */
    ch_append_sample(idx, cur->values, chain);
  }

  ch_append_stats(idx, accepted, chain);

  if(outfreq!=0)
  {
    if(((idx==1) || idx%outfreq==0))
    {
      sample_print_progress_screen(pro->pe, idx, chain);
      if(VERBOSE)
      {
        char outdir[FILENAME_MAX];
        ch_outdir(chain, outdir);
        if(pro->rank==0) sample_save_progress(outdir, accepted, idx, cur, pro);
      }
    }
  }

  TIMER_stop(TIMER_ACCEPTANCE);

  *ppro = pro;
  *pcur = cur;
}

/*****************************************************************************
 *
 *  sample_init_zero
 *
 *****************************************************************************/

int sample_init_zero(sample_t *sample){

  int i;
  assert(sample);

  for(i=0; i<sample->dim; i++)
    sample->values[i] = 0.0;

  sample_update_device_values(sample);

  sample->prior = 0.0;
  sample->likelihood = 0.0;
  sample->posterior = 0.0;

  return 0;
}

/*****************************************************************************
 *
 *  sample_create_device_values
 *
 *****************************************************************************/

void sample_create_device_values(sample_t *sample){

  TIMER_start(TIMER_CREATE_VALUES);

  precision *values = sample->values;
  int dim = sample->dim;

  int gpuid = sample->rank%sample->nprocs;
  #pragma acc set device_num(gpuid) device_type(acc_device_nvidia)
  #pragma acc enter data create(values[:dim])

  TIMER_stop(TIMER_CREATE_VALUES);
}

/*****************************************************************************
 *
 *  sample_free_device_values
 *
 *****************************************************************************/

void sample_free_device_values(sample_t *sample){

  precision *values = sample->values;

  int gpuid = sample->rank%sample->nprocs;
  #pragma acc set device_num(gpuid) device_type(acc_device_nvidia)
  #pragma acc exit data delete(values)

}

/*****************************************************************************
 *
 *  sample_update_device_values
 *
 *****************************************************************************/

void sample_update_device_values(sample_t *sample){

  TIMER_start(TIMER_UPDATE_VALUES);

  precision *values = sample->values;
  int dim = sample->dim;

  int gpuid = sample->rank%sample->nprocs;
  #pragma acc set device_num(gpuid) device_type(acc_device_nvidia)
  #pragma acc update device(values[:dim])

  TIMER_stop(TIMER_UPDATE_VALUES);
}

/*****************************************************************************
 *
 *  sample_dim_set
 *
 *****************************************************************************/

int sample_dim_set(sample_t *sample, int dim){

  assert(sample);

  sample->dim = dim;

  return 0;
}

/*****************************************************************************
 *
 *  sample_prior_set
 *
 *****************************************************************************/

int sample_prior_set(sample_t *sample, precision prior){

  assert(sample);

  sample->prior = prior;

  return 0;
}

/*****************************************************************************
 *
 *  sample_likelihood_set
 *
 *****************************************************************************/

int sample_likelihood_set(sample_t *sample, precision likelihood){

  assert(sample);

  sample->likelihood = likelihood;

  return 0;
}

/*****************************************************************************
 *
 *  sample_posterior_set
 *
 *****************************************************************************/

int sample_posterior_set(sample_t *sample, precision posterior){

  assert(sample);

  sample->posterior = posterior;

  return 0;
}

/*****************************************************************************
 *
 *  sample_nprocs_set
 *
 *****************************************************************************/

int sample_nprocs_set(sample_t *sample, int nprocs){

  assert(sample);

  sample->nprocs = nprocs;

  return 0;
}

/*****************************************************************************
 *
 *  sample_copy_values
 *
 *****************************************************************************/

int sample_copy_values(sample_t *sample, precision *values){

  assert(sample);
  assert(values);

  int i;

  for(i=0; i<sample->dim; i++) sample->values[i] = values[i];

  return 0;
}

/*****************************************************************************
 *
 *  sample_values
 *
 *****************************************************************************/

int sample_values(sample_t *sample, precision **pvalues){

  assert(sample);

  *pvalues = sample->values;

  return 0;
}

/*****************************************************************************
 *
 *  sample_dim
 *
 *****************************************************************************/

int sample_dim(sample_t *sample, int *dim){

  assert(sample);

  *dim = sample->dim;

  return 0;
}

/*****************************************************************************
 *
 *  sample_prior
 *
 *****************************************************************************/

int sample_prior(sample_t *sample, precision *prior){

  assert(sample);

  *prior = sample->prior;

  return 0;
}

/*****************************************************************************
 *
 *  sample_likelihood
 *
 *****************************************************************************/

int sample_likelihood(sample_t *sample, precision *likelihood){

  assert(sample);

  *likelihood = sample->likelihood;

  return 0;
}

/*****************************************************************************
 *
 *  sample_posterior
 *
 *****************************************************************************/

int sample_posterior(sample_t *sample, precision *posterior){

  assert(sample);

  *posterior = sample->posterior;

  return 0;
}

/*****************************************************************************
 *
 *  sample_allocate_values
 *
 *****************************************************************************/

static int sample_allocate_values(sample_t *sample){

  assert(sample);

  mem_malloc_precision(&sample->values, sample->dim);
  sample_create_device_values(sample);

  return 0;
}

/*****************************************************************************
 *
 *  sample_print_progress
 *
 *****************************************************************************/

static int sample_print_progress_screen(pe_t *pe, int idx, ch_t *chain){

  precision *ratio = NULL;
  int *accepted = NULL;

  assert(chain);

  ch_ratio(chain, &ratio);
  ch_accepted(chain, &accepted);

  pe_info(pe, "Iteration %6d:\t", idx);
  pe_info(pe, "%20s%6d\t", "Accepted Samples = ", accepted[idx]);
  pe_info(pe, "%20s%4.3f%s\n", "Acceptance Ratio = ",ratio[idx]*100, "(%)");

  return 0;
}

/*****************************************************************************
 *
 *  sample_save_progress
 *
 *****************************************************************************/

static int sample_save_progress(char *outdir, int dec, int idx, sample_t *cur, sample_t *pro){

  assert(cur);
  assert(pro);

  FILE *fp;
  char filename[BUFSIZ];

  rw_create_dir(outdir);
  sprintf(filename, "%s/%s", outdir, "progress.csv");

  if(idx==1)
  {
    fp = fopen(filename, "w");
    assert(fp);
    fprintf(fp, "#Iter,cPrior,cLhood,cPost,pPrior,pLhood,pPost\n");
  }else{
    fp = fopen(filename, "a");
    assert(fp);
  }

  fprintf(fp, "%6d,", idx);
  if(dec==1)
  {
    fprintf(fp, "%.*e,%.*e,%.*e,%.*e,%.*e,%.*e\n",
                PRINT_PREC-1, pro->prior, PRINT_PREC-1, pro->likelihood,
                PRINT_PREC-1, pro->posterior, PRINT_PREC-1, cur->prior,
                PRINT_PREC-1, cur->likelihood, PRINT_PREC-1, cur->posterior);
  }else{
    fprintf(fp, "%.*e,%.*e,%.*e,%.*e,%.*e,%.*e\n",
                PRINT_PREC-1, cur->prior, PRINT_PREC-1, cur->likelihood,
                PRINT_PREC-1, cur->posterior, PRINT_PREC-1, pro->prior,
                PRINT_PREC-1, pro->likelihood, PRINT_PREC-1, pro->posterior);
  }

  fclose(fp);

  return 0;
}
