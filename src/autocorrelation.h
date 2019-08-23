#ifndef __AUTOCORRELATION_H__
#define __AUTOCORRELATION_H__

#include "definitions.h"
#include "pe.h"
#include "chain.h"

typedef struct acr_s acr_t;

int acr_create(pe_t *pe, ch_t *chain, acr_t **pacr);
int acr_free(acr_t *acr);
int acr_init_rt(rt_t *rt, ch_t *chain, acr_t *acr);

int acr_compute(acr_t *acr);

int acr_info(pe_t *pe, acr_t *acr);
int acr_print_acr(pe_t *pe, acr_t *acr);
int acr_write_acr(acr_t *acr);

/* Functions to access elements in the structure */
int acr_dim_set(acr_t *acr, int dim);
int acr_dim(acr_t *acr, int *dim);
int acr_N_set(acr_t *acr, int N);
int acr_N(acr_t *acr, int *N);
int acr_maxlag_set(acr_t *acr, int maxlag);
int acr_maxlag(acr_t *acr, int *maxlag);
int acr_threshold_set(acr_t *acr, precision threshold);
int acr_threshold(acr_t *acr, precision *threshold);
int acr_outfreq_set(acr_t *acr, int outfreq);
int acr_outfreq(acr_t *acr, int *outfreq);
int acr_outdir_set(acr_t *acr, const char *outdir);
int acr_outdir(acr_t *acr, char *outdir);

int acr_X(acr_t *acr, precision **pX);
int acr_mean(acr_t *acr, precision **pmean);
int acr_variance(acr_t *acr, precision **pvariance);
int acr_offset(acr_t *acr, int **poffset);
int acr_maxlag_act(acr_t *acr, int **pmaxlag_act);
int acr_lagk(acr_t *acr, precision **plagk);

/* Mainly here for testing */
precision acr_compute_lagk(precision *X, precision mu, precision var,
                           int N, int lag, precision threshold);

#endif // __AUTOCORRELATION_H__
