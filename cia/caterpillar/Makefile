CFLAGS = -Wall -O3

BINS = catd client
CATD_OBJS = catd.o common.o
CLIENT_OBJS = client.o common.o

all: $(BINS)

catd: $(CATD_OBJS)
	$(CC) -o $@ $(CATD_OBJS) $(LDFLAGS)

client: $(CLIENT_OBJS)
	$(CC) -o $@ $(CLIENT_OBJS) $(LDFLAGS)

clean:
	rm -f $(BINS) $(CATD_OBJS) $(CLIENT_OBJS)