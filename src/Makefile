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
		 inference.o util.o decomposition.o

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
	@echo -e "MCMC make options ...\n"\
	    "\t make host \t\t\t builds serial implementation \n"\
			"\t make host-omp \t\t\t builds OpenMP implementation \n"\
			"\t make host-mpi \t\t\t builds MPI-only implementation \n"\
			"\t make host-hybrid \t\t builds MPI+OpenMP implementation \n"\
			"\t make gpu \t\t\t builds Single GPU implementation \n"\
			"\t make omp-gpu \t\t\t builds Multi-GPU implementation\n" \
			"\t \t   \t      \t\t (using OpenMP for single node)\n"\
			"\t make mpi-gpu \t\t\t builds Multi-GPU implementation\n" \
			"\t \t   \t      \t\t (using MPI only)\n"\
			"\t make mpi-hybrid \t\t builds Multi-GPU implementation\n" \
			"\t \t   \t      \t\t (using MPI+OpenMP) \n"\
      "\t Check the Makefile for further details\n"

code:	$(MAIN).o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(MAIN).o $(LIBRARY) $(LIBS)

# OpenACC versions

host:
	$(MAKE) lib "CFLAGS=$(CFLAGS) -mp –ta=host"
	$(MAKE) code "CFLAGS=$(CFLAGS) -mp –ta=host" "INC=$(INC) -I../mpi_s" "LIBS=$(LIBS) -L../mpi_s -lmpi"

host-omp:
	$(MAKE) lib "CFLAGS=$(CFLAGS) -mp –ta=host"
	$(MAKE) code "CFLAGS=$(CFLAGS) -mp –ta=host" \
							 "INC=$(INC) -I../mpi_s" "LIBS=$(LIBS) -L../mpi_s -lmpi"

host-mpi:
	$(MAKE) libmpi "CFLAGS=$(CFLAGS) –ta=host"
	$(MAKE) code "CC=$(MPICC) -cc=$(CC)" "CFLAGS=$(CFLAGS) –ta=host" \
	 						 "INC=$(INC) $(MPI_INCL)" "LIBS=$(LIBS) $(MPI_LIBS)"

host-hybrid:
	$(MAKE) libmpi "CFLAGS=$(CFLAGS) -mp –ta=host"
	$(MAKE) code "CC=$(MPICC) -cc=$(CC)" "CFLAGS=$(CFLAGS) –ta=host" \
	 						 "INC=$(INC) $(MPI_INCL)" "LIBS=$(LIBS) $(MPI_LIBS)"

gpu:
	$(MAKE) lib "CFLAGS=$(CFLAGS) -acc –ta=tesla -Minfo=accel"
	$(MAKE) code "CFLAGS=$(CFLAGS) -acc –ta=tesla -Minfo=accel" \
							 "INC=$(INC) -I../mpi_s" "LIBS=$(LIBS) -L../mpi_s -lmpi"

omp-gpu:
	$(MAKE) lib "CFLAGS=$(CFLAGS) -acc –ta=tesla -Minfo=accel"
	$(MAKE) code "CFLAGS=$(CFLAGS) -acc –ta=tesla -Minfo=accel" \
							 "INC=$(INC) -I../mpi_s" "LIBS=$(LIBS) -L../mpi_s -lmpi"

mpi-gpu:
	$(MAKE) libmpi "CFLAGS=$(CFLAGS) -acc –ta=tesla -Minfo=accel"
	$(MAKE) code "CC=$(MPICC) -cc=$(CC)" "CFLAGS=$(CFLAGS) -acc –ta=tesla -Minfo=accel" \
	 						 "INC=$(INC) $(MPI_INCL)" "LIBS=$(LIBS) $(MPI_LIBS)"

mpi-hybrid:
	$(MAKE) libmpi "CFLAGS=$(CFLAGS) -mp -acc –ta=tesla -Minfo=accel"
	$(MAKE) code "CC=$(MPICC) -cc=$(CC)" "CFLAGS=$(CFLAGS) -acc –ta=tesla -Minfo=accel" \
	 						 "INC=$(INC) $(MPI_INCL)" "LIBS=$(LIBS) $(MPI_LIBS)"

# Libraries

libar:	$(OBJS)
	$(AR) $(ARFLAGS) $(LIBRARY) $(OBJS)

lib:
	$(MAKE) libar "INC=$(INC) -I../mpi_s" "LIBS= -L../mpi_s -lmpi $(LIBS)"
libmpi:
	$(MAKE) libar "CC=$(CC)" "INC=$(INC) $(MPI_INCL)"

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
