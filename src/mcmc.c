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

// #include "timer.h"

#include "mcmc.h"

typedef struct mcmc_s mcmc_t;
// struct mcmc_s{
//   pe_t    *pe;         /* Parallel Environment */
//   rt_t    *rt;         /* Run time input handler */
//   met_t   *met;        /* Metropolis algorithm structure */
//   chain_t *burn;       /* Chain structure for burn-in samples */
//   chain_t *chain;      /* Chain structure for post burn-in samples */
//   stats_t *stats;      /* Statistics data structure */
//   infr_t  *infr;       /* Inference data structure */
// };
struct mcmc_s{
  pe_t *pe;        /* Parallel Environment */
  rt_t *rt;        /* Run time input handler */
  ch_t *burn;      /* Chain of burn-in samples */
  ch_t *chain;     /* Chain of post burn-in samples */
  met_t *met;      /* Metropolis MCMC */
};

static const char ALGORITHM_DEFAULT[BUFSIZ] = "metropolis";

static int mcmc_rt(mcmc_t *mcmc);

/*****************************************************************************
 *
 *  mcmc_run
 *
 *****************************************************************************/

 void mcmc_run(const char *inputfile){

   mcmc_t *mcmc = NULL;
   MPI_Comm comm;

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
     met_init(mcmc->pe, mcmc->met);
     met_run(mcmc->pe, mcmc->met);
   }

   if(mcmc->met) met_free(mcmc->met);
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
     met_create(pe, mcmc->burn, mcmc->chain, &mcmc->met);
     met_init_rt(pe, rt, mcmc->met);
     met_info_rt(pe, mcmc->met);
   }


   return 0;
 }
