#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <argtable2.h>

#include "util.h"
#include "macros.h"
#include "flags.h"

typedef struct cmd_line_args
{
  struct arg_int *dim, *train_n, *test_n, *Ns, *Nburn;
  struct arg_dbl *rwsd;
  struct arg_str *datadir;
  struct arg_file *train_x, *train_y, *test_x, *test_y;
  struct arg_lit  *help;
  struct arg_end *end;
  const char* progname;
} cmd_args;

static void setup_argtable(cmd_args *args);
static void setup_default_values(cmd_args *args);
static int extract_args(cmd_args *args, mcmc *mcmc);

int parse_args(int an, char *av[], mcmc *mcmc)
{
  cmd_args args;
  int nerrors, status;
  args.progname = av[0];

  setup_argtable(&args);
  void* argtable[] = {  args.dim, args.train_n, args.test_n,
                        args.Ns, args.Nburn, args.rwsd,
                        args.datadir,
                        args.train_x, args.train_y,
                        args.test_x, args.test_y,
                        args.help, args.end
                      };

  /* verify allocation of argtable */
  if (arg_nullcheck(argtable) != 0)
  {
    printf("In null check setup\n");
    /* NULL entries were detected, some allocations must have failed */
    printf("%s: insufficient memory\n", args.progname);
    /* deallocate each non-null entry in argtable[] before return */
    arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));
    return SIM_FAILURE;
  }

  /* setup default values */
  setup_default_values(&args);

  /* parse arguments */
  nerrors = arg_parse(an, av, argtable);

  /* print help message */
  if (args.help->count > 0)
  {
    printf("--------------------------------- Metropolis-MCMC -------------------------------\n");
    printf("This program implements the Metropolis Markov Chain Monte Carlo (Metropolis-MCMC)\n");
    printf("Accepts positive integers in decimal (123), positive real numbers and strings.   \n");
    printf("---------------------------------------------------------------------------------\n");
    printf("Usage: %s", args.progname);
    arg_print_syntax(stdout, argtable,"\n");
    arg_print_glossary(stdout, argtable,"  %-25s %s\n");
    /* deallocate each non-null entry in argtable[] before return */
    arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));
    return SIM_HELP;
  }

  /* display errors */
  if (nerrors > 0)
  {
    /* Display the error details contained in the arg_end struct.*/
    arg_print_errors(stdout, args.end, args.progname);
    printf("Try '%s --help' for more information.\n", args.progname);
    arg_freetable(argtable, sizeof(argtable)/sizeof(argtable));
    return SIM_INVALID;
  }

  /* copy the parsed values to the appropriate program variables */
	status = extract_args(&args, mcmc);
  if (status == SIM_INVALID)
  {
    arg_freetable(argtable, sizeof(argtable)/sizeof(argtable));
    return SIM_INVALID;
  }

  /* destroy argtable */
  arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));

  return SIM_SUCCESS;
}

void print_parameters(mcmc mcmc)
{
	printf("--------------------------------- Metropolis-MCMC -------------------------------\n");

	printf("%45s:\t%d\n", "Training set dimensionality", mcmc.train.dim);
	printf("%45s:\t%d\n", "Number of training data", mcmc.train.Nd );
	printf("%45s:\t%d\n", "Test set dimensionality", mcmc.test.dim);
	printf("%45s:\t%d\n", "Number of test data", mcmc.test.Nd);

	printf("%45s:\t%d\n", "Number of generated samples (post burn-in)", mcmc.metropolis.Ns);
	printf("%45s:\t%d\n", "Burn in samples", mcmc.metropolis.Nburn);
	printf("%45s:\t%f\n", "Standard deviation in random walk", mcmc.metropolis.rwsd);

	printf("%45s:\t%s\n", "Filename of training datapoints", mcmc.train.fx);
	printf("%45s:\t%s\n", "Filename of training labels", mcmc.train.fy);
	printf("%45s:\t%s\n", "Filename of test datapoints", mcmc.test.fx);
	printf("%45s:\t%s\n", "Filename of test labels", mcmc.test.fy);

	printf("---------------------------------------------------------------------------------\n");
}

static void setup_argtable(cmd_args *args)
{

  args->dim = arg_int0("d", "dim", "<int>", "define an integer value for dimensionality (default is 3)");
  args->train_n = arg_int0(NULL, "train_n", "<int>", "define an integer value for training samples (default is 8000)");
  args->test_n = arg_int0(NULL, "test_n", "<int>", "define an integer value for test samples (default is 2000)");
  args->Ns = arg_int0("s", "samples", "<int>", "define an integer value for generated samples (default is 20000)");
  args->Nburn = arg_int0("b", "burn", "<int>", "define an integer value for burn samples (default is 5000)");

  args->rwsd = arg_dbl0("w", "rwsd", "<double>", "define a real value for random walk step size (default is TBD)");

  args->datadir = arg_str0("l", "datadir", "<str>", "define a directory where the data will be read from (default is data/)");

  args->train_x = arg_file0(NULL, "train_x", "<file>", "define the filename for train data (default is X_train_10000_3.csv)");
  args->train_y = arg_file0(NULL, "train_y", "<file>", "define the filename for train labels (default is Y_train_10000_3.csv)");
  args->test_x  = arg_file0(NULL, "test_x", "<file>", "define the filename for test data (default is X_test_10000_3.csv)");
  args->test_y  = arg_file0(NULL, "test_y", "<file>", "define the filename for test labels (default is Y_test_10000_3.csv)");

  args->help    = arg_lit0("h", "help", "print this help and exit");

  args->end     = arg_end(20);

}

static void setup_default_values(cmd_args *args)
{
  args->dim->ival[0] = 3;
  args->train_n->ival[0] = 8000;
  args->test_n->ival[0] = 2000;
  args->Ns->ival[0] = 20000;
  args->Nburn->ival[0] = 5000;
  args->rwsd->dval[0] = 0.345;
  args->datadir->sval[0] = "./data";
  args->train_x->filename[0] = "X_train_10000_3.csv";
  args->train_y->filename[0] = "Y_train_10000_3.csv";
  args->test_x->filename[0] = "X_test_10000_3.csv";
  args->test_y->filename[0] = "Y_test_10000_3.csv";
}

static int extract_args(cmd_args *args, mcmc *mcmc)
{
	/* Checks each numerical entry if positive otherwise reports invalid and exits
	 * If it is positive, append the value to the appropriate variable
	 */
	CHECK_POSITIVE(mcmc->train.dim, args->dim->ival[0], args->progname, "-d|--dim=<int>");
	CHECK_POSITIVE(mcmc->train.Nd, args->train_n->ival[0], args->progname, "--train_n=<int>");
	CHECK_POSITIVE(mcmc->test.dim, args->dim->ival[0], args->progname, "-d|--dim=<int>");
	CHECK_POSITIVE(mcmc->test.Nd, args->test_n->ival[0], args->progname, "--test_n=<int>");

	CHECK_POSITIVE(mcmc->metropolis.Ns, args->Ns->ival[0], args->progname, "-s|--samples=<int>");
	CHECK_POSITIVE(mcmc->metropolis.Nburn, args->Nburn->ival[0], args->progname, "-b|--burn=<int>");
	CHECK_POSITIVE(mcmc->metropolis.dim, args->dim->ival[0], args->progname, "-d|--dim=<int>");
	CHECK_POSITIVE(mcmc->metropolis.rwsd, args->rwsd->dval[0], args->progname, "-w|--rwsd=<int>");

	/* Append directory to each filename */
	sprintf(mcmc->train.fx, "%s/%s", args->datadir->sval[0], args->train_x->filename[0]);
	sprintf(mcmc->train.fy, "%s/%s", args->datadir->sval[0], args->train_y->filename[0]);
	sprintf(mcmc->test.fx, "%s/%s", args->datadir->sval[0], args->test_x->filename[0]);
	sprintf(mcmc->test.fy, "%s/%s", args->datadir->sval[0], args->test_y->filename[0]);

	return SIM_SUCCESS;
}
