#ifndef __MCMC_H__
#define __MCMC_H__
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
/* initialises the mcmc sampler with the first sample
 * evaluates likelihood and prior of the sample
 */
 void mcmc_init(metropolis *mcmc, data *data, rng *rng, int random);
 int mcmc_run(metropolis *mcmc, data *data, rng *rng);


#endif //__MCMC_H__
