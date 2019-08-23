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
CFLAGS=-fast

AR = ar
ARFLAGS = -cru

LAUNCH_SERIAL_CMD=
LAUNCH_MPI_CMD=mpirun

# Change accordingly to specify location of OPENBLAS Library
BLAS_INC = ${HOME}/lib-blas/include
BLAS_LIB = ${HOME}/lib-blas/lib -lopenblas
