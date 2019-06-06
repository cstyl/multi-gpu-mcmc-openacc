#ifndef __MACROS_H__
#define __MACROS_H__

#define CHECK(error_status, error_func)\
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

#define CHECK_ERROR(error_status, error_func)\
({\
	if(error_status!=SIM_SUCCESS)\
	{\
			fprintf(stderr, "ERROR::%s:%d:%s: Function %s returned error %d!\n",\
						__FILE__, __LINE__, __func__, error_func, error_status);\
		return(error_status);\
	}\
})

#define CONVERT(in, type_t, out, pos)\
({\
  if(!strcmp(type_t,"int"))\
  {\
    *((int *)out + pos) = atoi(in);\
  }else if(!strcmp(type_t,"precision"))\
  {\
    *((precision *)out + pos) = (precision)atof(in);\
  }else{\
    fprintf(stderr, "ERROR::%s:%d:%s: Invalid type argument \"%s\"!\n",\
            __FILE__, __LINE__,__func__, type_t);\
    return(SIM_INVALID_TYPE);\
  }\
})

#define CHECK_FILE_OPEN(fp, filename) \
({\
  if(fp == NULL)\
  {\
  fprintf(stderr, "ERROR::%s:%d:%s: Opening %s file failed!\n", __FILE__, __LINE__, __func__, filename);\
  return(SIM_FILE_FAILURE);\
  }\
})

#define CHECK_MALLOC(ptr) \
({\
	if(ptr == NULL)\
	{\
		fprintf(stderr, "ERROR::%s:%d:%s: Memory allocation did not complete successfully!\n", __FILE__, __LINE__,__func__);\
		return(SIM_MEM_FAILURE);\
	}\
})

#define CHECK_VALID_MEM_REQUEST(size) \
({\
	if(size == 0)\
	{\
		fprintf(stderr, "ERROR::%s:%d:%s: Cannot allocate memory with negative or zero elements!\n", __FILE__, __LINE__,__func__);\
		return(SIM_ZERO_MEM_FAILURE);\
	}\
})

#define FREE(ptr) \
({\
	if(ptr != NULL)\
	{\
		free(ptr);\
		ptr = NULL;\
	}\
})

#define CHECK_POSITIVE(dest, source, progname, argument)\
({\
	if(source < 0)\
	{\
			fprintf(stderr, "%s: invalid argument to option %s. Please enter a positive value!\n", progname, argument);\
      return SIM_INVALID;\
	}else{\
		dest = source;\
	}\
})

#endif  //__MACROS_H__
