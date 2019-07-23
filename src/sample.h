#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include "definitions.h"
#include "pe.h"
#include "runtime.h"
#include "chain.h"
#include "logistic_regression.h"
#include "multivariate_normal.h"

typedef struct sample_s sample_t;

int sample_create(pe_t *pe, sample_t **psample);
int sample_free(sample_t *sample);
int sample_init_rt(rt_t *rt, sample_t *sample);

int sample_dim_set(sample_t *sample, int dim);
int sample_prior_set(sample_t *sample, precision prior);
int sample_likelihood_set(sample_t *sample, precision likelihood);
int sample_posterior_set(sample_t *sample, precision posterior);

int sample_values(sample_t *sample, precision **pvalues);
int sample_dim(sample_t *sample, int *dim);
int sample_prior(sample_t *sample, precision *prior);
int sample_likelihood(sample_t *sample, precision *likelihood);
int sample_posterior(sample_t *sample, precision *posterior);

int sample_allocate_values(sample_t *sample);
int sample_init_zero(sample_t *sample);

int sample_propose_mvnb(mvnb_t *mvnb, sample_t *cur, sample_t *pro);
precision sample_evaluate_lr(lr_t *lr, sample_t *cur, sample_t *pro);
void sample_choose(int idx, ch_t *chain, sample_t **pcur, sample_t **ppro);

#endif // __SAMPLE_H
