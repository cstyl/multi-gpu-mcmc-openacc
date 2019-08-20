#ifndef __DECOMPOSITION_H__
#define __DECOMPOSITION_H__

#include "pe.h"
#include "runtime.h"

typedef struct dc_s dc_t;

int dc_create(pe_t *pe, dc_t **pdc);
int dc_free(dc_t *dc);

int dc_init_rt(pe_t *pe, rt_t *rt, dc_t *dc);
int dc_print_info(pe_t *pe, dc_t *dc);

int dc_decompose(dc_t *dc);

/* Interface */
int dc_nprocs_set(dc_t *dc, int nprocs);
int dc_nprocs(dc_t *dc, int *nprocs);
int dc_nthreads_set(dc_t *dc, int nthreads);
int dc_nthreads(dc_t *dc, int *nthreads);
int dc_pbound_set(dc_t *dc, int plow, int phi);
int dc_pbound(dc_t *dc, int *plow, int *phi);
int dc_tbound(dc_t *dc, int **ptlow, int **pthi);
int dc_rank_set(dc_t *dc, int rank);
int dc_size_set(dc_t *dc, int size);
int dc_work_set(dc_t *dc, int work);

#endif // __DECOMPOSITION_H__
