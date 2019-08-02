#ifndef __INFERENCE_H__
#define __INFERENCE_H__

typedef struct infr_s infr_t;

#include "pe.h"
#include "runtime.h"
#include "chain.h"

int infr_create(pe_t *pe, ch_t *chain, infr_t **pinfr);
int infr_free(infr_t *infr);
int infr_init_rt(pe_t *pe, rt_t *rt, infr_t *infr);
int infr_init(pe_t *pe, infr_t *infr);
int infr_info(pe_t *pe, infr_t *infr);
int infr_print(pe_t *pe, infr_t *infr);

int infr_mc_integration_lr(infr_t *infr);

int infr_mc_case_set(infr_t *infr, const char *mc_case);
int infr_mc_case(infr_t *infr, char *mc_case);

#endif // __INFERENCE_H__
