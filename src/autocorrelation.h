#ifndef __AUTOCORRELATION_H__
#define __AUTOCORRELATION_H__

#include "definitions.h"
#include "command_line_parser.h"
#include "metropolis.h"

typedef struct acr_s acr_t;
typedef struct acr_1d_s acr_1d_t;

struct acr_s{
  cmd_t *cmd;
  chain_t *chain;
  acr_1d_t **acr_1d;
};

struct acr_1d_s{
  int N;
  int maxlag;
  precision mean;
  precision variance;
  precision *X;
  precision *acr_lagk;
  precision ess;
};

int acr_create(cmd_t *cmd, met_t *met, acr_t **pacr);
int acr_free(acr_t *acr);

int acr_compute_acr(acr_t *acr);
int acr_print_acr(acr_t *acr);
int acr_write_acr(acr_t *acr);

#endif // __AUTOCORRELATION_H__
