/*****************************************************************************
 *
 *  ran.h
 *
 *  Edinburgh Soft Matter and Statistical Physics Group and
 *  Edinburgh Parallel Computing Centre
 *
 *  (c) 2014-2016 The University of Edinburgh
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *
 *****************************************************************************/

#ifndef __RAN_H__
#define __RAN_H__

#include "pe.h"
#include "runtime.h"

int ran_init(pe_t * pe);
int ran_init_rt(pe_t * pe, rt_t * rt);
int ran_init_seed(pe_t * pe, int scalar_seed);

double ran_parallel_gaussian(void);
double ran_parallel_uniform(void);
double ran_serial_uniform(void);
double ran_serial_gaussian(void);

#endif // __RAN_H__
