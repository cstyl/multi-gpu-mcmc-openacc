#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "definitions.h"
#include "pe.h"
#include "data_input.h"
#include "logistic_regression.h"
#include "tests.h"

int test_lr_suite(void){

  int i, j;
  int dimx=3, dimy=1, N=5;
  precision lhood_ref, lhood_test, dot;

  pe_t *pe = NULL;
  data_t *data = NULL;
  lr_t *lr = NULL;

  precision x_ref[15] = {1.0000000000000000,1.4648106502470808,-1.5701058145856399,
                         1.0000000000000000,1.5407782876057425,-0.5819483312184370,
                         1.0000000000000000,0.1415183428679135,0.5459865569896000,
                         1.0000000000000000,0.4670300843306030,1.1185017644967441,
                         1.0000000000000000,1.6517639463377001,1.5968332608405691};

  int y_ref[5] = {-1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  1.0000000000000000,
                  -1.0000000000000000};

  precision sample[3] = {-10.0000000000000000, 5.0000000000000000, 10.0000000000000000};
  precision *x = (precision*) malloc(dimx * N * sizeof(precision));
  int *y = (int*) malloc(dimy * N * sizeof(int));

  pe_create(MPI_COMM_WORLD, PE_QUIET, &pe);
  assert(pe);
  test_assert(1);

  data_create_train(pe, &data);
  assert(data);
  test_assert(1);

  /* Initialize data struct */
  data_dimx_set(data, dimx);
  data_dimy_set(data, dimy);
  data_N_set(data, N);
  data_x_set(data, x);
  data_y_set(data, y);

  /* Copy reference data */
  for(i=0; i<N; i++)
  {
    for(j=0; j<dimx; j++)
    {
      x[i*dimx+j] = x_ref[i*dimx+j];
    }
    y[i] = y_ref[i];
  }

  /* Get reference likelihood */
  lhood_ref = 0.0;
  for(i=0; i<N; i++)
  {
    dot = 0.0;
    for(j=0; j<dimx; j++)
    {
      dot += sample[j] * x_ref[i*dimx+j];
    }
    lhood_ref -= log(1 + exp(-y_ref[i] * dot));
  }

  /* Start likelihood test */
  lr_lhood_create(pe, data, &lr);
  assert(lr);
  test_assert(1);

  lhood_test = lr_lhood(lr, sample);
  test_assert(fabs(lhood_ref - lhood_test) < TEST_PRECISION_TOLERANCE);

  data_free(data);
  lr_lhood_free(lr);

  pe_info(pe, "PASS\t./unit/test_logistic_regression\n");
  pe_free(pe);

  return 0;
}
