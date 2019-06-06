#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "structs.h"
#include "flags.h"
#include "macros.h"
#include "util.h"

int main(int argc, char *argv[])
{
  mcmc mcmc;
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
	/* Initialise samples */
	/* Run mcmc */
	error_status = destroy_data_vectors(&mcmc.train, &mcmc.test);
	CHECK(error_status, "destroy_data_vectors()");
	error_status = destroy_mcmc_vectors(&mcmc.metropolis);
	CHECK(error_status, "destroy_data_vectors()");

  return 0;

}
