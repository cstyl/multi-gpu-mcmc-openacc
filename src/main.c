#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "structs.h"
#include "flags.h"
#include "util.h"

#define CHECK_ERROR(error_status, error_func)\
({\
	if(error_status!=SIM_SUCCESS)\
	{\
		if((error_status==SIM_HELP) || (error_status==SIM_INVALID))\
		{\
				return(SIM_SUCCESS);\
		}else{\
			fprintf(stderr, "ERROR::%s:%d:%s: Function %s returned error %d!\n",\
						__FILE__, __LINE__, __func__, error_func, error_status);\
      return(error_status);\
		}\
	}\
})

int main(int argc, char *argv[])
{
  mcmc mcmc;
  int error_status = SIM_SUCCESS;

  error_status = parse_args(argc, argv, &mcmc);
  CHECK_ERROR(error_status, "parse_args()");
	print_parameters(mcmc);

	error_status = allocate_data_vectors(&mcmc.train, &mcmc.test);
	CHECK_ERROR(error_status, "parse_args()");
	error_status = allocate_mcmc_vectors(&mcmc.metropolis);
	CHECK_ERROR(error_status, "parse_args()");


	error_status = destroy_data_vectors(&mcmc.train, &mcmc.test);
	CHECK_ERROR(error_status, "parse_args()");
	error_status = destroy_mcmc_vectors(&mcmc.metropolis);
	CHECK_ERROR(error_status, "parse_args()");

  return 0;

}
