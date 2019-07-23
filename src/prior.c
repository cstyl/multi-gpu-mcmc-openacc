#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "prior.h"

// large to simulate non-informative prior
#define PRIOR_SD 100000
// #define PRIOR_SD 10
#define PI 3.14159265359

precision pr_log_prob(precision *sample, int dim){

  assert(sample);

  int i;
  precision priorProb = 0.0;

  for(i=0; i<dim; i++)
  {
    // priorProb += log(rng_normal_prob(&sample[i], PRIOR_SD));
    priorProb += log(exp(-(pow(sample[i],2.0)/(2*pow(PRIOR_SD, 2.0))))/sqrt(2*PI*pow(PRIOR_SD, 2.0)));
  }

  return priorProb;
}
