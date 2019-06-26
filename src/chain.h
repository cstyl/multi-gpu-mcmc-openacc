#ifndef __CHAIN_H__
#define __CHAIN_H__

#include "definitions.h"
#include "command_line_parser.h"

typedef struct chain_s chain_t;

struct chain_s{
  cmd_t *cmd;
  precision *samples;
  precision *probability;
  precision *ratio;
  int *accepted;
};

int chain_create(cmd_t *cmd, chain_t **pchain);
int chain_free(chain_t *chain);

int chain_append_sample(int i, precision *sample, chain_t *chain);
int chain_append_stats(int i, precision probability, int accepted, chain_t *chain);

#endif // __CHAIN_H__
