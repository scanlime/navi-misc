PKGS    = libglade-2.0 gtk+-2.0
CFLAGS  = `pkg-config --cflags $(PKGS)` -O3 -ffast-math -march=i686
LIBS    = `pkg-config --libs $(PKGS)`

BIN     = de-jong-explorer

HEADERS = \
	src/color-button.h	\
	src/main.h

OBJS    = \
	src/main.o		\
	src/ui-main.o		\
	src/ui-animation.o	\
	src/color-button.o	\
	src/render.o

$(BIN): $(OBJS)
	gcc -o $@ $(OBJS) $(LIBS)

.c.o: $(HEADERS)
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) src/*.o
