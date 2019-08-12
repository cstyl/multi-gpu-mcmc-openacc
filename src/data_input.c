#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "data_input.h"
#include "memory.h"
#include "timer.h"


#define SKIP_HEADER 1

struct data_s{
  dc_t *dc;     /* Data decomposition */
  int dimx;
  int dimy;
  int N;
  precision *x;
  int *y;
  char fx[FILENAME_MAX];
  char fy[FILENAME_MAX];
};

static int data_csvread(pe_t *pe, char *filename, int rowSz, int colSz, int skip_header,
                    const char *delimiter, const char *datatype, void *data);
static void data_rmheader(char* line, FILE *fp, int skip_num);
static int data_allocate_x(data_t *data);
static int data_allocate_y(data_t *data);
static int convert_tok(const char *tok, const char *datatype, void *data, int pos);

// void data_create_device_x(precision *x, int size);
// void data_create_device_y(int *y, int size);
void data_create_device_x(precision *x, int start, int end);
void data_create_device_y(int *y, int start, int end);
void data_update_device_x(precision *x, int start, int end);
void data_update_device_y(int *y, int start, int end);
void data_free_device_x(precision *x);
void data_free_device_y(int *y);

/*****************************************************************************
 *
 *  data_create_train
 *
 *****************************************************************************/

int data_create_train(pe_t *pe, data_t **pdata){

  data_t *data = NULL;

  assert(pe);

  data = (data_t *) calloc(1, sizeof(data_t));
  assert(data);
  if(data == NULL) pe_fatal(pe, "calloc(data_t) failed\n");

  data_dimx_set(data, DIMX_DEFAULT);
  data_dimy_set(data, DIMY_DEFAULT);
  data_N_set(data, N_TRAIN_DEFAULT);
  data_fx_set(data, TRAIN_X_DEFAULT);
  data_fy_set(data, TRAIN_Y_DEFAULT);

  *pdata = data;

  return 0;
}

/*****************************************************************************
 *
 *  data_create_test
 *
 *****************************************************************************/

int data_create_test(pe_t *pe, data_t **pdata){

   data_t *data = NULL;

   assert(pe);

   data = (data_t *) calloc(1, sizeof(data_t));
   assert(data);
   if(data == NULL) pe_fatal(pe, "calloc(data_t) failed\n");

   data_dimx_set(data, DIMX_DEFAULT);
   data_dimy_set(data, DIMY_DEFAULT);
   data_N_set(data, N_TEST_DEFAULT);
   data_fx_set(data, TEST_X_DEFAULT);
   data_fy_set(data, TEST_Y_DEFAULT);

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

   int nthreads;
   dc_nthreads(data->dc, &nthreads);
   /* Make sure each device memory is deleted */
   #pragma omp parallel default(shared) num_threads(nthreads)
   {
     int tid = omp_get_thread_num();
     /* Switch to the appropriate device and deallocate memory on it */
     #pragma acc set device_num(tid) device_type(acc_device_nvidia)
     data_free_device_x(data->x);
     data_free_device_y(data->y);
   }

   mem_free((void**)&data->x);
   mem_free((void**)&data->y);

   if(data->dc) dc_free(data->dc);  /* Do that after the devices are dealloacated */
   mem_free((void**)&data);

   return 0;
 }

/*****************************************************************************
 *
 *  data_init_train_rt
 *
 *****************************************************************************/

int data_init_train_rt(pe_t *pe, rt_t *rt, data_t *train){

  int dimx, dimy, N;
  char x[FILENAME_MAX], y[FILENAME_MAX];

  assert(rt);
  assert(train);

  if(rt_int_parameter(rt, "train_dimx", &dimx))
  {
    data_dimx_set(train, dimx);
  }

  if(rt_int_parameter(rt, "train_dimy", &dimy))
  {
    data_dimy_set(train, dimy);
  }

  if(rt_int_parameter(rt, "train_N", &N))
  {
    data_N_set(train, N);
  }

  if(rt_string_parameter(rt, "train_x", x, FILENAME_MAX))
  {
    data_fx_set(train, x);
  }

  if(rt_string_parameter(rt, "train_y", y, FILENAME_MAX))
  {
    data_fy_set(train, y);
  }

  dc_create(pe, &train->dc);
  dc_init_rt(pe, rt, train->dc);
  dc_print_info(pe, train->dc);
  dc_decompose(train->N, train->dimx, train->dimy, train->dc);

  data_allocate_x(train);
  data_allocate_y(train);

  return 0;
}

/*****************************************************************************
 *
 *  data_init_test_rt
 *
 *****************************************************************************/

int data_init_test_rt(pe_t *pe, rt_t *rt, data_t *test){

  int dimx, dimy, N;
  char x[FILENAME_MAX], y[FILENAME_MAX];

  assert(rt);
  assert(test);

  if(rt_int_parameter(rt, "test_dimx", &dimx))
  {
    data_dimx_set(test, dimx);
  }

  if(rt_int_parameter(rt, "test_dimy", &dimy))
  {
    data_dimy_set(test, dimy);
  }

  if(rt_int_parameter(rt, "test_N", &N))
  {
    data_N_set(test, N);
  }

  if(rt_string_parameter(rt, "test_x", x, FILENAME_MAX))
  {
    data_fx_set(test, x);
  }

  if(rt_string_parameter(rt, "test_y", y, FILENAME_MAX))
  {
    data_fy_set(test, y);
  }

  dc_create(pe, &test->dc);
  dc_init_rt(pe, rt, test->dc);
  dc_print_info(pe, test->dc);
  dc_decompose(test->N, test->dimx, test->dimy, test->dc);

  data_allocate_x(test);
  data_allocate_y(test);

  return 0;
}

/*****************************************************************************
 *
 *  data_input_train_info
 *
 *****************************************************************************/

int data_input_train_info(pe_t *pe, data_t *train){

  int dimx, dimy, N;
  char fx[FILENAME_MAX], fy[FILENAME_MAX];

  assert(pe);
  assert(train);

  data_dimx(train, &dimx);
  data_dimy(train, &dimy);
  data_N(train, &N);
  data_fx(train, fx);
  data_fy(train, fy);

  pe_info(pe, "\n");
  pe_info(pe, "Training Set Properties\n");
  pe_info(pe, "-----------------------\n");
  pe_info(pe, "%30s\t\t%d\n", "Number of Datapoints:", N);
  pe_info(pe, "%30s\t\t%d %s\n", "Datapoints Dimensionality:", dimx, "(includes bias)");
  pe_info(pe, "%30s\t\t%d\n", "Labels Dimensionality:", dimy);
  pe_info(pe, "%30s\t\t%s\n", "Datapoints Filename:", fx);
  pe_info(pe, "%30s\t\t%s\n", "Labels Filename:", fy);

  return 0;
}

/*****************************************************************************
 *
 *  data_input_test_info
 *
 *****************************************************************************/

int data_input_test_info(pe_t *pe, data_t *test){

  int dimx, dimy, N;
  char fx[FILENAME_MAX], fy[FILENAME_MAX];

  assert(pe);
  assert(test);

  data_dimx(test, &dimx);
  data_dimy(test, &dimy);
  data_N(test, &N);
  data_fx(test, fx);
  data_fy(test, fy);

  pe_info(pe, "\n");
  pe_info(pe, "Test Set Properties\n");
  pe_info(pe, "-------------------\n");
  pe_info(pe, "%30s\t\t%d\n", "Number of Datapoints:", N);
  pe_info(pe, "%30s\t\t%d %s\n", "Datapoints Dimensionality:", dimx, "(includes bias)");
  pe_info(pe, "%30s\t\t%d\n", "Labels Dimensionality:", dimy);
  pe_info(pe, "%30s\t\t%s\n", "Datapoints Filename:", fx);
  pe_info(pe, "%30s\t\t%s\n", "Labels Filename:", fy);

  return 0;
}

/*****************************************************************************
 *
 *  data_dimx_set
 *
 *****************************************************************************/

int data_dimx_set(data_t *data, int dimx){

  assert(data);

  data->dimx = dimx;

  return 0;
}

/*****************************************************************************
 *
 *  data_dimy_set
 *
 *****************************************************************************/

int data_dimy_set(data_t *data, int dimy){

  assert(data);

  data->dimy = dimy;

  return 0;
}

/*****************************************************************************
 *
 *  data_N_set
 *
 *****************************************************************************/

int data_N_set(data_t *data, int N){

  assert(data);

  data->N = N;

  return 0;
}

/*****************************************************************************
 *
 *  data_fx_set
 *
 *****************************************************************************/

int data_fx_set(data_t *data, const char *filename){

  assert(data);

  sprintf(data->fx, "%s", filename);

  return 0;
}

/*****************************************************************************
 *
 *  data_fy_set
 *
 *****************************************************************************/

int data_fy_set(data_t *data, const char *filename){

  assert(data);

  sprintf(data->fy, "%s", filename);

  return 0;
}

/*****************************************************************************
 *
 *  data_x_set
 *
 *****************************************************************************/

int data_x_set(data_t *data, precision *x){

  assert(data);

  data->x = x;

  return 0;
}

/*****************************************************************************
 *
 *  data_y_set
 *
 *****************************************************************************/

int data_y_set(data_t *data, int *y){

  assert(data);

  data->y = y;

  return 0;
}


/*****************************************************************************
 *
 *  data_dimx
 *
 *****************************************************************************/

int data_dimx(data_t *data, int *dimx){

  assert(data);

  *dimx = data->dimx;

  return 0;
}

/*****************************************************************************
 *
 *  data_dimy
 *
 *****************************************************************************/

int data_dimy(data_t *data, int *dimy){

  assert(data);

  *dimy = data->dimy;

  return 0;
}

/*****************************************************************************
 *
 *  data_N
 *
 *****************************************************************************/

int data_N(data_t *data, int *N){

  assert(data);

  *N = data->N;

  return 0;
}

/*****************************************************************************
 *
 *  data_fx
 *
 *****************************************************************************/

int data_fx(data_t *data, char *fx){

  assert(data);

  sprintf(fx, "%s", data->fx);

  return 0;
}

/*****************************************************************************
 *
 *  data_fy
 *
 *****************************************************************************/

int data_fy(data_t *data, char *fy){

  assert(data);

  sprintf(fy, "%s", data->fy);

  return 0;
}

/*****************************************************************************
 *
 *  data_x
 *
 *****************************************************************************/

int data_x(data_t *data, precision **px){

  assert(data);

  *px = data->x;

  return 0;
}

/*****************************************************************************
 *
 *  data_y
 *
 *****************************************************************************/

int data_y(data_t *data, int **py){

  assert(data);

  *py = data->y;

  return 0;
}

/*****************************************************************************
 *
 *  data_dc
 *
 *****************************************************************************/

int data_dc(data_t *data, dc_t **pdc){

  assert(data);

  *pdc = data->dc;

  return 0;
}

/*****************************************************************************
 *
 *  data_read_file
 *  at the moment each process reads the entire dataset
 *
 *****************************************************************************/

int data_read_file(pe_t *pe, data_t *data){

  int nthreads;
  int *txlow = NULL, *txhi = NULL;
  int *tylow = NULL, *tyhi = NULL;
  int *pxlow = NULL, *pxhi = NULL;
  int *pylow = NULL, *pyhi = NULL;

  assert(data);
  assert(data->x);
  assert(data->y);

  data_csvread(pe, data->fx, data->dimx, data->N, SKIP_HEADER, ",", "precision", data->x);
  data_csvread(pe, data->fy, data->dimy, data->N, SKIP_HEADER, ",", "int", data->y);

  /*  Use OpenMP to split the data and send to appropriate devices
  */
  dc_nthreads(data->dc, &nthreads);
  dc_txb(data->dc, &txlow, &txhi);
  dc_tyb(data->dc, &tylow, &tyhi);
  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    /* Switch to the appropriate device
    *  Need to make sure each device is allocate the correct portion of the data
    *  Since the whole matrix is available on each process,
    *  no offset is required to be subtracted
    */
    #pragma acc set device_num(tid) device_type(acc_device_nvidia)
    data_update_device_x(data->x, txlow[tid], txhi[tid]);
    data_update_device_y(data->y, tylow[tid], tyhi[tid]);
  }

  return 0;
}

/*****************************************************************************
 *
 *  data_csvread
 *
 *****************************************************************************/

static int data_csvread(pe_t *pe, char *filename, int rowSz, int colSz, int skip_header,
                    const char *delimiter, const char *datatype, void *data){

  FILE* fp = NULL;
  char line[BUFSIZ];
  const char *tok;
  int i,j;

  assert(data);

  pe_info(pe, "%30s\t%50s", "Reading data from:", filename);
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
      convert_tok(tok, datatype, data, i*rowSz);

      /* keep reading tokens from the buffer (using NULL)
       * until reach the end of line, each time converting them
       * and add them to data vector
       */
      for(j=1; j<rowSz; j++)
      {
         tok = strtok(NULL, delimiter);
         convert_tok(tok, datatype, data, i*rowSz+j);
      }
  }

  fclose(fp);
  pe_info(pe, "\tDone\n");

  return 0;
}

/*****************************************************************************
 *
 *  data_create_device_x
 *
 *****************************************************************************/

void data_create_device_x(precision *x, int start, int end){
  TIMER_start(TIMER_CREATE_DATA);
  #pragma acc enter data create(x[start:end])
  TIMER_stop(TIMER_CREATE_DATA);
}

/*****************************************************************************
 *
 *  data_create_device_y
 *
 *****************************************************************************/

void data_create_device_y(int *y, int start, int end){
  TIMER_start(TIMER_CREATE_DATA);
  #pragma acc enter data create(y[start:end])
  TIMER_stop(TIMER_CREATE_DATA);
}

/*****************************************************************************
 *
 *  data_update_device_x
 *
 *****************************************************************************/

void data_update_device_x(precision *x, int start, int end){
  TIMER_start(TIMER_UPDATE_DATA);
  #pragma acc update device(x[start:end])
  TIMER_stop(TIMER_UPDATE_DATA);
}

/*****************************************************************************
 *
 *  data_update_device_y
 *
 *****************************************************************************/

void data_update_device_y(int *y, int start, int end){
  TIMER_start(TIMER_UPDATE_DATA);
  #pragma acc update device(y[start:end])
  TIMER_stop(TIMER_UPDATE_DATA);
}

/*****************************************************************************
 *
 *  data_free_device_x
 *
 *****************************************************************************/

void data_free_device_x(precision *x){
  #pragma acc exit data delete(x)
}

/*****************************************************************************
 *
 *  data_free_device_y
 *
 *****************************************************************************/

void data_free_device_y(int *y){
  #pragma acc exit data delete(y)
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

/*****************************************************************************
 *
 *  data_allocate_x
 *
 *****************************************************************************/

static int data_allocate_x(data_t *data){

  int nthreads;
  int *txlow = NULL, *txhi = NULL;

  assert(data);

  mem_malloc_precision(&data->x, data->dimx * data->N);

  dc_nthreads(data->dc, &nthreads);
  dc_txb(data->dc, &txlow, &txhi);

  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    // int size = txhi[tid] - txlow[tid];
    /* Switch to the appropriate device and allocate memory on it */
    #pragma acc set device_num(tid) device_type(acc_device_nvidia)
    data_create_device_x(data->x, txlow[tid], txhi[tid]);
  }

  return 0;
}

/*****************************************************************************
 *
 *  data_allocate_y
 *
 *****************************************************************************/

static int data_allocate_y(data_t *data){

  int nthreads;
  int *tylow = NULL, *tyhi = NULL;

  assert(data);

  mem_malloc_integers(&data->y, data->dimy * data->N);

  dc_nthreads(data->dc, &nthreads);
  dc_tyb(data->dc, &tylow, &tyhi);

  #pragma omp parallel default(shared) num_threads(nthreads)
  {
    int tid = omp_get_thread_num();
    // int size = tyhi[tid] - tylow[tid];
    /* Switch to the appropriate device and allocate memory on it */
    #pragma acc set device_num(tid) device_type(acc_device_nvidia)
    data_create_device_y(data->y, tylow[tid], tyhi[tid]);
  }

  return 0;
}

/*****************************************************************************
 *
 *  convert_tok
 *  converts an input token from character
 *  to real or integer based on the desired type
 *
 *****************************************************************************/

static int convert_tok(const char *tok, const char *datatype, void *data, int pos){

  if(!strcmp(datatype, "int")){
    *((int *)data + pos) = atoi(tok);
  }else if(!strcmp(datatype,"precision")){
    *((precision *)data + pos) = (precision)atof(tok);
  }else{
    fprintf(stderr, "ERROR::%s:%d:%s: Invalid type argument \"%s\"!\n",\
            __FILE__, __LINE__,__func__, datatype);\
    exit(1);
  }

  return 0;
}
