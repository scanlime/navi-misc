PKGS    += libglade-2.0 gtk+-2.0
CFLAGS  += `pkg-config --cflags $(PKGS)` -O3 -ffast-math #-march=i686
LIBS    += `pkg-config --libs $(PKGS)`

BIN     = de-jong-explorer

OBJS    = \
	src/main.o		\
	src/de-jong.o		\
	src/explorer.o		\
	src/color-button.o	\
	src/animation.o		\
	src/chunked-file.o	\
	src/curve-editor.o	\
	src/spline.o		\
	src/explorer-tools.o	\
	src/explorer-animation.o \
	src/cell-renderer-transition.o \
	src/cell-renderer-bifurcation.o \
	src/histogram-imager.o	\
	src/parameter-holder.o	\
	src/bifurcation-diagram.o \
	src/math-util.o		\
	src/avi-writer.o	\
	src/parameter-editor.o


$(BIN): $(OBJS)
	gcc -o $@ $(OBJS) $(LIBS)

%.o: %.c src/*.h
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) src/*.o
