CFLAGS += -march=i686
CFLAGS += -O3 -ffast-math
BINS   += params-ab params-cd logscale

all: $(BINS)

%: %.c
	gcc -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(BINS)
