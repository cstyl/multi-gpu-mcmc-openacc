##############################################################################
#
#  lunix-gcc-default.mk
#
#  A typical Unix-like system will use:
#    gcc   Gnu C compiler
#    mpicc Wrapper to the local MPI C compiler
#
#
##############################################################################

CC=gcc
MPICC=mpicc
CFLAGS=-O2 -Wall

AR = ar
ARFLAGS = -cru

LAUNCH_SERIAL_CMD=
LAUNCH_MPI_CMD=mpirun
