#ifndef __STRUCTS_H__
#define __STRUCTS_H__
#include <stdio.h>
#include <stdlib.h>

#ifdef FLOAT
  typedef float precision;
#else
  typedef double precision;
#endif

typedef struct data_vectors
{
  precision *x;
  int *y;
  int dim, Nd;
  char fx[BUFSIZ], fy[BUFSIZ];
} data;

typedef struct sample
{
  precision *values;
  precision prior;
  precision likelihood;
} sample;

typedef struct mcmc_metropolis
{
  precision *chain;
  sample proposed, current;
  precision acceptance_prob, acceptance_ratio;
  int Ns, Nburn, dim;
  precision rwsd;
} metropolis;

typedef struct autocorrelation_lagk
{
  double *mean;
  double *std;
  double *value;
} autocorrelation;

typedef struct mcmc
{
  metropolis metropolis;
  data train, test;
} mcmc;

#endif //__STRUCTS_H__
