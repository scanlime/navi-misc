CFLAGS = -O3 -g -ffast-math -march=i686 -msse2
BIN = de_jong_explorer

all: $(BIN)

de_jong_explorer:	de_jong_explorer.c
	gcc -o de_jong_explorer de_jong_explorer.c `pkg-config --cflags --libs gtk+-2.0` $(CFLAGS)

clean:
	rm -f $(BIN)
