BINS = test

TEST_OBJS = \
	src/bit_file.c \
	src/test.c

LDFLAGS += -lusb

all: $(BINS)

test: $(TEST_OBJS) src/*.h
	$(CC) -o $@ $(TEST_OBJS) $(LDFLAGS)
