#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "definitions.h"
#include "pe.h"
#include "prior.h"
#include "tests.h"

int test_prior_suite(void){

  pe_t *pe = NULL;

  pe_create(MPI_COMM_WORLD, PE_QUIET, &pe);
  assert(pe);
  test_assert(1);

  int dim = 3;
  precision sample[3] = {-10.0000000000000000, 5.0000000000000000, 10.0000000000000000};
  precision prior_act;

  prior_act = pr_log_prob(sample, dim);
  test_assert(fabs(prior_act - -37.2955920057748003) < TEST_PRECISION_TOLERANCE);

  pe_info(pe, "PASS\t./unit/test_prior\n");
  pe_free(pe);

  return 0;
}
