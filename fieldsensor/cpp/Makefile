BINS = raw_test

LIBSRC = lib/FieldSensor.o

CPPFLAGS += -Iinclude
INCLUDES = include/*.h
LIBS = lib/libfield.a

all: $(BINS)

raw_test: src/raw_test.cpp $(INCLUDES) $(LIBS)
	g++ -o $@ $< $(CPPFLAGS) $(LDFLAGS) $(LIBS)

lib/libfield.a: $(LIBSRC)
	ar r lib/libfield.a $(LIBSRC)

clean:
	rm -f $(BINS) $(LIBS) $(LIBSRC)