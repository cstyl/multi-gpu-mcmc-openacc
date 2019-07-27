###############################################################################
#
#  Makefile
#
#  Unit tests for MCMC (Serial and MPI)
#
#  The targets follow those in the src directory and the appropriate
#  model library must be available. In addition:
#
#  make run-serial
#  make run-mpi
#
#  with compile and run the tests (MPI using NPROCS set below)
#
#  Edinburgh Soft Matter and Statistical Physics Group and
#  Edinburgh Parallel Computing Centre
#
#  Kevin Stratford (kevin@epcc.ed.ac.uk)
#  (c) 2010-2015 The University of Edinburgh
#
###############################################################################

include ../../Makefile.mk

#------------------------------------------------------------------------------
# Compilation options, etc.
#------------------------------------------------------------------------------

NPROCS = 8
MPIRUN = ${LAUNCH_MPI_CMD} ${LAUNCH_MPI_NP_SWITCH} $(NPROCS)

SRC     = ../../src
INCLUDE = -I$(SRC)

MPI_STUB_INCLUDE = -I../../mpi_s
MPI_STUB_LIB = -L../../mpi_s -lmpi

CLIBS  = -lm
MPILIB = -lmpi

#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------

TESTSOURCES = test_pe.c test_runtime.c test_random.c \
							test_memory.c test_data_input.c test_logistic_regression.c \
							test_prior.c test_multivariate_normal.c \
							test_chain.c test_sample.c

TESTS = ${TESTSOURCES:.c=}
TESTOBJECTS = ${TESTSOURCES:.c=.o}

#------------------------------------------------------------------------------
#  Rules
#------------------------------------------------------------------------------

base-me: $(TESTOBJECTS) tests.o
	$(CC) $(LDFLAGS) -L$(SRC) $(TESTOBJECTS) tests.o -L$(SRC) -lmcmc $(CLIBS)

serial:
	$(MAKE) serial-test

serial-float:
	$(MAKE) serial-test "OPTS=$(OPTS) -D_FLOAT_"

serial-test:
	$(MAKE) base-me "INCLUDE = $(INCLUDE) -I$(BLAS_INC) $(MPI_STUB_INCLUDE)" \
	"CLIBS=$(CLIBS) $(MPI_STUB_LIB) $(LBLIBS) -L$(BLAS_LIB)"

mpi:
	$(MAKE) mpi-test

mpi-float:
	$(MAKE) mpi-test "OPTS=$(OPTS) -D_FLOAT_"

mpi-test:
	$(MAKE) base-me "CC=$(MPICC)" "INCLUDE = $(INCLUDE) $(MPI_INCL)" \
		"CLIBS=$(MPI_LIBS) $(CLIBS) $(LBLIBS)"


run-serial:
	$(LAUNCH_SERIAL_CMD) ./a.out

run-mpi:
	$(MPIRUN) ./a.out

clean:
	$(RM) core *.o a.out

#------------------------------------------------------------------------------
# Other dependencies
#------------------------------------------------------------------------------

tests.o:	Makefile

#------------------------------------------------------------------------------
#  Implicit Rules
#------------------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(CC) $(OPTS) $(CFLAGS) $(INCLUDE) -c $*.c
