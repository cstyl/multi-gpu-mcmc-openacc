#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mcmc.h"
#include "timer.h"

int main(int argc, char *argv[]){

  mcmc_t *mcmc;

  TIMER_init();
  TIMER_start(TIMER_TOTAL);

  mcmc_create(&mcmc);
  mcmc_setup(argc, argv, mcmc);

  mcmc_sample(mcmc);

  mcmc_statistics(mcmc);

  mcmc_infer(mcmc);

  mcmc_disassemble(mcmc);
  mcmc_free(mcmc);

  TIMER_stop(TIMER_TOTAL);
  TIMER_statistics();

  return 0;
}
