CFLAGS = -O3 -g -ffast-math -march=i686
GTK = `pkg-config --cflags --libs gtk+-2.0`
BINS = de_jong_explorer

all: $(BINS)

de_jong_explorer: src/de_jong_explorer.c
	gcc -o $@ $< $(GTK) $(CFLAGS)

clean:
	rm -f $(BINS)
