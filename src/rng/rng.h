#ifndef __RNG_H__
#define __RNG_H__
#include <stdio.h>
#include <stdlib.h>
#include <gsl_rng.h>

typedef gsl_rng rng_i;
typedef gsl_rng_type rng_t;

typedef struct rng
{
  rng_i **instances;
  const rng_t *type;
  int count;
} rng;

int setup_rng(rng *rng);
int destroy_rng(rng *rng);

#endif // __RNG_H__
