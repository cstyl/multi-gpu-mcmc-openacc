###############################################################################
#
# Makefile for MCMC
#
# Preprocessor flags.
#
# Please use the following for any production run
#
# -DNDEBUG               Switch off standard C assert()
#                        The assertions can make the code much slower.
#
# make serial            for serial code
#
#
# Compiler flags.
#
# Appropriate flags should be set in the parent
# directory in the file config.mk. See ../Makefile.mk
#
###############################################################################

include ../Makefile.mk

MAIN = main
EXECUTABLE = mcmc.exe
LIBRARY = libmcmc.a

OPTS =
LIBS = -L$(BLAS_LIB) -lm
INC = -I$(BLAS_INC) -I.

###############################################################################
#
#  Files
#
###############################################################################

OBJS = \
		 chain.o data_input.o logistic_regression.o \
		 mcmc.o memory.o metropolis.o \
		 multivariate_normal.o pe.o prior.o \
		 ran.o runtime.o sample.o timer.o \
		 autocorrelation.o effective_sample_size.o \
		 inference.o util.o

###############################################################################
#
#  Targets
#
#  Typing "make" will give some instructions.
#
###############################################################################

default:
	$(MAKE) -s verbose

verbose:
	echo -e "MCMC make options ...\n"\
	    "\t make serial \t\t builds serial implementation \n"\
      "\t Check the Makefile for further details\n"

code:	$(MAIN).o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(MAIN).o $(LIBRARY) $(LIBS)

# Serial versions

serial:
	$(MAKE) serial-model

serial-float:
	$(MAKE) serial-model "OPTS=$(OPTS) -D_FLOAT_"

serial-model:
	$(MAKE) lib
	$(MAKE) code "INC=$(INC) -I../mpi_s" "LIBS=$(LIBS) -L../mpi_s -lmpi"

# MPI versions

mpi:
	$(MAKE) mpi-model

mpi-model:
	$(MAKE) libmpi
	$(MAKE) code "CC=$(MPICC)" "INC=$(INC) $(MPI_INCL)" "LIBS=$(LIBS) $(MPI_LIBS)"

# Libraries

libar:	$(OBJS)
	$(AR) $(ARFLAGS) $(LIBRARY) $(OBJS)

lib:
	$(MAKE) libar "INC=$(INC) -I../mpi_s" "LIBS= -L../mpi_s -lmpi $(LIBS)"
libmpi:
	$(MAKE) libar "CC=$(MPICC)" "INC=$(INC) $(MPI_INCL)"

###############################################################################
#
#  Compilation rules
#
###############################################################################


#
#  Default rule for compilation in the current directory.
#
.c.o :
	$(CC) $(OPTS) $(CFLAGS) $(INC) -c $?


.PHONY : clean
clean:
	rm -f $(OBJS) $(EXECUTABLE) $(LIBRARY) $(MAIN).o