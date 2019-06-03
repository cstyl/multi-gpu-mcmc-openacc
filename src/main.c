#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "structs.h"
#include "cmd_line_parser.h"
#define MAIN_SUCCESS 0

#define CHECK_ERROR(error_status, error_func)\
({\
	if(error_status!=MAIN_SUCCESS)\
	{\
			fprintf(stderr, "ERROR::%s:%d:%s: Function %s returned error %d!\n",\
						__FILE__, __LINE__, __func__, error_func, error_status);\
      return(error_status);\
	}\
})

int main(int argc, char *argv[])
{
  mcmc mcmc;
  int error_status = MAIN_SUCCESS;

  error_status = parse_args(argc, argv, &mcmc);
  CHECK_ERROR(error_status, "parse_args()");

  return 0;

}
