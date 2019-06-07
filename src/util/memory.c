#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "macros.h"
#include "flags.h"

static int malloc_sample(sample **array, int elements);
static int malloc_precision(precision **array, int elements);
static int malloc_integers(int **array, int elements);

/* Allocates one dimensional arrays for train and test vectors
 * Note that one dimensional arrays are chosen so that
 * a contiguous block of memory is obtained
 */
int allocate_data_vectors(data *train, data *test)
{
  int status;

  status = malloc_precision(&train->x, train->dim * train->Nd);
	CHECK_ERROR(status, "malloc_precision()");
  status = malloc_integers(&train->y, train->dim * train->Nd);
	CHECK_ERROR(status, "malloc_integers()");

  status = malloc_precision(&test->x, test->dim * test->Nd);
	CHECK_ERROR(status, "malloc_precision()");
  status = malloc_integers(&test->y, test->dim * test->Nd);
	CHECK_ERROR(status, "malloc_integers()");

  return SIM_SUCCESS;
}

/* Allocates one dimensional arrays for the generated chain that hosts both
 * samples during burn-in period and after(useful ones).
 * Allocates the temporary proposed and current sample array which will keep
 * the equivalent sample values at each iteration.
 * Note that one dimensional arrays are chosen so that
 * a contiguous block of memory is obtained
 */
int allocate_mcmc_vectors(metropolis *metropolis)
{
  int status;

  status = malloc_precision(&metropolis->chain, (metropolis->Ns + metropolis->Nburn)* metropolis->dim);
	CHECK_ERROR(status, "malloc_precision()");

  status = malloc_sample(&metropolis->proposed, 1);
  CHECK_ERROR(status, "malloc_sample()");
  status = malloc_sample(&metropolis->current, 1);
  CHECK_ERROR(status, "malloc_sample()");

  status = malloc_precision(&metropolis->proposed->values, metropolis->dim);
	CHECK_ERROR(status, "malloc_precision()");
  status = malloc_precision(&metropolis->current->values, metropolis->dim);
  CHECK_ERROR(status, "malloc_precision()");

  return SIM_SUCCESS;
}

int destroy_data_vectors(data *train, data *test)
{
  FREE(train->x);
  FREE(train->y);

  FREE(test->x);
  FREE(test->y);

  return SIM_SUCCESS;
}

int destroy_mcmc_vectors(metropolis *metropolis)
{
  FREE(metropolis->chain);

  FREE(metropolis->proposed->values);
  FREE(metropolis->current->values);

  FREE(metropolis->proposed);
  FREE(metropolis->current);

  return SIM_SUCCESS;
}

static int malloc_precision(precision **array, int elements)
{
	*array = NULL;

	CHECK_VALID_MEM_REQUEST(elements);

	*array = (precision *) malloc(elements * sizeof(precision));
	CHECK_MALLOC(*array);


	return SIM_SUCCESS;
}

static int malloc_sample(sample **array, int elements)
{
	*array = NULL;

	CHECK_VALID_MEM_REQUEST(elements);

	*array = (sample *) malloc(elements * sizeof(sample));
	CHECK_MALLOC(*array);


	return SIM_SUCCESS;
}

static int malloc_integers(int **array, int elements)
{
	*array = NULL;

	CHECK_VALID_MEM_REQUEST(elements);

	*array = (int *) malloc(elements * sizeof(int));

	CHECK_MALLOC(*array);

	return SIM_SUCCESS;
}
