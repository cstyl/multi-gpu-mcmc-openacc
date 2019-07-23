#ifndef __CHAIN_H__
#define __CHAIN_H__

#include "definitions.h"
#include "pe.h"
#include "runtime.h"

typedef struct ch_s ch_t;

int ch_create(pe_t *pe, ch_t **pchain);
int ch_free(ch_t *chain);

int ch_init_burn_rt(rt_t *rt, ch_t *burn);
int ch_init_chain_rt(rt_t *rt, ch_t *chain);
int ch_burn_info(pe_t *pe, ch_t *burn);
int ch_chain_info(pe_t *pe, ch_t *chain);

int ch_dim_set(ch_t *chain, int dim);
int ch_N_set(ch_t *chain, int N);
int ch_outfreq_set(ch_t *chain, int outfreq);
int ch_outdir_set(ch_t *chain, const char *outdir);

int ch_dim(ch_t *chain, int *dim);
int ch_N(ch_t *chain, int *N);
int ch_outfreq(ch_t *chain, int *outfreq);
int ch_outdir(ch_t *chain, char *outdir);
int ch_probability(ch_t *chain, precision **pprobability);
int ch_ratio(ch_t *chain, precision **pratio);
int ch_accepted(ch_t *chain, int **paccepted);

int ch_allocate_samples(ch_t *chain);
int ch_allocate_probability(ch_t *chain);
int ch_allocate_ratio(ch_t *chain);
int ch_allocate_accepted(ch_t *chain);

int ch_append_probability(int idx, precision probability, ch_t *chain);
int ch_append_sample(int idx, precision *sample, ch_t *chain);
int ch_append_stats(int idx, int accepted, ch_t *chain);
int ch_init_stats(int idx, ch_t *chain);

#endif // __CHAIN_H__
