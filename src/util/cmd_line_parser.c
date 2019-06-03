#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <argtable2.h>

#include "cmd_line_parser.h"

#define PAR_ERROR -1
#define PAR_SUCCESS 0
#define PAR_HELP 1

#define CHECK_ERROR(error_status, error_func)\
({\
	if(error_status!=PAR_SUCCESS)\
	{\
			fprintf(stderr, "ERROR::%s:%d:%s: Function %s returned error %d!\n",\
						__FILE__, __LINE__, __func__, error_func, error_status);\
      return(error_status);\
	}\
})

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
static void extract_args(cmd_args *args, mcmc *mcmc);

int parse_args(int an, char *av[], mcmc *mcmc)
{
  cmd_args args;
  int nerrors;
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
    return PAR_ERROR;
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
    return PAR_HELP;
  }

  /* display errors */
  if (nerrors > 0)
  {
    /* Display the error details contained in the arg_end struct.*/
    arg_print_errors(stdout, args.end, args.progname);
    printf("Try '%s --help' for more information.\n", args.progname);
    arg_freetable(argtable, sizeof(argtable)/sizeof(argtable));
    return PAR_ERROR;
  }

  /* display message when no arguments are provided */
  if (an == 1)
  {
    printf("Try '%s --help' for more information.\n", args.progname);
    /* deallocate each non-null entry in argtable[] before return */
    arg_freetable(argtable, sizeof(argtable)/sizeof(argtable));
    return PAR_HELP;
  }

  /* copy the parsed values to the appropriate program variables */
  extract_args(&args, mcmc);
  /* destroy argtable */
  arg_freetable(argtable, sizeof(argtable)/sizeof(argtable[0]));

  return PAR_SUCCESS;
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

  args->train_x = arg_file0(NULL, "train_x", "<file>", "define the filename for train data (default is X_train.csv");
  args->train_y = arg_file0(NULL, "train_y", "<file>", "define the filename for train labels (default is Y_train.csv");
  args->test_x  = arg_file0(NULL, "test_x", "<file>", "define the filename for test data (default is X_test.csv");
  args->test_y  = arg_file0(NULL, "test_y", "<file>", "define the filename for test labels (default is Y_test.csv");

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
  args->train_x->filename[0] = "X_train.csv";
  args->train_y->filename[0] = "Y_train.csv";
  args->test_x->filename[0] = "X_test.csv";
  args->test_y->filename[0] = "Y_test.csv";

}

static int extract_args(cmd_args *args, mcmc *mcmc)
{
  int error_status = PAR_SUCCESS;

  mcmc->train.dim = args->dim->ival[0];
  mcmc->train.Nd = args->train_n->ival[0];
  mcmc->test.dim = args->dim->ival[0];
  mcmc->test.Nd = args->test_n->ival[0];

  mcmc->metropolis.Ns = args->Ns->ival[0];
  mcmc->metropolis.Nburn = args->Nburn->ival[0];
  mcmc->metropolis.rwsd = args->rwsd->dval[0];

  // mcmc->train.fx = args->train_x->filename[0];
  // mcmc->train.fy = args->train_y->filename[0];
  // mcmc->test.fx = args->test_x->filename[0];
  // mcmc->test.fy = args->test_y->filename[0];
}
