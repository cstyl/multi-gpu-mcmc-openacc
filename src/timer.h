/*****************************************************************************
 *
 *  timer.h
 *
 *  Note that the number of timers, their ids, and their descriptive
 *  names must match here.
 *
 *  $Id: timer.h,v 1.4 2010-10-15 12:40:03 kevin Exp $
 *
 *  Edinburgh Soft Matter and Statistical Physics Group and
 *  Edinburgh Parallel Computing Centre
 *
 *  (c) 2010-2017 The University of Edinburgh
 *
 *  Contributing authors:
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *
 *****************************************************************************/

#ifndef __MCMC_TIMER_H__
#define __MCMC_TIMER_H__

#include "pe.h"

int TIMER_init(pe_t * pe);
void TIMER_start(const int);
void TIMER_stop(const int);
void TIMER_statistics(void);

enum timer_id {TIMER_TOTAL = 0,
               TIMER_MCMC_SETUP,
               TIMER_MCMC_SAMPLER,
               TIMER_MCMC_STATISTICS,
               TIMER_MCMC_INFERENCE,
               TIMER_MCMC_DISSASEMBLE,
               TIMER_BURN_IN,
               TIMER_POST_BURN_IN,
               TIMER_SAMPLER_STEP,
               TIMER_LIKELIHOOD,
               TIMER_PRIOR,
               TIMER_PROPOSE,
	             TIMER_NTIMERS /* This must be the last entry */
};

#endif // __MCMC_TIMER_H__
