#ifndef __DECOMPOSITION_H__
#define __DECOMPOSITION_H__

#include "pe.h"
#include "runtime.h"

typedef struct dc_s dc_t;

int dc_create(pe_t *pe, dc_t **pdc);
int dc_free(dc_t *dc);

int dc_init_rt(pe_t *pe, rt_t *rt, dc_t *dc);
int dc_print_info(pe_t *pe, dc_t *dc);

int dc_decompose(int N, int dimx, int dimy, dc_t *dc);
int dc_check_inputs(pe_t *pe, int nprocs, int nthreads, int ngpus);

/* Interface */
int dc_nprocs_set(dc_t *dc, int nprocs);
int dc_nprocs(dc_t *dc, int *nprocs);
int dc_nthreads_set(dc_t *dc, int nthreads);
int dc_nthreads(dc_t *dc, int *nthreads);
int dc_ngpus_set(dc_t *dc, int ngpus);
int dc_ngpus(dc_t *dc, int *ngpus);
int dc_pxb_set(dc_t *dc, int pxlow, int pxhi);
int dc_pxb(dc_t *dc, int **ppxlow, int **ppxhi);
int dc_pyb_set(dc_t *dc, int pylow, int pyhi);
int dc_pyb(dc_t *dc, int **ppylow, int **ppyhi);
int dc_txb(dc_t *dc, int **ptxlow, int **ptxhi);
int dc_tyb(dc_t *dc, int **ptylow, int **ptyhi);
int dc_rank_set(dc_t *dc, int rank);
int dc_size_set(dc_t *dc, int size);

#endif // __DECOMPOSITION_H__
