#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mcmc.h"

int main(int argc, char *argv[]){

  mcmc_t *mcmc;

  mcmc_create(&mcmc);
  mcmc_setup(argc, argv, mcmc);

  mcmc_sample(mcmc);
  mcmc_statistics(mcmc);
  mcmc_infer(mcmc);

  mcmc_disassemble(mcmc);
  mcmc_free(mcmc);

  return 0;
}
