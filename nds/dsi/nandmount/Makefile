PACKAGES := fuse
CFLAGS += $(shell pkg-config --cflags $(PACKAGES))
LDFLAGS += $(shell pkg-config --libs $(PACKAGES))

BIN := nandmount
OBJS := nandmount.o

all: $(BIN)

$(BIN): $(OBJS)
	cc -o $(BIN) $(OBJS) $(LDFLAGS)

*.o: Makefile

clean:
	rm -f $(BIN) $(OBJS)
