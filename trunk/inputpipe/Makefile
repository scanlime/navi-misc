BINS = inputpipe-server inputpipe-client

INSTALL = install
INSTDIR = /usr/local/bin

COMMON_OBJS = \
	src/packet.o

SERVER_OBJS = $(COMMON_OBJS) \
	src/server.o

CLIENT_OBJS = $(COMMON_OBJS) \
	src/client.o

CFLAGS = -I uinput -I src -g

all: $(BINS)

install: $(BINS)
	$(INSTALL) inputpipe-server $(INSTDIR)/inputpipe-server
	$(INSTALL) inputpipe-client $(INSTDIR)/inputpipe-client

inputpipe-server: $(SERVER_OBJS)
	gcc -o $@ $(SERVER_OBJS) $(LDFLAGS)

inputpipe-client: $(CLIENT_OBJS)
	gcc -o $@ $(CLIENT_OBJS) $(LDFLAGS)

clean:
	rm -f $(BINS) $(SERVER_OBJS) $(CLIENT_OBJS)
