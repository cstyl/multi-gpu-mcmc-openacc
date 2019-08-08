/*****************************************************************************
 *
 *  timer.c
 *
 *  Performance timer routines.
 *
 *  There are a number of separate 'timers', each of which can
 *  be started, and stopped, independently.
 *
 *  $Id: timer.c,v 1.5 2010-10-15 12:40:03 kevin Exp $
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

#include <assert.h>
#include <time.h>
#include <float.h>

#include "pe.h"
#include "timer.h"

struct timer_struct {
  double          t_start;
  double          t_sum;
  double          t_max;
  double          t_min;
  unsigned int    active;
  unsigned int    nsteps;
};

static pe_t * pe_stat = NULL;
static struct timer_struct timer[TIMER_NTIMERS];

static const char * timer_name[] = {"Total",
                                    "Runtime Setup",
                                    "MCMC Metropolis",
                                    "MCMC Metropolis Init",
                                    "MCMC Burn-in",
                                    "MCMC Post Burn-in",
                                    "MCMC Proposal Kernel",
                                    "MCMC Evaluation Step",
                                    "MCMC Acceptance Step",
                                    "Likelihood",
                                    "MatVec Mult Kernel",
                                    "Reduction Kernel",
                                    "Prior",
                                    "Sampler Step",
                                    "Autocorrelation",
                                    "Effective Sample Size",
                                    "Inference",
                                    "MC Integration",
                                    "Logistic Regression",
                                    "Load Training Set",
                                    "Load Test Set",
                                    "Write Output Files"
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

int TIMER_init(pe_t * pe) {

  int n;

  pe_stat = pe;

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

  timer[t_id].t_start = MPI_Wtime();
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

    t_elapse = MPI_Wtime() - timer[t_id].t_start;

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

  MPI_Comm comm;

  assert(pe_stat);
  r = MPI_Wtick();

  pe_mpi_comm(pe_stat, &comm);
  pe_info(pe_stat, "\nTimer resolution: %g second\n", r);
  pe_info(pe_stat, "\nTimer statistics\n");
  pe_info(pe_stat, "%25s: %10s %10s %10s\n", "Section", "  tmin", "  tmax", " total");

  for (n = 0; n < TIMER_NTIMERS; n++) {

    /* Report the stats for active timers */

    if (timer[n].nsteps != 0) {

      t_min = timer[n].t_min;
      t_max = timer[n].t_max;
      t_sum = timer[n].t_sum;

      MPI_Reduce(&(timer[n].t_min), &t_min, 1, MPI_DOUBLE, MPI_MIN, 0, comm);
      MPI_Reduce(&(timer[n].t_max), &t_max, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
      MPI_Reduce(&(timer[n].t_sum), &t_sum, 1, MPI_DOUBLE, MPI_SUM, 0, comm);

      t_sum /= pe_mpi_size(pe_stat);

      pe_info(pe_stat, "%25s: %10.3f %10.3f %10.3f\t%15.6f", timer_name[n],
	                      t_min, t_max, t_sum, t_sum/(double) timer[n].nsteps);
      pe_info(pe_stat, " (%d call%s)\n", timer[n].nsteps, timer[n].nsteps > 1 ? "s" : "");
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
