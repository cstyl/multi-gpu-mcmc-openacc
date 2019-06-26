#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include "definitions.h"
#include "command_line_parser.h"
#include "random_number_generator.h"
#include "data_input.h"
#include "chain.h"

typedef struct sample_s sample_t;

int sample_propose(cmd_t *cmd, rng_t *rng, sample_t *cur, sample_t *pro);
precision sample_evaluate(cmd_t *cmd, rng_t *rng, data_t *data,
                          sample_t *cur, sample_t *pro);
void sample_choose(int idx, cmd_t *cmd, rng_t *rng,
                   chain_t *chain, sample_t **pcur, sample_t **ppro);

int sample_init(cmd_t *cmd, rng_t *rng, data_t *data,
               chain_t *chain, sample_t *cur, int random);
int sample_create(cmd_t *cmd, sample_t **psample);
int sample_free(sample_t *sample);

#endif // __SAMPLE_H
