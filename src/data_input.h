#ifndef __DATA_INPUT_H__
#define __DATA_INPUT_H__

#include "definitions.h"
#include "pe.h"
#include "runtime.h"

typedef struct data_s data_t;

int data_create_train(pe_t *pe, data_t **pdata);
int data_create_test(pe_t *pe, data_t **pdata);
int data_free(data_t *data);

void data_send_to_device(data_t *data);
void data_delete_from_device(data_t *data);

int data_init_train_rt(rt_t *rt, data_t *train);
int data_init_test_rt(rt_t *rt, data_t *test);
int data_input_train_info(pe_t *pe, data_t *train);
int data_input_test_info(pe_t *pe, data_t *test);

int data_dimx_set(data_t *data, int dimx);
int data_dimy_set(data_t *data, int dimy);
int data_N_set(data_t *data, int N);
int data_fx_set(data_t *data, const char *filename);
int data_fy_set(data_t *data, const char *filename);
int data_x_set(data_t *data, precision *x);
int data_y_set(data_t *data, int *y);

int data_dimx(data_t *data, int *dimx);
int data_dimy(data_t *data, int *dimy);
int data_N(data_t *data, int *N);
int data_fx(data_t *data, char *fx);
int data_fy(data_t *data, char *fy);
int data_x(data_t *data, precision **px);
int data_y(data_t *data, int **py);

int data_read_file(pe_t *pe, data_t *data);
int data_print_file(data_t *data);

#endif // __DATA_INPUT_H__
