CFLAGS += -march=i686
CFLAGS += -O3 -ffast-math
CFLAGS += -g
BINS   += params-ab params-cd logscale henon

all: $(BINS)

%: %.c
	gcc -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(BINS)
