OBJS = \
	src/bit_file.c \
	src/test.c

BINS = test

all: $(BINS)

test: $(OBJS) src/*.h
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
