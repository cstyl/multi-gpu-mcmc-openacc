/*****************************************************************************
 *
 *  pe.h
 *
 *  Edinburgh Soft Matter and Statistical Physics Group and
 *  Edinburgh Parallel Computing Centre
 *
 *  (c) 2010-2017 The University of Edinburgh
 *
 *  Contribtuing authors:
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *
 *****************************************************************************/

#ifndef __PE_H__
#define __PE_H__

#include "../version.h"
#include <mpi.h>

typedef struct pe_s pe_t;

typedef enum {PE_QUIET = 0, PE_VERBOSE, PE_OPTION_MAX} pe_enum_t;

int pe_create(MPI_Comm parent, pe_enum_t flag, pe_t ** ppe);
int pe_free(pe_t * pe);
int pe_retain(pe_t * pe);
int pe_set(pe_t * pe, pe_enum_t option);
int pe_message(pe_t * pe);
int pe_mpi_comm(pe_t * pe, MPI_Comm * comm);
int pe_mpi_rank(pe_t * pe);
int pe_mpi_size(pe_t * pe);
int pe_subdirectory(pe_t * pe, char * name);
int pe_subdirectory_set(pe_t * pe, const char * name);
int pe_info(pe_t * pe, const char * fmt, ...);
int pe_fatal(pe_t * pe, const char * fmt, ...);
int pe_verbose(pe_t * pe, const char * fmt, ...);

#endif
