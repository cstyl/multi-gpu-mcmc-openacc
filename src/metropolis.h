#ifndef __METROPOLIS_H__
#define __METROPOLIS_H__

#include "pe.h"
#include "runtime.h"
#include "chain.h"

typedef struct met_s met_t;

int met_create(pe_t *pe, ch_t *burn, ch_t *chain, met_t **pmet);
int met_free(met_t *met);

int met_init_rt(pe_t *pe, rt_t *rt, met_t *met);
int met_info_rt(pe_t *pe, met_t *met);

int met_init(pe_t *pe, met_t *met);
int met_run(pe_t *pe, met_t *met);
int met_random_init_set(met_t *met, int random_init);
int met_random_init(met_t *met, int *random_init);

#endif // __METROPOLIS_H__
