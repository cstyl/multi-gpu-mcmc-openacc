#ifndef __LOGISTIC_REGRESSION_H__
#define __LOGISTIC_REGRESSION_H__

#include "definitions.h"
#include "pe.h"
#include "data_input.h"

typedef struct lr_s lr_t;

int lr_lhood_create(pe_t *pe, data_t *data, lr_t **plr);
int lr_lhood_free(lr_t *lr);
precision lr_lhood(lr_t *lr, precision *sample);

int lr_dim(lr_t *lr, int *dim);
int lr_N(lr_t *lr, int *N);
int lr_data(lr_t *lr, data_t **pdata);
int lr_dot(lr_t *lr, precision **pdot);
#endif // __LOGISTIC_REGRESSION_H__
