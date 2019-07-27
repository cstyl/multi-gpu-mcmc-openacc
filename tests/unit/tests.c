/*****************************************************************************
 *
 *  tests.c
 *
 *  This runs the tests.
 *
 *  Edinburgh Soft Matter and Statistical Physics Group and
 *  Edinburgh Parallel Computing Centre
 *
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *  (c) 2010-2016 The University of Edinburgh
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "tests.h"

int tests_create(void);

/*****************************************************************************
 *
 *  main
 *
 *****************************************************************************/

int main(int argc, char ** argv) {

  MPI_Init(&argc, &argv);

  tests_create();

  MPI_Finalize();

  return 0;
}

/*****************************************************************************
 *
 *  tests_create
 *
 *****************************************************************************/

int tests_create() {

  test_pe_suite();
  // test_coords_suite();

  test_random_suite();
  test_rt_suite();
  test_memory_suite();
  test_data_input_suite();
  test_lr_suite();
  test_prior_suite();
  test_mvn_suite();
  test_chain_suite();
  test_sample_suite();
  // test_timer_suite();


  return 0;
}

/*****************************************************************************
 *
 *  test_assert
 *
 *  Asimple assertion to control what happens in parallel.
 *
 *****************************************************************************/

void test_assert_info(const int lvalue, int line, const char * file) {

  int rank;

  if (lvalue) {
    /* ok */
  }
  else {
    /* Who has failed? */

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("[%d] Line %d file %s Failed test assertion\n", rank, line, file);
    MPI_Abort(MPI_COMM_WORLD, 0);
  }

  return;
}
