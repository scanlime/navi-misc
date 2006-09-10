LDFLAGS = -lpulse-simple -lfftw3f
CFLAGS = -O3 -ffast-math
OBJS = mi6k.o main.o
BIN = pulse6k

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(BIN) $(OBJS)