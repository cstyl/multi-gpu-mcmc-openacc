#ifndef __DATA_INPUT_H__
#define __DATA_INPUT_H__

#include "definitions.h"
#include "command_line_parser.h"

typedef struct data_s data_t;

struct data_s{
  cmd_data_t *params;
  precision *x;
  int *y;
};

int data_create(cmd_data_t *cmd_data, data_t **pdata);
int data_free(data_t *data);
int data_read_file(data_t *data);
int data_print_file(data_t *data);

#endif // __DATA_INPUT_H__
