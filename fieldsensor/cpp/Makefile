BINS = raw_test new_net train_net net_test

LIBSRC = \
	lib/FieldSensor.o \
	annie/src/Layer.o \
	annie/src/Exception.o \
	annie/src/File.o \
	annie/src/InputLayer.o \
	annie/src/InputNeuron.o \
	annie/src/Link.o \
	annie/src/Matrix.o \
	annie/src/MultiLayerNetwork.o \
	annie/src/Network.o \
	annie/src/Neuron.o \
	annie/src/SimpleNeuron.o \
	annie/src/TrainingSet.o \
	annie/src/TwoLayerNetwork.o \


CPPFLAGS += -Iinclude -Iannie/include -g -pg
CPPFLAGS += -O3 -fschedule-insns -falign-functions=4 -ffast-math -fforce-addr
CPPFLAGS += -march=athlon-xp -fprefetch-loop-arrays -mfpmath=sse
INCLUDES = include/*.h annie/include/*.h
LDFLAGS += -g -pg
LIBS = lib/libfield.a

all: $(BINS)

raw_test: src/raw_test.cpp $(INCLUDES) $(LIBS)
	g++ -o $@ $< $(CPPFLAGS) $(LDFLAGS) $(LIBS)

new_net: src/new_net.cpp $(INCLUDES) $(LIBS)
	g++ -o $@ $< $(CPPFLAGS) $(LDFLAGS) $(LIBS)

train_net: src/train_net.cpp $(INCLUDES) $(LIBS)
	g++ -o $@ $< $(CPPFLAGS) $(LDFLAGS) $(LIBS)

net_test: src/net_test.cpp $(INCLUDES) $(LIBS)
	g++ -o $@ $< $(CPPFLAGS) $(LDFLAGS) $(LIBS)

lib/libfield.a: $(LIBSRC)
	ar r $@ $(LIBSRC)
	ranlib $@

clean:
	rm -f $(BINS) $(LIBS) $(LIBSRC)