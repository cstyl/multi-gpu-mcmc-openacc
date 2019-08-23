##############################################################################
#
#  lunix-pgi-default.mk
#
#  A typical Unix-like system will use:
#    pgcc  PGI C compiler
#    mpicc Wrapper to the local MPI C compiler
#
#
##############################################################################

CC=pgcc
MPICC=mpicc
CFLAGS=-fast -acc

AR = ar
ARFLAGS = -cru

LAUNCH_SERIAL_CMD=
LAUNCH_MPI_CMD=mpirun

# Change accordingly to specify location of OPENBLAS Library
BLAS_INC = /path/to/openblas/include
BLAS_LIB = /path/to/openblas/lib -lopenblas
