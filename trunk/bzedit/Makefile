PKGS	+= libglade-2.0 gtk+-2.0 gtkglext-1.0
CFLAGS	+= '-DGLADEDIR="data"'
CFLAGS	+= $(shell pkg-config --cflags $(PKGS))
LIBS	+= $(shell pkg-config --libs $(PKGS))

BIN	= bzedit

OBJS	= 			\
	src/gldrawingarea.o	\
	src/main.o		\
	src/parameter-editor.o	\
	src/parameter-holder.o

$(BIN):	$(OBJS)
	gcc -o $@ $(OBJS) $(LIBS)

%.o:	%.c src/*.h
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) src/*.o
