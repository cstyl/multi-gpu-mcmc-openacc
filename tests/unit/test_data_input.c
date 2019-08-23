#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "definitions.h"
#include "pe.h"
#include "runtime.h"
#include "data_input.h"
#include "tests.h"

static int test_data_train_rt_default(pe_t *pe);
static int test_data_test_rt_default(pe_t *pe);
static int test_data_train_rt(pe_t *pe);
static int test_data_test_rt(pe_t *pe);
static int test_data_train_input_file(pe_t *pe);
static int test_data_test_input_file(pe_t *pe);

int test_data_input_suite(void){

  pe_t *pe = NULL;

  pe_create(MPI_COMM_WORLD, PE_QUIET, &pe);
  assert(pe);
  test_assert(1);

  test_data_train_rt_default(pe);
  test_data_test_rt_default(pe);
  test_data_train_rt(pe);
  test_data_test_rt(pe);
  test_data_train_input_file(pe);
  test_data_test_input_file(pe);

  pe_info(pe, "PASS\t./unit/test_data_input\n");
  pe_free(pe);

  return 0;
}

static int test_data_train_rt_default(pe_t *pe){

  assert(pe);

  int dimx, dimy, N;
  char fx[FILENAME_MAX], fy[FILENAME_MAX];
  precision *x = NULL;
  int *y = NULL;

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *train = NULL;

  dc_create(pe, &dc);
  assert(dc);

  rt_create(pe, &rt);
  assert(rt);

  data_create_train(pe, dc, &train);
  assert(train);
  test_assert(1);

  data_dimx(train, &dimx);
  test_assert(dimx == 3);
  data_dimy(train, &dimy);
  test_assert(dimy == 1);
  data_N(train, &N);
  test_assert(N == 500);
  data_fx(train, fx);
  test_assert(strcmp(fx, "../data/synthetic/default/X_train.csv") == 0);
  data_fy(train, fy);
  test_assert(strcmp(fy, "../data/synthetic/default/Y_train.csv") == 0);
  data_x(train, &x);
  test_assert(x == NULL);
  data_y(train, &y);
  test_assert(y == NULL);

  data_free(train);
  rt_free(rt);
  dc_free(dc);

  return 0;
}

static int test_data_test_rt_default(pe_t *pe){

  assert(pe);

  int dimx, dimy, N;
  char fx[FILENAME_MAX], fy[FILENAME_MAX];
  precision *x = NULL;
  int *y = NULL;

  rt_t *rt = NULL;
  dc_t *dc = NULL;
  data_t *test = NULL;

  dc_create(pe, &dc);
  assert(dc);

  rt_create(pe, &rt);
  assert(rt);

  data_create_test(pe, dc, &test);
  assert(test);
  test_assert(1);

  data_dimx(test, &dimx);
  test_assert(dimx == 3);
  data_dimy(test, &dimy);
  test_assert(dimy == 1);
  data_N(test, &N);
  test_assert(N == 100);
  data_fx(test, fx);
  test_assert(strcmp(fx, "../data/synthetic/default/X_test.csv") == 0);
  data_fy(test, fy);
  test_assert(strcmp(fy, "../data/synthetic/default/Y_test.csv") == 0);
  data_x(test, &x);
  test_assert(x == NULL);
  data_y(test, &y);
  test_assert(y == NULL);

  data_free(test);
  rt_free(rt);
  dc_free(dc);
  return 0;
}

static int test_data_train_rt(pe_t *pe){

  assert(pe);

  int dimx, dimy, N;
  char fx[FILENAME_MAX], fy[FILENAME_MAX];
  precision *x = NULL;
  int *y = NULL;

  rt_t *rt = NULL;
  data_t *train = NULL;
  dc_t *dc = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, dc, &train);
  assert(train);
  test_assert(1);

  /* testing train data runtime init */
  data_init_train_rt(pe, rt, train);

  data_dimx(train, &dimx);
  test_assert(dimx == 3);
  data_dimy(train, &dimy);
  test_assert(dimy == 1);
  data_N(train, &N);
  test_assert(N == 10);
  data_fx(train, fx);
  test_assert(strcmp(fx, "./data/X_train.csv") == 0);
  data_fy(train, fy);
  test_assert(strcmp(fy, "./data/Y_train.csv") == 0);
  data_x(train, &x);
  test_assert(x != NULL);
  data_y(train, &y);
  test_assert(y != NULL);

  data_free(train);
  rt_free(rt);
  dc_free(dc);
  return 0;
}

static int test_data_test_rt(pe_t *pe){

  assert(pe);

  int dimx, dimy, N;
  char fx[FILENAME_MAX], fy[FILENAME_MAX];
  precision *x = NULL;
  int *y = NULL;

  rt_t *rt = NULL;
  data_t *test = NULL;
  dc_t *dc = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  dc_create(pe, &dc);
  assert(dc);

  data_create_test(pe, dc, &test);
  assert(test);
  test_assert(1);

  /* testing test data runtime init */
  data_init_test_rt(pe, rt, test);

  data_dimx(test, &dimx);
  test_assert(dimx == 3);
  data_dimy(test, &dimy);
  test_assert(dimy == 1);
  data_N(test, &N);
  test_assert(N == 5);
  data_fx(test, fx);
  test_assert(strcmp(fx, "./data/X_test.csv") == 0);
  data_fy(test, fy);
  test_assert(strcmp(fy, "./data/Y_test.csv") == 0);
  data_x(test, &x);
  test_assert(x != NULL);
  data_y(test, &y);
  test_assert(y != NULL);

  data_free(test);
  rt_free(rt);
  dc_free(dc);
  return 0;
}

static int test_data_train_input_file(pe_t *pe){

  assert(pe);

  int dim=3, N=10;
  precision xref[30] = {1.0000000000000000,1.4648106502470808,-1.5701058145856399,
                        1.0000000000000000,1.5407782876057425,-0.5819483312184370,
                        1.0000000000000000,0.1415183428679135,0.5459865569896000,
                        1.0000000000000000,0.4670300843306030,1.1185017644967441,
                        1.0000000000000000,1.6517639463377001,1.5968332608405691,
                        1.0000000000000000,0.2072717426154768,-0.1663456615821221,
                        1.0000000000000000,1.4538306269217489,-0.0559761017281283,
                        1.0000000000000000,-1.2885503864798247,1.2163849967988494,
                        1.0000000000000000,1.6410274182494682,0.6013758387036505,
                        1.0000000000000000,1.5014949152091088,-1.4987275632154795};

  int yref[10] = {-1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000,
                  -1.0000000000000000};

  precision *x = NULL;
  int *y = NULL;

  rt_t *rt = NULL;
  data_t *train = NULL;
  dc_t *dc = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  dc_create(pe, &dc);
  assert(dc);

  data_create_train(pe, dc, &train);
  assert(train);
  test_assert(1);
  data_init_train_rt(pe, rt, train);

  data_read_file(pe, train);
  data_x(train, &x);
  data_y(train, &y);

  int i, j;

  for(i=0; i<N; i++){
    for(j=0; j<dim; j++){
        test_assert(fabs(x[i*dim+j] - xref[i*dim+j]) < TEST_PRECISION_TOLERANCE);
    }
    test_assert(fabs(y[i] - yref[i]) < TEST_PRECISION_TOLERANCE);
  }

  data_free(train);
  rt_free(rt);
  dc_free(dc);

  return 0;
}

static int test_data_test_input_file(pe_t *pe){

  assert(pe);

  int dim=3, N=5;
  precision xref[15] = {1.0000000000000000,-0.4469225773869986,1.4839047164310180,
                        1.0000000000000000,1.0353630781933607,-1.1761131434288334,
                        1.0000000000000000,1.4670978966146828,-0.1568568743995166,
                        1.0000000000000000,-0.8791561693397250,-0.6247706139077658,
                        1.0000000000000000,-0.5598310572811263,0.6429483711969575};

  int yref[5] = {-1.0000000000000000,
                 -1.0000000000000000,
                 -1.0000000000000000,
                 1.0000000000000000,
                 -1.0000000000000000};

  precision *x = NULL;
  int *y = NULL;

  rt_t *rt = NULL;
  data_t *test = NULL;
  dc_t *dc = NULL;

  rt_create(pe, &rt);
  assert(rt);
  rt_read_input_file(rt, "test.dat");

  dc_create(pe, &dc);
  assert(dc);

  data_create_test(pe, dc, &test);
  assert(test);
  test_assert(1);
  data_init_test_rt(pe, rt, test);

  data_read_file(pe, test);
  data_x(test, &x);
  data_y(test, &y);

  int i, j;

  for(i=0; i<N; i++){
    for(j=0; j<dim; j++){
        test_assert(fabs(x[i*dim+j] - xref[i*dim+j]) < TEST_PRECISION_TOLERANCE);
    }
    test_assert(fabs(y[i] - yref[i]) < TEST_PRECISION_TOLERANCE);
  }

  data_free(test);
  rt_free(rt);
  dc_free(dc);
  return 0;
}
