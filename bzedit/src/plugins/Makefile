PKGS			+= gobject-2.0 glib-2.0 gtk+-2.0
CFLAGS			+= $(shell pkg-config --cflags $(PKGS))
CFLAGS			+= -fPIC -I.. -g
LIBS			+= $(shell pkg-config --libs $(PKGS))

PLUGINS			=			\
			core.so			\
			renderpasses.so

CORE_SO_OBJS		=			\
			box.o			\
			core.o			\
			group.o			\
			pyramid.o		\
			teleporter.o		\
			world.o

RENDERPASSES_SO_OBJS	=			\
			basicrenderpass.o	\
			renderpasses.o

all:			$(PLUGINS)

core.so:		$(CORE_SO_OBJS)
			gcc -shared -o $@ $(CORE_SO_OBJS) $(LIBS)

renderpasses.so:	$(RENDERPASSES_SO_OBJS)
			gcc -shared -o $@ $(RENDERPASSES_SO_OBJS) $(LIBS)

%.o:			%.c *.h
			gcc -c -o $@ $< $(CFLAGS)

clean:
			rm -f $(PLUGINS) *.o
