LDFLAGS=-lpulse-simple -lfftw3f
OBJS = mi6k.o main.o

all: pulse6k

pulse6k: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
