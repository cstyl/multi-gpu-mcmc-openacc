#ifndef __EFFECTIVE_SAMPLE_SIZE_H__
#define __EFFECTIVE_SAMPLE_SIZE_H__

#include "definitions.h"
#include "pe.h"
#include "runtime.h"
#include "autocorrelation.h"

typedef struct ess_s ess_t;

int ess_create(pe_t *pe, acr_t *acr, ess_t **pess);
int ess_free(ess_t *ess);
int ess_init_rt(rt_t *rt, ess_t *ess);
int ess_compute(ess_t *ess);
int ess_print_ess(pe_t *pe, ess_t *ess);
int ess_info(pe_t *pe, ess_t *ess);

int ess_dim_set(ess_t *ess, int dim);
int ess_case_set(ess_t *ess, const char *ess_case);

#endif // __EFFECTIVE_SAMPLE_SIZE_H__
