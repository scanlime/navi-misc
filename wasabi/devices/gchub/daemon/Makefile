BIN = cubed
OBJS = cubed.o

CFLAGS += -I ../include -I /usr/local/include
LDFLAGS += -L /usr/local/lib -lusb

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f $(BIN) $(OBJS)