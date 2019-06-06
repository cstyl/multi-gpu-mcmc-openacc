#ifndef __UTIL_H__
#define __UTIL_H__

#include "structs.h"
#include "flags.h"

int parse_args(int an, char *av[], mcmc *mcmc);
void print_parameters(mcmc mcmc);

int allocate_data_vectors(data *train, data *test);
int allocate_mcmc_vectors(metropolis *metropolis);
int destroy_data_vectors(data *train, data *test);
int destroy_mcmc_vectors(metropolis *metropolis);
#endif //__UTIL_H__
