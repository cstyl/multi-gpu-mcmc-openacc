ARGTABLE_INC = ${HOME}/include
ARGTABLE_LIB = ${HOME}/lib

CC	=	gcc
CCFLAGS	= -O2 -Wall -g -std=c99

LDFLAGS = -L${ARGTABLE_LIB}
LDLIBS= -largtable2 -lm

SRC = src
OBJ = obj
BIN = bin
OUT = out
DATA = data
RES = res
SCRIPTS = scripts

RESOURCES=$(SRC)/resources
RNG=$(SRC)/rng
MCMC=$(SRC)/mcmc
UTIL=$(SRC)/util

VPATH = $(SRC) $(RESOURCES) $(RNG) $(MCMC) $(IO) $(UTIL)

INC_PATH = -I$(SRC) -I$(RESOURCES) -I$(RNG) -I$(MCMC) -I$(UTIL) -I$(ARGTABLE_INC)

MAIN_OBJ = $(OBJ)/main.o
# RNG_OBJ = $(OBJ_DIR)/rng_setup.o
UTIL_OBJ = $(OBJ)/cmd_line_parser.o $(OBJ)/memory.o

all: dir $(BIN)/util

$(OBJ)/%.o: %.c
	$(CC) $(CCFLAGS) $(INC_PATH) -o $@ -c $<

$(BIN)/util: $(UTIL_OBJ) $(MAIN_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: dir
## Create necessary directories
dir:
	mkdir -p $(BIN) $(OUT) $(OBJ)

.PHONY: clean
## Create clean directory
clean:
	rm -rf $(BIN) $(OUT) $(OBJ)
