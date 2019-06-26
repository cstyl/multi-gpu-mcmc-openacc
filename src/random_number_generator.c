#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gsl_rng.h>
#include <gsl/gsl_randist.h>  // to obtain various pdf

#include "random_number_generator.h"

struct rng_s{
  cmd_t *cmd;
  gsl_rng **instances;
  const gsl_rng_type *type;
};

enum rng_error {RNG_SUCCESS = 0,
                RNG_ALLOC_ERROR,
                RNG_ERROR
};

static int rng_malloc(gsl_rng ***array, int elements);
static void rng_choose_type(rng_t *rng);
static int rng_allocate_instances(rng_t *rng);
static void rng_seed_instances(rng_t *rng);

/*****************************************************************************
 *
 *  rng_create
 *
 *****************************************************************************/

int rng_create(cmd_t *cmd, rng_t **prng){

  rng_t *rng = NULL;

  assert(cmd);

  rng = (rng_t *) calloc(1, sizeof(rng_t));
  assert(rng);
  if(rng == NULL)
  {
    printf("calloc(rng_t) failed\n");
    exit(RNG_ERROR);
  }

  rng->cmd = cmd;

  /* Allocate two more, 1 for bias one for accept/reject step */
  rng_malloc(&rng->instances, rng->cmd->dim+2);
  assert(rng->instances);
  if(rng->instances == NULL)
  {
    printf("malloc_rng(gsl_rng) failed\n");
    exit(RNG_ERROR);
  }

  *prng = rng;

  return RNG_SUCCESS;
};

/*****************************************************************************
 *
 *  rng_setup
 *
 *****************************************************************************/

int rng_setup(rng_t *rng){
  /*
   * Choose rng type
   * Allocate rng for each instance
   * Seed each instance
   */

  assert(rng);

  rng_choose_type(rng);
  rng_allocate_instances(rng);
  rng_seed_instances(rng);

  return RNG_SUCCESS;
};

/*****************************************************************************
 *
 *  rng_free
 *
 *****************************************************************************/

int rng_free(rng_t *rng){

  int i;
  int count = rng->cmd->dim + 2; // plus bias and accept/reject

  assert(rng);

  for(i=0; i<count; i++)
  {
    if(rng->instances[i]) gsl_rng_free(rng->instances[i]);
    assert(rng->instances[i] != NULL);
  }

  if(rng->instances) free(rng->instances);
  assert(rng->instances != NULL);

  free(rng);
  assert(rng != NULL);

  return RNG_SUCCESS;
};

/*****************************************************************************
 *
 *  rng_normal
 *  Sample from a zero mean normal distribution
 *
 *****************************************************************************/

precision rng_normal(rng_t *rng, int i, precision rwsd){

  assert(rng);

  return (precision)gsl_ran_gaussian_ziggurat(rng->instances[i], (double)rwsd);
}

/*****************************************************************************
 *
 *  rng_uniform_prob
 *
 *****************************************************************************/

precision rng_uniform_prob(rng_t *rng, int i){

  assert(rng);
  return (precision)gsl_rng_uniform(rng->instances[i]);
}

/*****************************************************************************
 *
 *  rng_normal_prob
 *
 *****************************************************************************/

precision rng_normal_prob(precision *sample, int sd){

  assert(sample);
  return (precision)gsl_ran_gaussian_pdf(*sample, sd);
}

/*****************************************************************************
 *
 *  rng_malloc
 *
 *****************************************************************************/

static int rng_malloc(gsl_rng ***array, int elements){

  *array = NULL;
  assert(elements);

  *array = (gsl_rng **) malloc(elements * sizeof(gsl_rng));
  assert(array);

  return RNG_SUCCESS;
}

/*****************************************************************************
 *
 *  rng_choose_type
 *
 *****************************************************************************/

static void rng_choose_type(rng_t *rng){

#if RNG_TYPE == MT19937
		rng->type = gsl_rng_mt19937;
#elif RNG_TYPE == RANLXS2
		rng->type = gsl_rng_ranlxs2;
#elif RNG_TYPE == RANLXD2
		rng->type = gsl_rng_ranlxd2;
#elif RNG_TYPE == RANLUXS389
		rng->type = gsl_rng_ranlux389;
#elif RNG_TYPE == CMRG
		rng->type = gsl_rng_cmrg;
#elif RNG_TYPE == MRG
		rng->type = gsl_rng_mrg;
#elif RNG_TYPE == TAUS
		rng->type = gsl_rng_taus;
#endif

}

/*****************************************************************************
 *
 *  rng_allocate_instances
 *
 *****************************************************************************/

static int rng_allocate_instances(rng_t *rng){

	int i;
  int count = rng->cmd->dim + 2; // plus bias and accept/reject
  assert(rng);

	for(i=0; i<count; i++)
	{
    rng->instances[i] = NULL;
		rng->instances[i] = gsl_rng_alloc(rng->type);
    assert(rng->instances[i]);
	}

	return RNG_SUCCESS;
}

/*****************************************************************************
 *
 *  rng_seed_instances
 *
 *****************************************************************************/

static void rng_seed_instances(rng_t *rng){

	int i;
  int count = rng->cmd->dim + 2;
	unsigned long int seed = count;

  assert(rng);
  /* Seed value is chosen as the product of the instances
   * with the current instance index.
   * For a parallel version maybe change it to be
   * dependent on the thread/process id
   */
	for(i=0; i<count; i++)
	{
		gsl_rng_set(rng->instances[i], (unsigned long int) i * seed);
	}
}
