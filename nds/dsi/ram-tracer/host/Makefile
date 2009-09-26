PACKAGES := libusb-1.0
CFLAGS += $(shell pkg-config --cflags $(PACKAGES))
LDFLAGS += $(shell pkg-config --libs $(PACKAGES))

CFLAGS += -I../include

# Large file support
CFLAGS += $(shell getconf LFS_CFLAGS)

BIN := memhost
OBJS := main.o fastftdi.o fpgaconfig.o bit_file.o \
        hw_common.o hw_trace.o hw_patch.o iohook_svc.o

CFLAGS += -O3 -g

all: $(BIN)

$(BIN): $(OBJS)
	cc -o $(BIN) $(OBJS) $(LDFLAGS)

*.o: *.h Makefile

clean:
	rm -f $(BIN) $(OBJS)
