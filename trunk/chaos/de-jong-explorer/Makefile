PKGS    += libglade-2.0 gtk+-2.0
CFLAGS  += `pkg-config --cflags $(PKGS)` -O3 -ffast-math -march=i686
LIBS    += `pkg-config --libs $(PKGS)`

BIN     = de-jong-explorer

HEADERS = \
	src/color-button.h	\
	src/de-jong.h		\
	src/explorer.h		\
	src/animation.h		\
	src/chunked-file.h	\
	src/curve-editor.h	\
	src/spline.h		\
	src/cell-renderer-transition.h

OBJS    = \
	src/main.o		\
	src/de-jong.o		\
	src/de-jong-render.o	\
	src/explorer.o		\
	src/color-button.o	\
	src/animation.o		\
	src/chunked-file.o	\
	src/curve-editor.o	\
	src/spline.o		\
	src/explorer-tools.o	\
	src/explorer-animation.o \
	src/cell-renderer-transition.o


$(BIN): $(OBJS)
	gcc -o $@ $(OBJS) $(LIBS)

%.o: %.c $(HEADERS)
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) src/*.o
