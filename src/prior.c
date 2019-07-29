#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "prior.h"
#include "timer.h"

// large to simulate non-informative prior
#define PRIOR_SD 100000
#define PI 3.14159265359

static precision pr_normal_prob(precision sample, precision sd);

precision pr_log_prob(precision *sample, int dim){

  assert(sample);

  int i;
  precision priorProb = 0.0;

  TIMER_start(TIMER_PRIOR);

  for(i=0; i<dim; i++)
  {
    priorProb += log(pr_normal_prob(sample[i], PRIOR_SD));
  }

  TIMER_stop(TIMER_PRIOR);

  return priorProb;
}


static precision pr_normal_prob(precision sample, precision sd){

  return exp(-(pow(sample,2.0)/(2*pow(sd, 2.0))))/sqrt(2*PI*pow(sd, 2.0));

}
