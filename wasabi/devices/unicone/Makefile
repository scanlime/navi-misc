BINS = test

TEST_OBJS = \
	src/bit_file.o \
	src/test.o

CFLAGS += -I include
LDFLAGS += -lusb

all: $(BINS)

test: $(TEST_OBJS) src/*.h include/*.h
	$(CC) -o $@ $(TEST_OBJS) $(LDFLAGS)

clean:
	rm -f $(BINS) $(TEST_OBJS)