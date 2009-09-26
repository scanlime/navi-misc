CFLAGS := -O3 -g -I../include

BINS        := decoder
OBJ_DECODER := decoder.o memtrace.o

all: $(BINS)

decoder: $(OBJ_DECODER)

*.o: *.h Makefile

clean:
	rm -f $(BINS) $(OBJ_DECODER)
