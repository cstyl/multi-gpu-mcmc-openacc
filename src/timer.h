#ifndef __MCMC_TIMER_H__
#define __MCMC_TIMER_H__

int TIMER_init(void);
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