CFLAGS = -O3 -g
BIN = de_jong_explorer

all: $(BIN)

de_jong_explorer:	de_jong_explorer.cpp
	g++ -o de_jong_explorer de_jong_explorer.cpp `pkg-config --cflags --libs gtk+-2.0` $(CFLAGS)

clean:
	rm -f $(BIN)
