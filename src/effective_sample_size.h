#ifndef __EFFECTIVE_SAMPLE_SIZE_H__
#define __EFFECTIVE_SAMPLE_SIZE_H__

#include "definitions.h"
#include "command_line_parser.h"
#include "metropolis.h"

typedef struct ess_s ess_t;

int ess_create(cmd_t *cmd, met_t *met, ess_t **pess);
int ess_free(ess_t *ess);
int ess_compute(ess_t *ess);
int ess_print(ess_t *ess);

#endif // __EFFECTIVE_SAMPLE_SIZE_H__
