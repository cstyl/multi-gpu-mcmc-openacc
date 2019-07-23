#ifndef __MVN_H__
#define __MVN_H__

#include "definitions.h"
#include "pe.h"
#include "runtime.h"

typedef struct mvnb_s mvnb_t;

int mvn_block_create(pe_t *pe, mvnb_t **pmvnb);
int mvn_block_free(mvnb_t *mvnb);

int mvn_block_init_rt(rt_t *rt, mvnb_t *mvnb);
int mvn_block_init(mvnb_t *mvnb);
int mvn_block_init_covariance(mvnb_t *mvnb);
int mvn_block_cholesky_decomp(mvnb_t *mvnb);
int mvn_block_sample(mvnb_t *mvnb, precision *cur, precision *pro);

int mvn_block_dim_set(mvnb_t *mvnb, int dim);
int mvn_block_rwsd_set(mvnb_t *mvnb, precision rwsd);
int mvn_block_tune_set(mvnb_t *mvnb, int tune);

int mvn_block_dim(mvnb_t *mvnb, int *dim);
int mvn_block_rwsd(mvnb_t *mvnb, precision *rwsd);
int mvn_block_tune(mvnb_t *mvnb, int *tune);
int mvn_block_covariance(mvnb_t *mvnb, precision **covariance);
int mvn_block_L(mvnb_t *mvnb, precision **L);

#endif // __MVN_H__
