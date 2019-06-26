#ifndef __LOGISTIC_REGRESSION_H__
#define __LOGISTIC_REGRESSION_H__

#include "definitions.h"
#include "command_line_parser.h"
#include "data_input.h"

typedef struct lh_s lh_t;

int lr_lhood_create(int N, lh_t **plh);
int lr_lhood_free(lh_t *lh);

precision lr_lhood(lh_t *lr, precision *sample, data_t *data);
precision lr_lhood_dot(lh_t *lr, precision *sample, data_t *data);
precision lr_lhood_mv(lh_t *lr, precision *sample, data_t *data);

#endif // __LOGISTIC_REGRESSION_H__
