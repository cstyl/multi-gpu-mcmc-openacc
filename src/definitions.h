#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#include <float.h>
#ifdef _OPENACC
#include "openacc.h"
#endif

#define REST __restrict__

#ifdef _FLOAT_
  typedef float precision;
  #define PRECISION_TOLERANCE  1.0e-07
  #define POTRF LAPACKE_spotrf
  #define TRMV cblas_strmv
  #define GEMV cublasSgemv
  #define PRINT_PREC FLT_DIG+3
#else
  typedef double precision;
  #define PRECISION_TOLERANCE 1.0e-14
  #define POTRF LAPACKE_dpotrf
  #define TRMV cblas_dtrmv
  #define GEMV cublasDgemv
  #define PRINT_PREC DBL_DIG+3
#endif

static const int DEFAULT_PROCS = 1;
static const int DEFAULT_THREADS = 1;

static const char ALGORITHM_DEFAULT[BUFSIZ] = "metropolis";
static const char KERNEL_DEFAULT[BUFSIZ] = "mvn_block";
static const char LHOOD_DEFAULT[BUFSIZ] = "logistic_regression";
static const char ESS_CASE_DEFAULT[BUFSIZ] = "max";
static const char MC_CASE_DEFAULT[BUFSIZ] = "logistic_regression";

static const int DIMX_DEFAULT = 3;
static const int DIMY_DEFAULT = 1;
static const int TUNE_DEFAULT = 0;
static const int N_TRAIN_DEFAULT = 500;
static const int N_TEST_DEFAULT = 100;
static const int N_DATA_DEFAULT = 500;
static const int N_CHAIN_DEFAULT = 500;

static const char TRAIN_X_DEFAULT[FILENAME_MAX] = "../data/synthetic/default/X_train.csv";
static const char TRAIN_Y_DEFAULT[FILENAME_MAX] = "../data/synthetic/default/Y_train.csv";
static const char TEST_X_DEFAULT[FILENAME_MAX] = "../data/synthetic/default/X_test.csv";
static const char TEST_Y_DEFAULT[FILENAME_MAX] = "../data/synthetic/default/Y_test.csv";

// RWSD_DEFAULT = 2.38 / sqrt(DIM_DEFAULT - 1);
static const precision RWSD_DEFAULT = 1.6829141392239828;
static const int RAND_INIT_DEFAULT = 0;

static const int MAXLAG_AUTO_DEFAULT = 249;
static const precision THRESHOLD_AUTO_DEFAULT = 0.1;

static const int OUTFREQ_CHAIN_DEFAULT = 50;
static const int OUTFREQ_AUTO_DEFAULT = 50;
static const char OUTDIR_DEFAULT[FILENAME_MAX] = "../out";


#endif // __DEFINITIONS_H__
