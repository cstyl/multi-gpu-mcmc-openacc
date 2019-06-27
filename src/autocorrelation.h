#ifndef __AUTOCORRELATION_H__
#define __AUTOCORRELATION_H__

#include "definitions.h"
#include "command_line_parser.h"
#include "metropolis.h"

typedef struct acr_s acr_t;

int acr_create(cmd_t *cmd, met_t *met, acr_t **pacr);
int acr_free(acr_t *acr);

int acr_compute_acr(acr_t *acr);
int acr_print_acr(acr_t *acr);

#endif // __AUTOCORRELATION_H__
