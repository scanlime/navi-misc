PKGS	+= libglade-2.0 gtk+-2.0 gtkglext-1.0
CFLAGS	+= '-DGLADEDIR="data"'
CFLAGS	+= $(shell pkg-config --cflags $(PKGS))
LIBS	+= $(shell pkg-config --libs $(PKGS))

BIN	= bzedit

OBJS	= 			\
	src/displaylist.o	\
	src/drawable.o		\
	src/editor.o		\
	src/gldrawingarea.o	\
	src/main.o		\
	src/parameter-editor.o	\
	src/parameter-holder.o	\
	src/plugins.o		\
	src/renderpass.o	\
	src/renderstate.o	\
	src/scene.o		\
	src/sceneobject.o	\
	src/texturemanager.o

$(BIN):	$(OBJS)
	gcc -o $@ $(OBJS) $(LIBS)

%.o:	%.c src/*.h
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) src/*.o
