BINS = inputpipe-server
SERVER_OBJS = src/server.o

LINUX_DIR = /lib/modules/`uname -r`/build
CFLAGS = -I$(LINUX_DIR)/include -g

inputpipe-server: $(SERVER_OBJS)
	gcc -o $@ $(SERVER_OBJS) $(LDFLAGS)

clean:
	rm -f $(BINS) $(SERVER_OBJS)