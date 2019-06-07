#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "structs.h"
#include "flags.h"
#include "macros.h"
#include "util.h"
#include "rng.h"
#include "mcmc.h"

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

	/* Setup RNG. We add +1 so that generator can be used at the acceptance step */
  rng.count = mcmc.metropolis.dim + 1;
  error_status = setup_rng(&rng);
  CHECK(error_status, "setup_rng()");
  // Set heuristically as proposed in Efficient Metropolis Jumping rules by Gelman et al.
  mcmc.metropolis.rwsd = 2.38 / sqrt(mcmc.metropolis.dim);
  /* Initialise samples */
  mcmc_init(&mcmc.metropolis, &mcmc.train, &rng, 0);
	/* Run mcmc */
  mcmc_run(&mcmc.metropolis, &mcmc.train, &rng);

	error_status = destroy_data_vectors(&mcmc.train, &mcmc.test);
	CHECK(error_status, "destroy_data_vectors()");
	error_status = destroy_mcmc_vectors(&mcmc.metropolis);
	CHECK(error_status, "destroy_mcmc_vectors()");

  error_status = destroy_rng(&rng);
	CHECK(error_status, "destroy_rng()");
  return 0;

}
