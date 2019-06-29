#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"

static void createheader(char *header, int dim, const char *varName);

/*****************************************************************************
 *
 *  util_write_array
 *
 *****************************************************************************/

int util_write_array(precision *data, int N, int dim, const char *dir, const char *varName){

  char filename[BUFSIZ], header[BUFSIZ];
  FILE *fp;

  util_create_dir(dir);
  sprintf(filename, "%s/%s.csv", dir, varName);

  printf("Writing %s...", filename);
  fp = fopen(filename, "w+");
  assert(fp);

  createheader(header, dim, varName);

  /* write header */
  fprintf(fp, "%s\n", header);

  int i,j;
  for(i=0; i<N; i++)
  {
    for(j=0; j<dim-1; j++)
    {
#ifdef FLOAT
      fprintf(fp, "%.7f,", data[i*dim+j]);
#else
      fprintf(fp, "%.16f,", data[i*dim+j]);
#endif
    }
#ifdef FLOAT
    fprintf(fp, "%.7f\n", data[i*dim+dim-1]);
#else
    fprintf(fp, "%.16f\n", data[i*dim+dim-1]);
#endif
  }

  fclose(fp);
  printf("\tDone\n");

  return 0;
}

/*****************************************************************************
 *
 *  util_create_dir
 *
 *****************************************************************************/

void util_create_dir(const char *directory){

  struct stat st = {0};

  if (stat(directory, &st) == -1) {
      if(mkdir(directory, 0700) == -1){
        fprintf(stderr, "Unable to create %s\n", directory);
      }
  }

}

/*****************************************************************************
 *
 *  createheader
 *
 *****************************************************************************/

static void createheader(char *header, int dim, const char *varName){

  int i;

  sprintf(header, "#%s%d,", varName, 0);
  for(i=1; i<dim-1; i++)
  {
    sprintf(header, "%s%s%d,", header, varName, i);
  }
  sprintf(header, "%s%s%d,", header, varName, dim-1);
}
