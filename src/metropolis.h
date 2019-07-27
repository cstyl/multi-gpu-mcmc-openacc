#ifndef __METROPOLIS_H__
#define __METROPOLIS_H__

#include "pe.h"
#include "runtime.h"
#include "chain.h"
#include "data_input.h"
#include "multivariate_normal.h"
#include "logistic_regression.h"
#include "sample.h"

typedef struct met_s met_t;

int met_create(pe_t *pe, ch_t *burn, ch_t *chain, met_t **pmet);
int met_free(met_t *met);

int met_init_rt(pe_t *pe, rt_t *rt, met_t *met);
int met_info_rt(pe_t *pe, met_t *met);

int met_init(pe_t *pe, met_t *met);
int met_run(pe_t *pe, met_t *met);
int met_random_init_set(met_t *met, int random_init);
int met_random_init(met_t *met, int *random_init);

int met_burn(met_t *met, ch_t **pburn);
int met_chain(met_t *met, ch_t **pchain);
int met_data(met_t *met, data_t **pdata);
int met_mvnb(met_t *met, mvnb_t **pmvnb);
int met_lr(met_t *met, lr_t **plr);
int met_current(met_t *met, sample_t **pcurrent);
int met_proposed(met_t *met, sample_t **pproposed);

#endif // __METROPOLIS_H__
