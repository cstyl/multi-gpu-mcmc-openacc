#ifndef __COMMAND_LINE_PARSER_H__
#define __COMMAND_LINE_PARSER_H__

#include "definitions.h"

typedef struct cmd_s cmd_t;
typedef struct cmd_data_s cmd_data_t;

struct cmd_data_s
{
    int dim;
    int N;
    char datadir[BUFSIZ];
    char fx[BUFSIZ], fy[BUFSIZ];
};

struct cmd_s
{
  int dim;
  int Ns;
  int Nburn;
  int maxlag;
  precision rwsd;
  cmd_data_t train;
  cmd_data_t test;
  char dataset[BUFSIZ];
  char outdir[BUFSIZ];
};

int cmd_create(cmd_t **pcmd);
int cmd_free(cmd_t *cmd);
int cmd_parse(int an, char *av[], cmd_t *cmd);
int cmd_print_status(cmd_t *cmd);

#endif // __COMMAND_LINE_PARSER_H__
