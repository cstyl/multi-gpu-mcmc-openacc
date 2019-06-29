ARGTABLE_INC = ${HOME}/include
ARGTABLE_LIB = ${HOME}/lib
GSL_INC=${HOME}/include/gsl
GSL_LIB=${HOME}/lib

CC	=	gcc
CCFLAGS	= -O2 -Wall -g -std=c99

LDFLAGS = -L${ARGTABLE_LIB} -L${GSL_LIB}
LDLIBS= -largtable2 -lgsl -lgslcblas -lm

SRC = src
OBJ = obj
BIN = bin
OUT = out
DATA = data
RES = res
SCRIPTS = scripts

VPATH = $(SRC)

INC_PATH = -I$(SRC) -I$(ARGTABLE_INC) -I$(GSL_INC)

OBJS = $(OBJ)/main.o \
			 $(OBJ)/chain.o $(OBJ)/command_line_parser.o $(OBJ)/data_input.o \
			 $(OBJ)/logistic_regression.o $(OBJ)/mcmc.o $(OBJ)/memory.o \
			 $(OBJ)/metropolis.o $(OBJ)/prior.o $(OBJ)/random_number_generator.o \
			 $(OBJ)/sample.o $(OBJ)/autocorrelation.o $(OBJ)/effective_sample_size.o \
			 $(OBJ)/inference.o $(OBJ)/timer.o $(OBJ)/util.o


all: dir $(BIN)/util

run:
	./$(BIN)/util --dim=2 --train_n=5000 --test_n=1000 --samples=500000 --burn=100000 \
								--rwsd=0 --datadir=data/synthetic/6000_2 \
								--maxlag=249999 --outdir=./out --dataset=synthetic \
								--train_x=X_train.csv --train_y=Y_train.csv \
								--test_x=X_test.csv --test_y=Y_test.csv

shortrun:
	./$(BIN)/util --dim=2 --train_n=500 --test_n=100 --samples=35000 --burn=10000 \
								--rwsd=0 --datadir=data/synthetic/600_2 \
								--maxlag=17499 --outdir=./out --dataset=synthetic \
								--train_x=X_train.csv --train_y=Y_train.csv \
								--test_x=X_test.csv --test_y=Y_test.csv


synthetic:
	cd ./generators/synthetic/ && \
	python generator.py -dim $(DIM) -N $(N) -train 0.8 -precision double && \
	mv *.csv ../../data/

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CCFLAGS) $(INC_PATH) -o $@ -c $<

$(BIN)/util: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: dir
## Create necessary directories
dir:
	mkdir -p $(BIN) $(OUT) $(OBJ)

.PHONY: clean
## Create clean directory
clean:
	rm -rf $(BIN) $(OUT) $(OBJ)
