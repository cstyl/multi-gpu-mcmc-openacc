#ifndef __FLAGS_H__
#define __FLAGS_H__

enum ERROR_FLAGS{
  SIM_FAILURE=-1,							/**< Error code returned when execuion is incorrect.*/
	SIM_SUCCESS=0,  						/**< Error code returned when the execution was correct.*/
	SIM_HELP=1,				          /**< Error code returned when --help is set as command line argument.*/
  SIM_INVALID=2,               /**< Error code returned when an invalid command line argument is passed.*/
  SIM_ZERO_MEM_FAILURE=3,			/**< Error code returned when trying to allocate memory with negative or zero value.*/
	SIM_MEM_FAILURE=4,					/**< Error code returned when there is a failure in allocating memory.*/
};


#endif //__FLAGS_H__
