#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "structs.h"
#include "flags.h"
#include "macros.h"
#include "util.h"
#include "rng.h"

int main(int argc, char *argv[])
{
  mcmc mcmc;
  rng  rng;

  int error_status = SIM_SUCCESS;

  error_status = parse_args(argc, argv, &mcmc);
  CHECK(error_status, "parse_args()");
	print_parameters(mcmc);

	error_status = allocate_data_vectors(&mcmc.train, &mcmc.test);
	CHECK(error_status, "allocate_data_vectors()");
	error_status = allocate_mcmc_vectors(&mcmc.metropolis);
	CHECK(error_status, "allocate_data_vectors()");

	error_status = read_files(&mcmc.train);
	CHECK(error_status, "read_files()");
	error_status = read_files(&mcmc.test);
	CHECK(error_status, "read_files()");

	/* Setup RNG */
  rng.count = mcmc.metropolis.dim;
  error_status = setup_rng(&rng);
  CHECK(error_status, "setup_rng()");

  /* Initialise samples */

	/* Run mcmc */

	error_status = destroy_data_vectors(&mcmc.train, &mcmc.test);
	CHECK(error_status, "destroy_data_vectors()");
	error_status = destroy_mcmc_vectors(&mcmc.metropolis);
	CHECK(error_status, "destroy_mcmc_vectors()");

  error_status = destroy_rng(&rng);
	CHECK(error_status, "destroy_rng()");
  return 0;

}
