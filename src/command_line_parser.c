#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argtable2.h>
#include <math.h>

#include "command_line_parser.h"

typedef struct cmd_line_args_s cmd_args_t;

struct cmd_line_args_s
{
  struct arg_int *dim, *train_n, *test_n, *Ns, *Nburn, *maxlag;
  struct arg_dbl *rwsd;
  struct arg_str *datadir, *outdir, *dataset;
  struct arg_file *train_x, *train_y, *test_x, *test_y;
  struct arg_lit  *help;
  struct arg_end *end;
  const char* progname;
};

#define CHECK_POSITIVE(dest, source, progname, argument)\
({\
	if(source < 0)\
	{\
			fprintf(stderr, "%s: invalid argument to option %s. Please enter a positive value!\n", progname, argument);\
      return 1;\
	}else{\
		dest = source;\
	}\
})

static void cmd_setup_argtable(cmd_args_t *args);
static void cmd_setup_default_values(cmd_args_t *args);
static int cmd_extract_args(cmd_args_t *args, cmd_t *cmd);

/*****************************************************************************
 *
 *  cmd_create
 *
 *****************************************************************************/

int cmd_create(cmd_t **pcmd){

  cmd_t *cmd = NULL;

  cmd = (cmd_t *) calloc(1, sizeof(cmd_t));
  assert(cmd);
  if(cmd == NULL)
  {
    printf("calloc(cmd) failed\n");
    exit(1);
  }

  *pcmd = cmd;

  return 0;
}

/*****************************************************************************
 *
 *  cmd_free
 *
 *****************************************************************************/

int cmd_free(cmd_t *cmd){

  assert(cmd);
  free(cmd);

  assert(cmd != NULL);

  return 0;
}

/*****************************************************************************
 *
 *  cmd_parse
 *
 *****************************************************************************/

int cmd_parse(int an, char *av[], cmd_t *cmd){

  cmd_args_t args;
  int nerrors, status;

  assert(cmd);

  args.progname = av[0];
  cmd_setup_argtable(&args);
  void* argtable[] = {  args.dim, args.train_n, args.test_n,
                        args.Ns, args.Nburn, args.rwsd, args.maxlag,
                        args.datadir, args.outdir, args.dataset,
                        args.train_x, args.train_y,
                        args.test_x, args.test_y,
                        args.help, args.end
                      };

  /* verify allocation of argtable */
  if (arg_nullcheck(argtable) != 0)
  {
    /* NULL entries were detected, some allocations must have failed */
    printf("%s: insufficient memory\n", args.progname);
    /* deallocate each non-null entry in argtable[] before return */
    arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));
    exit(0);
  }

  /* setup default values */
  cmd_setup_default_values(&args);

  /* parse arguments */
  nerrors = arg_parse(an, av, argtable);

  /* print help message */
  if (args.help->count > 0)
  {
    printf("\n----------------------------------- Metropolis-MCMC ---------------------------------\n");
    printf("  This program implements the Metropolis Markov Chain Monte Carlo (Metropolis-MCMC)\n");
    printf("  Accepts positive integers in decimal (123), positive real numbers and strings.   \n");
    printf("-------------------------------------------------------------------------------------\n\n");
    printf("Usage: %s", args.progname);
    arg_print_syntax(stdout, argtable,"\n");
    arg_print_glossary(stdout, argtable,"  %-25s %s\n");
    /* deallocate each non-null entry in argtable[] before return */
    arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));
    exit(0);
  }

  /* display errors */
  if (nerrors > 0)
  {
    /* Display the error details contained in the arg_end struct.*/
    arg_print_errors(stdout, args.end, args.progname);
    printf("Try '%s --help' for more information.\n", args.progname);
    arg_freetable(argtable, sizeof(argtable)/sizeof(argtable));
    exit(0);
  }

  /* copy the parsed values to the appropriate program variables */
	status = cmd_extract_args(&args, cmd);
  if (status != 0)
  {
    arg_freetable(argtable, sizeof(argtable)/sizeof(argtable));
    exit(0);
  }

  /* destroy argtable */
  arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));

  return 0;
}

/*****************************************************************************
 *
 *  cmd_print_status
 *
 *****************************************************************************/

int cmd_print_status(cmd_t *cmd){

  assert(cmd);
  printf("--------------------------------- Metropolis-MCMC -------------------------------\n");

	printf("%45s:\t%s\n", "Selected dataset", cmd->dataset);
	printf("%45s:\t%d\n", "Training set dimensionality", cmd->train.dim);
	printf("%45s:\t%d\n", "Number of training data", cmd->train.N );
  printf("%45s:\t%s\n", "Filename of training datapoints", cmd->train.fx);
	printf("%45s:\t%s\n", "Filename of training labels", cmd->train.fy);

	printf("%45s:\t%d\n", "Test set dimensionality", cmd->test.dim);
	printf("%45s:\t%d\n", "Number of test data", cmd->test.N);
  printf("%45s:\t%s\n", "Filename of test datapoints", cmd->test.fx);
	printf("%45s:\t%s\n", "Filename of test labels", cmd->test.fy);

	printf("%45s:\t%d\n", "Number of generated samples (post burn-in)", cmd->Ns);
	printf("%45s:\t%d\n", "Burn in samples", cmd->Nburn);
	printf("%45s:\t%f\n", "Standard deviation in random walk", cmd->rwsd);
	printf("%45s:\t%d\n", "Maximum allowed autocorrelation lag", cmd->maxlag);
  printf("%45s:\t%s\n", "Results Directory", cmd->outdir);
	printf("---------------------------------------------------------------------------------\n");

  return 0;
}

/*****************************************************************************
 *
 *  cmd_setup_argtable
 *
 *****************************************************************************/

static void cmd_setup_argtable(cmd_args_t *args){

  assert(args);
  args->dim = arg_int0("d", "dim", "<int>", "define an integer value for dimensionality (default is 3). Includes 1 for bias.");
  args->train_n = arg_int0(NULL, "train_n", "<int>", "define an integer value for training samples (default is 8000)");
  args->test_n = arg_int0(NULL, "test_n", "<int>", "define an integer value for test samples (default is 2000)");
  args->Ns = arg_int0("s", "samples", "<int>", "define an integer value for generated samples (default is 20000)");
  args->Nburn = arg_int0("b", "burn", "<int>", "define an integer value for burn samples (default is 5000)");
  args->maxlag = arg_int0("a", "maxlag", "<int>", "define an integer value for maximum allowed autocorrelation lag (default is 1)");

  args->rwsd = arg_dbl0("w", "rwsd", "<real>", "define a real value for random walk step size (default is 1.374)");

  args->datadir = arg_str0("l", "datadir", "<str>", "define a directory where the data will be read from (default is data/)");
  args->outdir = arg_str0("o", "outdir", "<str>", "define a directory where the results will be written to (default is out/)");
  args->dataset = arg_str0(NULL, "dataset", "<str>", "define a dataset to be used. Currently supported choices are: 'synthetic', 'infiMnist', 'Flights' (default is 'synthetic')");


  args->train_x = arg_file0(NULL, "train_x", "<file>", "define the filename for train data (default is X_train_10000_3.csv)");
  args->train_y = arg_file0(NULL, "train_y", "<file>", "define the filename for train labels (default is Y_train_10000_3.csv)");
  args->test_x  = arg_file0(NULL, "test_x", "<file>", "define the filename for test data (default is X_test_10000_3.csv)");
  args->test_y  = arg_file0(NULL, "test_y", "<file>", "define the filename for test labels (default is Y_test_10000_3.csv)");

  args->help    = arg_lit0("h", "help", "print this help and exit");

  args->end     = arg_end(20);

}

/*****************************************************************************
 *
 *  cmd_setup_default_values
 *
 *****************************************************************************/

static void cmd_setup_default_values(cmd_args_t *args){

  assert(args);

  args->dim->ival[0] = 2;
  args->train_n->ival[0] = 8000;
  args->test_n->ival[0] = 2000;
  args->Ns->ival[0] = 20000;
  args->Nburn->ival[0] = 5000;
  args->rwsd->dval[0] = 0;
  args->maxlag->ival[0] = (args->Ns->ival[0] / 2)-1;

  args->datadir->sval[0] = "./data/synthetic/8000_2";
  args->dataset->sval[0] = "synthetic";
  args->train_x->filename[0] = "X_train_10000_3.csv";
  args->train_y->filename[0] = "Y_train_10000_3.csv";
  args->test_x->filename[0] = "X_test_10000_3.csv";
  args->test_y->filename[0] = "Y_test_10000_3.csv";

}

/*****************************************************************************
 *
 *  cmd_extract_args
 *
 *****************************************************************************/

static int cmd_extract_args(cmd_args_t *args, cmd_t *cmd){

  assert(args);
  assert(cmd);
	/* Checks each numerical entry if positive otherwise reports invalid and exits
	 * If it is positive, append the value to the appropriate variable
	 */
	CHECK_POSITIVE(cmd->dim, args->dim->ival[0], args->progname, "-d|--dim=<int>");
	CHECK_POSITIVE(cmd->Ns, args->Ns->ival[0], args->progname, "-s|--samples=<int>");
	CHECK_POSITIVE(cmd->Nburn, args->Nburn->ival[0], args->progname, "-b|--burn=<int>");
  CHECK_POSITIVE(cmd->maxlag, args->maxlag->ival[0], args->progname, "-l|--maxlag=<int>");
  if(cmd->maxlag > (cmd->Ns/2) -1)
  {
    printf("Warning: Maximum autocorrelation lag set to %d exceeds %d. ",
            cmd->maxlag, (cmd->Ns/2) - 1);
    cmd->maxlag = (cmd->Ns/2) - 1;
    printf("Value casted to %d. ", cmd->maxlag);
  }
	CHECK_POSITIVE(cmd->rwsd, args->rwsd->dval[0], args->progname, "-w|--rwsd=<int>");
  if(cmd->rwsd == 0)
  {
    cmd->rwsd = 2.38 / sqrt(cmd->dim+1);
  }

  CHECK_POSITIVE(cmd->train.dim, args->dim->ival[0], args->progname, "-d|--dim=<int>");
	CHECK_POSITIVE(cmd->train.N, args->train_n->ival[0], args->progname, "--train_n=<int>");
  sprintf(cmd->train.datadir, "%s", args->datadir->sval[0]);
	sprintf(cmd->train.fx, "%s/%s", args->datadir->sval[0], args->train_x->filename[0]);
	sprintf(cmd->train.fy, "%s/%s", args->datadir->sval[0], args->train_y->filename[0]);


  CHECK_POSITIVE(cmd->test.dim, args->dim->ival[0], args->progname, "-d|--dim=<int>");
	CHECK_POSITIVE(cmd->test.N, args->test_n->ival[0], args->progname, "--test_n=<int>");
  sprintf(cmd->test.datadir, "%s", args->datadir->sval[0]);
  sprintf(cmd->test.fx, "%s/%s", args->datadir->sval[0], args->test_x->filename[0]);
	sprintf(cmd->test.fy, "%s/%s", args->datadir->sval[0], args->test_y->filename[0]);

  sprintf(cmd->dataset, "%s", args->dataset->sval[0]);
  // sprintf(cmd->outdir, "%s/%s/%d_%d",
  //         args->outdir->sval[0], cmd->dataset,
  //         args->train_n->ival[0], args->dim->ival[0]
  //        );
  sprintf(cmd->outdir, "%s/%d_%d",
          args->outdir->sval[0],
          args->train_n->ival[0], args->dim->ival[0]
         );
	return 0;
}
