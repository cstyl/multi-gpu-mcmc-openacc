#ifndef __RNG_H__
#define __RNG_H__

#include "definitions.h"
#include "command_line_parser.h"

typedef struct rng_s rng_t;

int rng_create(cmd_t *cmd, rng_t **prng);
int rng_setup(rng_t *rng);
int rng_free(rng_t *rng);

precision rng_normal(rng_t *rng, int i, precision rwsd);
precision rng_uniform_prob(rng_t *rng, int i);
precision rng_normal_prob(precision *sample, int sd);

#endif // __RNG_H__
