#ifndef __MCMC_H__
#define __MCMC_H__

#include "definitions.h"
typedef struct mcmc_s mcmc_t;

int mcmc_create(mcmc_t **pmcmc);
int mcmc_free(mcmc_t *mcmc);

int mcmc_setup(int an, char *av[], mcmc_t *mcmc);
int mcmc_disassemble(mcmc_t *mcmc);

int mcmc_sample(mcmc_t *mcmc);
int mcmc_statistics(mcmc_t *mcmc);
int mcmc_infer(mcmc_t *mcmc);

#endif // __MCMC_H__
