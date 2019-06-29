#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "inference.h"
#include "logistic_regression.h"
#include "memory.h"

struct infr_s{
  cmd_t *cmd;
  chain_t *chain;
  data_t *test;
  precision *sum;
  int *labels;
  precision accuracy;
};

static int infr_print_lr(infr_t *infr);
static int infr_write_lr(infr_t *infr);

/*****************************************************************************
 *
 * infr_create
 *
 *****************************************************************************/

int infr_create(cmd_t *cmd, chain_t *chain, data_t *data, infr_t **pinfr){

  infr_t *infr = NULL;

  assert(cmd);
  assert(chain);
  assert(data);

  infr = (infr_t *) calloc(1, sizeof(infr_t));
  assert(infr);
  if(infr == NULL)
  {
    printf("calloc(infr_t) failed\n");
    exit(1);
  }

  infr->cmd = cmd;
  infr->chain = chain;
  infr->test = data;

  mem_malloc_precision(&infr->sum, infr->cmd->test.N);
  mem_malloc_integers(&infr->labels, infr->cmd->test.N);

  *pinfr = infr;

  return 0;
}

/*****************************************************************************
 *
 * infr_free
 *
 *****************************************************************************/

int infr_free(infr_t *infr){

  assert(infr);

  free(infr->sum);
  free(infr->labels);

  free(infr);

  assert(infr->sum != NULL);
  assert(infr->labels != NULL);
  assert(infr != NULL);

  return 0;
}

/*****************************************************************************
 *
 * infr_monte_carlo_integration
 *
 *****************************************************************************/

int infr_mc_integration_lr(infr_t *infr){

  cmd_t *cmd = NULL;
  precision *sample = NULL;
  precision *x = NULL;
  assert(infr);

  cmd = infr->cmd;

  int i,j, acc_sum=0;
  for(i=0; i<infr->test->params->N; i++)
  {
    x = &infr->test->x[i*(cmd->dim+1)];
    infr->sum[i] = 0.0;
    for(j=1; j<=cmd->Ns; j++)
    {
      sample = &infr->chain->samples[j*(cmd->dim+1)];
      infr->sum[i] += lr_logistic_regression(sample, x, cmd->dim+1) / cmd->Ns;
    }

    infr->labels[i] = (infr->sum[i]>=0.5) ? 1 : -1;
    acc_sum += (infr->labels[i] == infr->test->y[i]) ? 1 : 0;
  }

  infr->accuracy = (precision)acc_sum / (precision)infr->test->params->N;

  return 0;
}

/*****************************************************************************
 *
 * infr_print
 *
 *****************************************************************************/

int infr_print(infr_t *infr, int func){

  assert(infr);

  if(func == LOGISTIC_REGRESSION) infr_print_lr(infr);

  return 0;
}

/*****************************************************************************
 *
 * infr_print_lr
 *
 *****************************************************************************/

static int infr_print_lr(infr_t *infr){

  assert(infr);

  printf("Accuracy = %f\n", infr->accuracy * 100);

  infr_write_lr(infr);

  return 0;
}

/*****************************************************************************
 *
 * infr_write_lr
 *
 *****************************************************************************/

static int infr_write_lr(infr_t *infr){

  assert(infr);

  char filename[BUFSIZ], header[BUFSIZ];
  FILE *fp = NULL;

  // sprintf(filename, "%s/infer_bin_labels.txt", cmd->test->params->datadir);
  sprintf(filename, "%s/infer_bin_labels.txt", "./out");
  sprintf(header, "#%19s\t%20s\t%20s\t%20s\n",
          "Datapoint", "Probability", "Label", "Actual Label"
         );

  fp = fopen(filename, "w+");
  assert(fp);

  /* write header */
  fprintf(fp, "%s\n", header);

  int i;
  for(i=0; i<infr->test->params->N; i++)
  {
    fprintf(fp, "\t%20d\t%20.16f\t%20d\t%20d\n",
            i+1, infr->sum[i], infr->labels[i], infr->test->y[i]
           );
  }

  fclose(fp);

  return 0;
}
