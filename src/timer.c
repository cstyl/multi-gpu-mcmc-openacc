#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "timer.h"

/* Use clock() only as a last resort in serial */

#ifdef _OPENMP
#include <omp.h>
#else
#include <time.h>
#define omp_get_wtime() ((double) clock()*(1.0/CLOCKS_PER_SEC))
#define omp_get_wtick() (1.0/CLOCKS_PER_SEC)
#endif

struct timer_struct {
  double          t_start;
  double          t_sum;
  double          t_max;
  double          t_min;
  unsigned int    active;
  unsigned int    nsteps;
};

static struct timer_struct timer[TIMER_NTIMERS];

static const char * timer_name[] = {"Total",
                                    "MCMC Setup",
                                    "MCMC Sampler",
                                    "MCMC Statistics",
                                    "MCMC Inference",
                                    "MCMC Dissasemble",
                                    "Burn-in Period",
                                    "Post Burn-in Period",
                                    "Sampler Step",
                                    "Likelihood",
                                    "Prior Probability",
                                    "Proposal Step"
};

double dmin(const double a, const double b);
double dmax(const double a, const double b);

/****************************************************************************
 *
 *  TIMER_init
 *
 *  Make sure everything is set to go.
 *
 ****************************************************************************/

int TIMER_init(void) {

  int n;

  for (n = 0; n < TIMER_NTIMERS; n++) {
    timer[n].t_sum  = 0.0;
    timer[n].t_max  = FLT_MIN;
    timer[n].t_min  = FLT_MAX;
    timer[n].active = 0;
    timer[n].nsteps = 0;
  }

  return 0;
}


/****************************************************************************
 *
 *  TIMER_start
 *
 *  Start timer for the specified timer.
 *
 ****************************************************************************/

void TIMER_start(const int t_id) {

  timer[t_id].t_start = omp_get_wtime();
  timer[t_id].active  = 1;
  timer[t_id].nsteps += 1;

  return;
}


/****************************************************************************
 *
 *  TIMER_stop_timer
 *
 *  Stop the specified timer and add the elapsed time to the total.
 *
 ****************************************************************************/

void TIMER_stop(const int t_id) {

  double t_elapse;

  if (timer[t_id].active) {

    t_elapse = omp_get_wtime() - timer[t_id].t_start;

    timer[t_id].t_sum += t_elapse;
    timer[t_id].t_max  = dmax(timer[t_id].t_max, t_elapse);
    timer[t_id].t_min  = dmin(timer[t_id].t_min, t_elapse);
    timer[t_id].active = 0;
  }

  return;
}

/*****************************************************************************
 *
 *  TIMER_statistics
 *
 *  Print a digestable overview of the time statistics.
 *
 *****************************************************************************/

void TIMER_statistics() {

  int    n;
  double t_min, t_max, t_sum;
  double r;

  r = omp_get_wtick();

  fprintf(stdout, "\nTimer resolution: %g second\n", r);
  fprintf(stdout, "\nTimer statistics\n");
  fprintf(stdout, "%20s: %10s %10s %10s\n", "Section", "  tmin", "  tmax", " total");

  for (n = 0; n < TIMER_NTIMERS; n++) {

    /* Report the stats for active timers */

    if (timer[n].nsteps != 0) {

      t_min = timer[n].t_min;
      t_max = timer[n].t_max;
      t_sum = timer[n].t_sum;

      fprintf(stdout, "%20s: %10.3f %10.3f %10.3f %10.6f", timer_name[n],
	   t_min, t_max, t_sum, t_sum/(double) timer[n].nsteps);
      fprintf(stdout, " (%d call%s)\n", timer[n].nsteps, timer[n].nsteps > 1 ? "s" : "");
    }
  }

  return;
}

double dmin(const double a, const double b){
  return ((a < b) ? a : b);
}

double dmax(const double a, const double b){
  return ((a > b) ? a : b);
}
