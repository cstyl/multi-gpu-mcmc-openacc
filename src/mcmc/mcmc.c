#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mcmc.h"
#include "macros.h"
#include "flags.h"
#include "rng.h"

#include <gsl/gsl_randist.h>  // to obtain various pdf
#include <gsl/gsl_cblas.h>    // to perform dot products and vector operations

// large to simulate non-informative prior
#define PRIOR_SD 100

#ifndef DOT
#define DOT GSL_DOT
#endif

static void proposeSample(rng *r, precision *current, precision *proposed,
                          int dim, precision rwsd);
static precision evaluateSample(metropolis *mcmc, data *data);
static void chooseSample(metropolis *mcmc, rng *rng, int idx);

static precision sampleNormal(rng_i *r, precision sample, precision rwsd);
static void appendChain(int idx, int dim, precision *chain, precision *sample);

static precision logLhood(precision *sample, data *data);
static precision logPriorProb(precision *sample, int dim);

static void swapPtrs(void **ptr1, void **ptr2);

void mcmc_init(metropolis *mcmc, data *data, rng *rng, int random)
{
    int i;

    /* initialise to zero */
    for(i=0; i<mcmc->dim; i++)
      mcmc->current->values[i] = 0.0;

    /* If enabled, the sampler starts from a random position */
    if(random)
    {
      proposeSample(rng, mcmc->current->values, mcmc->current->values,
                    mcmc->dim, (double)(2.38/mcmc->dim));
    }

    appendChain(0, mcmc->dim, mcmc->chain, mcmc->current->values);

    mcmc->current->likelihood = logLhood(mcmc->current->values, data);
    mcmc->current->prior = logPriorProb(mcmc->current->values, mcmc->dim);
    mcmc->current->posterior = mcmc->current->prior + mcmc->current->likelihood;
}


int mcmc_run(metropolis *mcmc, data *data, rng *rng)
{
  int i=0;
  mcmc->acceptance_ratio = 0;

  for(i=0; i<(mcmc->Nburn+mcmc->Ns); i++)
  {
    proposeSample(rng, mcmc->current->values, mcmc->proposed->values,
                  mcmc->dim, mcmc->rwsd);

    mcmc->acceptance_prob = evaluateSample(mcmc, data);
    precision u = (precision) gsl_rng_uniform(rng->instances[mcmc->dim]);
    chooseSample(mcmc, rng, i);
    printf("sample accepted: %f\n", mcmc->acceptance_ratio);
    // printf("\ncurrent: %f %f %f ", mcmc->current->prior, mcmc->current->likelihood, mcmc->current->posterior);
    // printf("proposed: %f %f %f ", mcmc->proposed->prior, mcmc->proposed->likelihood, mcmc->proposed->posterior);
    // printf("accept: %f %f %f\n", mcmc->acceptance_prob, exp(mcmc->acceptance_prob),log(u));

    swapPtrs((void**)&mcmc->current, (void**)&mcmc->proposed);
  }
  mcmc->acceptance_ratio /= mcmc->Ns;
  printf("acceptance ratio: %f\n", mcmc->acceptance_ratio);
  return SIM_SUCCESS;
}

static void proposeSample(rng *r, precision *current, precision *proposed,
                          int dim, precision rwsd)
{
  int i;
  for(i=0; i<dim; i++)
    proposed[i] = sampleNormal(r->instances[i], current[i], (double)rwsd);
}

static precision evaluateSample(metropolis *mcmc, data *data)
{
  /* evaluate the posterior of the proposed samples */
  mcmc->proposed->prior = logPriorProb(mcmc->proposed->values, mcmc->dim);
  mcmc->proposed->likelihood = logLhood(mcmc->proposed->values, data);
  mcmc->proposed->posterior = mcmc->proposed->prior + mcmc->proposed->likelihood;
  /* Note that current posterior is already precomputed in the previous iteration */
  return mcmc->proposed->posterior - mcmc->current->posterior;
}

static void chooseSample(metropolis *mcmc, rng *rng, int idx)
{
  /* to stochastically accept/reject the proposed sample*/
  precision u = (precision) gsl_rng_uniform(rng->instances[mcmc->dim]);
  printf("%f < %f\n", log(u), mcmc->acceptance_prob);
  if(log(u) <= mcmc->acceptance_prob)
  {
    /* accept the proposal, add the proposed sample to the chain */
    appendChain(idx, mcmc->dim, mcmc->chain, mcmc->proposed->values);
    /* keeps track of the accepted samples */
    mcmc->acceptance_ratio += 1.0;
  }else{
    /* add the current sample to the chain */
    appendChain(idx, mcmc->dim, mcmc->chain, mcmc->current->values);
  }
}

static precision sampleNormal(rng_i *r, precision sample, precision rwsd)
{
  return sample + gsl_ran_gaussian_ziggurat(r, (double)rwsd);
}

static void appendChain(int idx, int dim, precision *chain, precision *sample)
{
  int i;
  for(i=0; i<dim; i++)
    chain[idx*dim + i] = sample[i];
}

static void swapPtrs(void **ptr1, void **ptr2)
{
  void *ptr_temp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = ptr_temp;
}

/* evaluates logistic regression likelihood in log-domain
 * L_n(theta) = 1 / (1 + exp(-y_n * theta^T * x_n)) where y_n={-1,1}
 * log(L_n(theta)) = - log(1 + exp(-y_n * theta^T * x_n)
 */
static precision logLhood(precision *sample, data *data)
{
  int i,j;
  precision lhood=0.0, dot;

  for(i=0; i<data->Nd; i++)
  {
#if DOT == NAIVE
    dot = 0.0;
    for(j=0; j<data->dim; j++)
    {
      dot += sample[j] * data->x[i*data->dim+j];
    }
#else
    dot = cblas_ddot(data->dim, &data->x[i*data->dim], 1, sample, 1);
#endif

    lhood -= log(1 + exp(-data->y[i] * dot));
  }

  return lhood;
}

/* Non-informative prior
 * assuming iid priors that follow normal distribution
 * with zero mean and std = PRIOR_SD (generaly large enough
 * to represent the non-informative nature of prior
 * and hence let the data speak for it self) */
static precision logPriorProb(precision *sample, int dim)
{
  precision priorProb = 0.0;
  int i;

  for(i=0; i<dim; i++){
    priorProb += log(gsl_ran_gaussian_pdf(sample[i], PRIOR_SD));
  }

  return priorProb;
}
