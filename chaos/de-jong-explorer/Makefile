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
	src/cell-editable-table.h

OBJS    = \
	src/main.o		\
	src/de-jong.o		\
	src/de-jong-render.o	\
	src/explorer.o		\
	src/color-button.o	\
	src/animation.o		\
	src/chunked-file.o	\
	src/cell-editable-table.o


$(BIN): $(OBJS)
	gcc -o $@ $(OBJS) $(LIBS)

.c.o: $(HEADERS)
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) src/*.o
