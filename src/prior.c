#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "prior.h"
#include "random_number_generator.h"
#include "timer.h"

// large to simulate non-informative prior
#define PRIOR_SD 1000000

precision pr_log_prob(cmd_t *cmd, precision *sample){

  assert(cmd);

  TIMER_start(TIMER_PRIOR);

  int i;
  precision priorProb = 0.0;

  for(i=0; i<cmd->dim+1; i++)
  {
    priorProb += log(rng_normal_prob(&sample[i], PRIOR_SD));
  }

  TIMER_stop(TIMER_PRIOR);

  return priorProb;
}
