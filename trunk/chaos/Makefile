CFLAGS = -O3 -g -ffast-math -march=i686
GTK = `pkg-config --cflags --libs gtk+-2.0`
BINS = de_jong_explorer

DE_JONG_OBJS = src/de_jong_explorer.c src/color_button.c
DE_JONG_H    = src/color_button.h

all: $(BINS)

de_jong_explorer: $(DE_JONG_OBJS) $(DE_JONG_H)
	gcc -o $@ $(DE_JONG_OBJS) $(GTK) $(CFLAGS)

clean:
	rm -f $(BINS)
