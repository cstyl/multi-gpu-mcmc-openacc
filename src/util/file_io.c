#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "macros.h"
#include "flags.h"

#define SKIP_HEADER 1
#define Y_DIM 1

static void rmheader(char* line, FILE *file, int skip_num);
static int csvread( char *filename, int rowSz, int colSz, int skip_header,
             const char *delimiter, const char *datatype,
             void *data);

int read_files(data *set)
{
  int status;

  status =csvread(set->fx, set->dim, set->Nd, SKIP_HEADER, ",", "precision", set->x);
  CHECK_ERROR(status, "csvread()");

  status =csvread(set->fy, Y_DIM, set->Nd, SKIP_HEADER, ",", "int", set->y);
  CHECK_ERROR(status, "csvread()");

  return SIM_SUCCESS;
}

static int csvread( char *filename, int rowSz, int colSz, int skip_header,
                    const char *delimiter, const char *datatype,
                    void *data)
{
    FILE* fp;
    char line[BUFSIZ];
    const char *tok;
    int i,j;

    printf("Reading data from %s...\n", filename);
    fp = fopen(filename, "r");
    CHECK_FILE_OPEN(fp, filename);

    rmheader(line, fp, skip_header);
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
    printf("Reading data from %s completed...\n", filename);
    return SIM_SUCCESS;
}

static void rmheader(char* line, FILE *fp, int skip_num)
{
  int i;
  for(i=0; i<skip_num; i++)
  {
    fgets(line, BUFSIZ, fp);
  }
}
