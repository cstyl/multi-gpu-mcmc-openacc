#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "data_input.h"
#include "memory.h"

#define SKIP_HEADER 1
#define Y_DIM 1

#define CONVERT(in, type_t, out, pos)\
({\
  if(!strcmp(type_t,"int"))\
  {\
    *((int *)out + pos) = atoi(in);\
  }else if(!strcmp(type_t,"precision"))\
  {\
    *((precision *)out + pos) = (precision)atof(in);\
  }else{\
    fprintf(stderr, "ERROR::%s:%d:%s: Invalid type argument \"%s\"!\n",\
            __FILE__, __LINE__,__func__, type_t);\
    exit(1);\
  }\
})

static int data_csvread( char *filename, int rowSz, int colSz, int skip_header,
                    const char *delimiter, const char *datatype, void *data);
static void data_rmheader(char* line, FILE *fp, int skip_num);

/*****************************************************************************
 *
 *  data_create
 *
 *****************************************************************************/

int data_create(cmd_data_t *cmd_data, data_t **pdata){

  data_t *data = NULL;

  assert(cmd_data);

  data = (data_t *) calloc(1, sizeof(data_t));
  assert(data);
  if(data == NULL)
  {
    printf("calloc(data_t) failed\n");
    exit(1);
  }

  data->params = cmd_data;

  mem_malloc_precision(&data->x, (data->params->dim+1) * data->params->N);
  mem_malloc_integers(&data->y, Y_DIM * data->params->N);

  *pdata = data;

  return 0;
}

/*****************************************************************************
 *
 *  data_free
 *
 *****************************************************************************/

int data_free(data_t *data){

  assert(data);

  if(data->x) free(data->x);
  if(data->y) free(data->y);

  free(data);

  assert(data->x != NULL);
  assert(data->y != NULL);
  assert(data != NULL);

  return 0;
}

/*****************************************************************************
 *
 *  data_read_file
 *
 *****************************************************************************/

int data_read_file(data_t *data){

  cmd_data_t *params = NULL;
  assert(data);
  assert(data->x);
  assert(data->y);

  params = data->params;
  data_csvread(params->fx, params->dim+1, params->N, SKIP_HEADER, ",", "precision", data->x);
  data_csvread(params->fy, Y_DIM, params->N, SKIP_HEADER, ",", "int", data->y);

  return 0;
}

/*****************************************************************************
 *
 *  data_print_file
 *
 *****************************************************************************/

int data_print_file(data_t *data){

  int i, j;
  assert(data);

  printf("Printing %s:\n", data->params->fx);
  for(i=0; i<data->params->N; i++)
  {
    for(j=0; j<data->params->dim; j++)
    {
      printf("\t%f", data->x[i*(data->params->dim+1) + j]);
    }
    printf("\t%f\n", data->x[i*(data->params->dim+1) + data->params->dim]);
  }

  printf("Printing %s:\n", data->params->fy);
  for(i=0; i<data->params->N; i++)
  {
    printf("\t%2d\n", data->y[i]);
  }

  return 0;
}

/*****************************************************************************
 *
 *  data_csvread
 *
 *****************************************************************************/

static int data_csvread( char *filename, int rowSz, int colSz, int skip_header,
                    const char *delimiter, const char *datatype, void *data){

  FILE* fp = NULL;
  char line[BUFSIZ];
  const char *tok;
  int i,j;

  assert(data);

  printf("Reading data from %s...", filename);
  fp = fopen(filename, "r");
  assert(fp);

  data_rmheader(line, fp, skip_header);
  for(i=0; i<colSz; i++)
  {
      /* Read a line from the file and parse each value creating a token
       * convert and store token to data vector with appropriate type
       */
      fgets(line, BUFSIZ, fp);
      tok = strtok(line, delimiter);
      CONVERT(tok, datatype, data, i*rowSz);

      /* keep reading tokens from the buffer (using NULL)
       * until reach the end of line, each time converting them
       * and add them to data vector
       */
      for(j=1; j<rowSz; j++)
      {
         tok = strtok(NULL, delimiter);
         CONVERT(tok, datatype, data, i*rowSz+j);
      }
  }

  fclose(fp);
  printf("\tDone\n");

  return 0;
}

/*****************************************************************************
 *
 *  data_rmheader
 *
 *****************************************************************************/

static void data_rmheader(char* line, FILE *fp, int skip_num){

  int i;
  for(i=0; i<skip_num; i++)
  {
    fgets(line, BUFSIZ, fp);
  }
}
