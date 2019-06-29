#ifndef __METROPOLIS_H__
#define __METROPOLIS_H__

#include "definitions.h"
#include "command_line_parser.h"
#include "random_number_generator.h"
#include "data_input.h"
#include "chain.h"

typedef struct met_s met_t;

int metropolis_create(cmd_t *cmd, rng_t *rng, data_t *data, met_t **pmet);
int metropolis_init(met_t *met, int random);
int metropolis_run(met_t *met);
int metropolis_free(met_t *met);
int metropolis_chain(met_t *met, chain_t **pchain);
int metropolis_write_chains(met_t *met);

#endif // __METROPOLIS_H__
