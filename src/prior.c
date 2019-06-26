#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "prior.h"
#include "random_number_generator.h"

// large to simulate non-informative prior
#define PRIOR_SD 10

precision pr_log_prob(cmd_t *cmd, precision *sample){

  assert(cmd);

  int i;
  precision priorProb = 0.0;

  for(i=0; i<cmd->dim+1; i++)
  {
    priorProb += log(rng_normal_prob(&sample[i], PRIOR_SD));
  }

  return priorProb;
}
