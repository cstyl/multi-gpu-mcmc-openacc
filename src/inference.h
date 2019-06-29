#ifndef __INFERENCE_H__
#define __INFERENCE_H__

typedef struct infr_s infr_t;

enum monte_carlo_function{
  MEAN = 0,
  VARIANCE = 1,
  LOGISTIC_REGRESSION = 2,
  SOFTMAX = 3
};

#include "definitions.h"
#include "chain.h"
#include "data_input.h"

int infr_create(cmd_t *cmd, chain_t *chain, data_t *data, infr_t **pinfr);
int infr_free(infr_t *infr);

int infr_mc_integration_lr(infr_t *infr);
int infr_print(infr_t *infr, int func);

#endif // __INFERENCE_H__
