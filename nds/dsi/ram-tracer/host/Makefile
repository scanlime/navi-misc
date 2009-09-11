PACKAGES := libusb-1.0
CFLAGS += $(shell pkg-config --cflags $(PACKAGES))
LDFLAGS += $(shell pkg-config --libs $(PACKAGES))

BIN := memhost
OBJS := main.o fastftdi.o fpgaconfig.o

CFLAGS += -O3 -g

all: $(BIN)

$(BIN): $(OBJS)
	cc -o $(BIN) $(OBJS) $(LDFLAGS)

*.o: *.h Makefile

clean:
	rm -f $(BIN) $(OBJS)
