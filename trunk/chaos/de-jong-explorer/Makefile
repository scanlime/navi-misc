PKGS    = libglade-2.0 gtk+-2.0
CFLAGS  = `pkg-config --cflags $(PKGS)` -O3 -g -ffast-math -march=i686
LIBS    = `pkg-config --libs $(PKGS)`

BIN     = de-jong-explorer
OBJS    = src/main.o src/interface.o src/color-button.o src/render.o
HEADERS = src/color-button.h

$(BIN): $(OBJS)
	gcc -o $@ $(OBJS) $(LIBS)

.c.o: $(HEADERS)
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) src/*.o
