#include <stdio.h>
#include <stdlib.h>

#include "rng.h"
#include "macros.h"
#include "flags.h"

#ifndef RNG_TYPE
// default rng type
#define RNG_TYPE TAUS
#endif

static int allocate_rng(rng *rng);
static int allocate_instance(rng *rng);
static int malloc_rng(rng_i ***array, int elements);
static void choose_rng_type(rng *rng);
static void seed_instances(rng *rng);

/* Selects a type of rng to be used.
 * Allocates an array of pointers of type rng_i, one for each rng instance
 * Then each rng instance is allocated memory.
 * Each instance is seeded with a different seed
 * so that it generates a different sequence.
 */
int setup_rng(rng *rng)
{
  int status;

  choose_rng_type(rng);

  status = allocate_rng(rng);
  CHECK_ERROR(status, "allocate_rng()");

  seed_instances(rng);


  return SIM_SUCCESS;
}

int destroy_rng(rng *rng)
{
  /* Deallocate first each instance and then the structure */
  int i;
  for(i=0; i<rng->count; i++)
  {
    if(rng->instances[i]!=NULL)
    {
      gsl_rng_free(rng->instances[i]);
      rng->instances[i] = NULL;
    }
  }

  FREE(rng->instances);

  return SIM_SUCCESS;
}


static int allocate_rng(rng *rng)
{
  int status;
  status = malloc_rng(&rng->instances, rng->count);
	CHECK_ERROR(status, "malloc_rng()");

  status = allocate_instance(rng);
	CHECK_ERROR(status, "allocate_instance()");

  return SIM_SUCCESS;
}

static int malloc_rng(rng_i ***array, int elements)
{
  *array = NULL;
  CHECK_VALID_MEM_REQUEST(elements);

  *array = (rng_i **) malloc(elements * sizeof(rng_i));
  CHECK_MALLOC(*array);

  return SIM_SUCCESS;
}

static void choose_rng_type(rng *rng)
{
#if RNG_TYPE == MT19937
		rng->type = gsl_rng_mt19937;
    // printf("Selected RNG: MT19937");
#elif RNG_TYPE == RANLXS2
		rng->type = gsl_rng_ranlxs2;
    // printf("Selected RNG: RANLXS2");
#elif RNG_TYPE == RANLXD2
		rng->type = gsl_rng_ranlxd2;
    // printf("Selected RNG: RANLXD2");
#elif RNG_TYPE == RANLUXS389
		rng->type = gsl_rng_ranlux389;
    // printf("Selected RNG: RANLUXS389");
#elif RNG_TYPE == CMRG
		rng->type = gsl_rng_cmrg;
    // printf("Selected RNG: CMRG");
#elif RNG_TYPE == MRG
		rng->type = gsl_rng_mrg;
    // printf("Selected RNG: MRG");
#elif RNG_TYPE == TAUS
		rng->type = gsl_rng_taus;
    // printf("Selected RNG: TAUS");
#endif
    printf("Selected RNG type: %d\n", RNG_TYPE);
}

static int allocate_instance(rng *rng)
{
	int i;

	for(i=0; i<rng->count; i++)
	{
    rng->instances[i] = NULL;
		rng->instances[i] = gsl_rng_alloc(rng->type);
    CHECK_MALLOC(rng->instances[i]);
	}

	return SIM_SUCCESS;
}

static void seed_instances(rng *rng)
{
	int i;
	unsigned long int seed = rng->count;

  /* Seed value is chosen as the product of the instances
   * with the current instance index.
   * For a parallel version maybe change it to be
   * dependent on the thread/process id
   */
	for(i=0; i<rng->count; i++)
	{
		gsl_rng_set(rng->instances[i], (unsigned long int) i * seed);
	}
}
