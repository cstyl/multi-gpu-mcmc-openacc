#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "definitions.h"

#include "pe.h"
#include "runtime.h"

#include "chain.h"
#include "metropolis.h"
#include "autocorrelation.h"
#include "effective_sample_size.h"
#include "inference.h"
#include "timer.h"

#include "mcmc.h"

typedef struct mcmc_s mcmc_t;

struct mcmc_s{
  pe_t *pe;        /* Parallel Environment */
  rt_t *rt;        /* Run time input handler */
  ch_t *burn;      /* Chain of burn-in samples */
  ch_t *chain;     /* Chain of post burn-in samples */
  met_t *met;      /* Metropolis MCMC */
  acr_t *acr;      /* Autocorrelation structure */
  ess_t *ess;      /* Effective Sample Size */
  infr_t  *infr;   /* Inference data structure */
};

static int mcmc_rt(mcmc_t *mcmc);

/*****************************************************************************
 *
 *  mcmc_run
 *
 *****************************************************************************/

 void mcmc_run(const char *inputfile){

   mcmc_t *mcmc = NULL;
   MPI_Comm comm;
   char mc_case[BUFSIZ];

   mcmc = (mcmc_t*) calloc(1, sizeof(mcmc_t));
   assert(mcmc);

   pe_create(MPI_COMM_WORLD, PE_VERBOSE, &mcmc->pe);
   pe_mpi_comm(mcmc->pe, &comm);

   rt_create(mcmc->pe, &mcmc->rt);
   rt_read_input_file(mcmc->rt, inputfile);
   rt_info(mcmc->rt);

   mcmc_rt(mcmc);

   if(mcmc->met)
   {
     TIMER_start(TIMER_MCMC_METROPOLIS);
     /* Burn in */
     met_init(mcmc->pe, mcmc->met);

     TIMER_start(TIMER_BURN_IN);
     met_run(mcmc->pe, mcmc->met);
     TIMER_stop(TIMER_BURN_IN);
     /* Post burn-in */
     met_chain_set(mcmc->met, mcmc->chain);
     met_init_post_burn(mcmc->pe, mcmc->met);

     TIMER_start(TIMER_POST_BURN_IN);
     met_run(mcmc->pe, mcmc->met);
     TIMER_stop(TIMER_POST_BURN_IN);
     /* Clean up */
     met_free(mcmc->met);
     TIMER_stop(TIMER_MCMC_METROPOLIS);
   }

   acr_compute(mcmc->acr);
   acr_print_acr(mcmc->pe, mcmc->acr);

   ess_compute(mcmc->ess);
   ess_print_ess(mcmc->pe, mcmc->ess);

   /* Write output files */
   if(pe_mpi_rank(mcmc->pe) == 0)
   {
     TIMER_start(TIMER_WRITE_FILES);
     ch_write_files(mcmc->burn, "burn");
     ch_write_files(mcmc->chain, "postburn");
     acr_write_acr(mcmc->acr);
     TIMER_stop(TIMER_WRITE_FILES);
   }

   if(mcmc->infr)
   {
     TIMER_start(TIMER_INFERENCE);

     infr_init(mcmc->pe, mcmc->infr);

     infr_mc_case(mcmc->infr, mc_case);
     if(strcmp(mc_case, "logistic_regression") == 0)
     {
       infr_mc_integration_lr(mcmc->infr);
       infr_print(mcmc->pe, mcmc->infr);
     }

     infr_free(mcmc->infr);

     TIMER_stop(TIMER_INFERENCE);
   }

   TIMER_stop(TIMER_TOTAL);
   TIMER_statistics();

   acr_free(mcmc->acr);
   ess_free(mcmc->ess);
   ch_free(mcmc->burn);
   ch_free(mcmc->chain);

   rt_free(mcmc->rt);
   pe_free(mcmc->pe);

   return;
 }

 /*****************************************************************************
 *
 *  mcmc_rt
 *
 *  Digest the run-time arguments for different parts of the code.
 *  Allocate memory where needed.
 *
 *****************************************************************************/

 static int mcmc_rt(mcmc_t *mcmc){

   pe_t  *pe  = NULL;
   rt_t  *rt  = NULL;
   char algorithm_value[BUFSIZ];

   assert(mcmc);

   TIMER_init(mcmc->pe);
   TIMER_start(TIMER_TOTAL);

   TIMER_start(TIMER_RUNTIME_SETUP);

   TIMER_start(TIMER_ACC_INIT);
   #pragma acc init device_type(acc_device_not_host)
   TIMER_stop(TIMER_ACC_INIT);

   pe = mcmc->pe;
   rt = mcmc->rt;

   sprintf(algorithm_value, "%s", ALGORITHM_DEFAULT);


   ch_create(pe, &mcmc->burn);
   ch_init_burn_rt(rt, mcmc->burn);
   ch_burn_info(pe, mcmc->burn);

   ch_create(pe, &mcmc->chain);
   ch_init_chain_rt(rt, mcmc->chain);
   ch_chain_info(pe, mcmc->chain);

   rt_string_parameter(rt, "mcmc_algorithm", algorithm_value, BUFSIZ);
   if(strcmp(algorithm_value, "metropolis") == 0)
   {
     met_create(pe, mcmc->burn, &mcmc->met);
     met_init_rt(pe, rt, mcmc->met);
     met_info_rt(pe, mcmc->met);
   }

   acr_create(pe, mcmc->chain, &mcmc->acr);
   acr_init_rt(rt, mcmc->chain, mcmc->acr);
   acr_info(pe, mcmc->acr);

   ess_create(pe, mcmc->acr, &mcmc->ess);
   ess_init_rt(rt, mcmc->ess);
   ess_info(pe, mcmc->ess);

   if(rt_switch(rt, "inference"))
   {
     infr_create(pe, mcmc->chain, &mcmc->infr);
     infr_init_rt(pe, rt, mcmc->infr);
     infr_info(pe, mcmc->infr);
   }

   TIMER_stop(TIMER_RUNTIME_SETUP);

   return 0;
 }
