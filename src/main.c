#include <stdio.h>

#include "pe.h"
#include "mcmc.h"

/*****************************************************************************
 *
 *  main
 *
 *****************************************************************************/

int main(int argc, char ** argv) {

  char inputfile[FILENAME_MAX] = "input";


  MPI_Init(&argc, &argv);

  #ifdef _OPENMP
  printf("hello with openmp\n");
  #endif

  if (argc > 1) sprintf(inputfile, "%s", argv[1]);

  mcmc_run(inputfile);

  MPI_Finalize();

  return 0;
}
