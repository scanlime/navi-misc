LDFLAGS += `sdl-config --libs` -lGL -lGLU
CFLAGS += `sdl-config --cflags` -O3 -g -Wall -DDATADIR='"data"'

OBJS = main.o model.o shader.o
BIN = hdr_test

all: $(BIN)

$(BIN): $(OBJS) *.h
	gcc -o $(BIN) $(LDFLAGS) $(OBJS)

clean:
	rm -f $(BIN) $(OBJS)
