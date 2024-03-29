/*****************************************************************************
 *
 *  random.c
 *
 *  Random number generators for serial and parallel.
 *
 *  Edinburgh Soft Matter and Statistical Physics Group and
 *  Edinburgh Parallel Computing Centre
 *
 *  (c) 2014 The University of Edinburgh
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *
 *****************************************************************************/

#include <assert.h>
#include <math.h>

#include "pe.h"
#include "ran.h"
#include "runtime.h"

struct lecuyer {
  double rspare;                  /* Spare Gaussian random number */
  int    ispare;                  /* Flag for ditto */
  int    rstate[5];               /* State space (32-bit) */
};

static struct lecuyer p_rng;      /* Parallel generator */
static struct lecuyer s_rng;      /* Serial generator */

static double ran_gaussian(struct lecuyer *);
static double ran_lecuyer(struct lecuyer *);

#define _rmodulus 4.656612873077393e-10
#define _m        2147483647
#define _a1       107374182
#define _q1       20
#define _r1       7
#define _a5       104480
#define _q5       20554
#define _r5       1727

/****************************************************************************
 *
 *  ran_init
 *
 *  Initialise both serial and parallel random states.
 *
 ****************************************************************************/

int ran_init(pe_t * pe) {

  int scalar_seed = 7361237;

  ran_init_seed(pe, scalar_seed);

  return 0;
}

int ran_init_rt(pe_t * pe, rt_t * rt) {

  int n;
  int scalar_seed = 7361237;

  assert(pe);
  assert(rt);

  /* Look for "random_seed" in the user input, or use a default. */

  n = rt_int_parameter(rt, "random_seed", &scalar_seed);

  if (n == 0) {
    pe_info(pe, "[Default] Random number seed: %d\n", scalar_seed);
  }
  else {
    pe_info(pe, "[User   ] Random number seed: %d\n", scalar_seed);
  }

  ran_init_seed(pe, scalar_seed);

  return 0;
}

int ran_init_seed(pe_t * pe, int scalar_seed) {

  assert(pe);

  /* Serial generator */

  s_rng.ispare = 0;

  s_rng.rstate[0] = scalar_seed;
  s_rng.rstate[1] = 1;
  s_rng.rstate[2] = 0;
  s_rng.rstate[3] = 3;
  s_rng.rstate[4] = 4;

  /* Parallel generator */

  p_rng.ispare = 0;

  p_rng.rstate[0] = scalar_seed;
  p_rng.rstate[1] = pe_mpi_size(pe);
  p_rng.rstate[2] = pe_mpi_rank(pe);
  p_rng.rstate[3] = 3;
  p_rng.rstate[4] = 4;

  return 0;
}

/*****************************************************************************
 *
 *  ran_serial_uniform
 *  ran_serial_gaussian
 *  ran_parallel_uniform
 *  ran_parallel_gaussian
 *
 *  These make up the interface; the routines below actually do the work.
 *
 *****************************************************************************/

double ran_serial_uniform() {
  return ran_lecuyer(&s_rng);
}

double ran_serial_gaussian() {
  return ran_gaussian(&s_rng);
}

double ran_parallel_uniform() {
  return ran_lecuyer(&p_rng);
}

double ran_parallel_gaussian() {
  return ran_gaussian(&p_rng);
}

/*****************************************************************************
 *
 *  ran_gaussian
 *
 *  Return a single random number from Gaussian distribution
 *  with unit variance.
 *
 *  The transformation for uniform to Gaussian is described
 *  in Numerical Recipes by Press et al.
 *
 *****************************************************************************/

static double ran_gaussian(struct lecuyer * rng) {

  double result;
  double ran1, ran2;
  double rsq, f;

  if (rng->ispare) {
    /* Use spare number */
    result = rng->rspare;
    rng->ispare = 0;
  }
  else {

    do {
      ran1 = 2.0*ran_lecuyer(rng) - 1.0;
      ran2 = 2.0*ran_lecuyer(rng) - 1.0;

      rsq  = ran1*ran1 + ran2*ran2;
    } while (rsq >= 1.0 || rsq <= 0.0);

    f = sqrt(-2.0*log(rsq)/rsq);
    rng->rspare = f*ran1;
    rng->ispare = 1;
    result = f*ran2;
  }

  return result;
}

/*****************************************************************************
 *
 *  ran_lecuyer
 *
 *  This returns a random number uniformly distributed on [0,1].
 *  The algorithm is that of L'Ecuyer...
 *
 *  L'Ecuyer, ACM Transactions on Modeling and Computer Simulation,
 *  \textbf{3}, 87--98, 1993.
 *
 *  The argument is the generator state to be used.
 *
 *****************************************************************************/

static double ran_lecuyer(struct lecuyer * rng) {

  int h, p1, p5;

  h = rng->rstate[4] / _q5;
  p5 = _a5*(rng->rstate[4] - h*_q5) - h*_r5;

  rng->rstate[4] = rng->rstate[3];
  rng->rstate[3] = rng->rstate[2];
  rng->rstate[2] = rng->rstate[1];
  rng->rstate[1] = rng->rstate[0];

  h = rng->rstate[0]/_q1;
  p1 = _a1*(rng->rstate[0] - h*_q1) - h*rng->rstate[0];

  if (p1 < 0) p1 += _m;
  if (p5 > 0) p5 -= _m;

  rng->rstate[0] = p1 + p5;
  if (rng->rstate[0] < 0) rng->rstate[0] += _m;

  return (_rmodulus*rng->rstate[0]);
}
