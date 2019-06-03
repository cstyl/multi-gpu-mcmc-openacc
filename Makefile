CC	=	gcc
CCFLAGS	= -O2 -Wall -std=c99 \
					-I${HOME}/include

LDFLAGS = -L${HOME}/lib
LDLIBS= -largtable2 -lm

SRC = src
OBJ = obj
BIN = bin
OUT = out
DATA = data
RES = res
SCRIPTS = scripts

STRUCTS=$(SRC)/structs
RNG=$(SRC)/rng
MCMC=$(SRC)/mcmc
IO=$(SRC)/io
UTIL=$(SRC)/util

VPATH = $(SRC) $(STRUCTS) $(RNG) $(MCMC) $(IO) $(UTIL)

INC_PATH = -I$(SRC) -I$(STRUCTS) -I$(RNG) -I$(MCMC) -I$(IO) -I$(UTIL)

MAIN_OBJ = $(OBJ)/main.o
# RNG_OBJ = $(OBJ_DIR)/rng_setup.o
ARGS_OBJ = $(OBJ)/cmd_line_parser.o

all: dir $(BIN)/args

$(OBJ)/%.o: %.c
	$(CC) $(CCFLAGS) $(INC_PATH) -o $@ -c $<

$(BIN)/args: $(ARGS_OBJ) $(MAIN_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: dir
## Create necessary directories
dir:
	mkdir -p $(BIN) $(OUT) $(OBJ)

.PHONY: clean
## Create clean directory
clean:
	rm -rf $(BIN) $(OUT) $(OBJ)
