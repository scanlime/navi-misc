CPLUSPLUS=g++
DOXYGEN=doxygen
AR=ar r
FLAGS=-O -Wall

LIB=annie
LIBFILE=lib$(LIB).a
LIB_DIR=lib/
SRC_DIR=src/
OBJ_DIR=obj/
BIN_DIR=bin/
INC_DIR=include/
EXAMPLES_DIR=examples/

OFILES=	$(SRC_DIR)Exception.o  \
	$(SRC_DIR)File.o \
	$(SRC_DIR)Neuron.o \
	$(SRC_DIR)InputNeuron.o \
	$(SRC_DIR)SimpleNeuron.o \
	$(SRC_DIR)CenterNeuron.o \
	$(SRC_DIR)Link.o \
	$(SRC_DIR)Layer.o \
	$(SRC_DIR)InputLayer.o \
	$(SRC_DIR)Network.o \
	$(SRC_DIR)MultiLayerNetwork.o \
	$(SRC_DIR)TwoLayerNetwork.o \
	$(SRC_DIR)RadialBasisNetwork.o \
	$(SRC_DIR)TrainingSet.o \
	$(SRC_DIR)Matrix.o \
	$(SRC_DIR)RecurrentNeuron.o \
	$(SRC_DIR)HopfieldNetwork.o 

EXAMPLES_OFILES= $(EXAMPLES_DIR)xor.o $(EXAMPLES_DIR)hopfield01.o

default: lib examples utils

utils: bin2txt txt2bin

all: lib examples utils docs

lib: $(OFILES)
	$(AR) $(LIB_DIR)$(LIBFILE) $(OFILES)

bin2txt: lib $(SRC_DIR)bin2txt.o
	$(CPLUSPLUS) -o $(BIN_DIR)bin2txt $(SRC_DIR)bin2txt.o -I $(INC_DIR) -L $(LIB_DIR) -l$(LIB)

txt2bin: lib $(SRC_DIR)txt2bin.o
	$(CPLUSPLUS) -o $(BIN_DIR)txt2bin $(SRC_DIR)txt2bin.o -I $(INC_DIR) -L $(LIB_DIR) -l$(LIB)

examples: lib $(EXAMPLES_OFILES)
	$(CPLUSPLUS) -o $(BIN_DIR)exampleXOR $(EXAMPLES_DIR)xor.o -I $(INC_DIR) -L $(LIB_DIR) -l$(LIB)
	$(CPLUSPLUS) -o $(BIN_DIR)exampleHopfield01 $(EXAMPLES_DIR)hopfield01.o -I $(INC_DIR) -L $(LIB_DIR) -l$(LIB)

docs:
	cd doc; $(DOXYGEN)
clean:
	rm *.o -rf
	rm *~ -rf
	rm $(SRC_DIR)*.o -rf
	rm $(EXAMPLES_DIR)*.o -rf

.cpp.o:
	$(CPLUSPLUS) $(FLAGS) -c $< -o $@ -I $(INC_DIR)
