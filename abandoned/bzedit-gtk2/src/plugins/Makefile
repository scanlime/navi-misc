PKGS			+= gobject-2.0 glib-2.0 gtk+-2.0
CFLAGS			+= $(shell pkg-config --cflags $(PKGS))
CFLAGS			+= -fPIC -I.. -g
LIBS			+= $(shell pkg-config --libs $(PKGS))

PLUGINS			=			\
			core.so			\
			extended.so		\
			renderpasses.so

CORE_SO_OBJS		=			\
			base.o			\
			box.o			\
			core.o			\
			flagzone.o		\
			link.o			\
			pyramid.o		\
			tankzone.o		\
			teleporter.o		\
			world.o			\
			worldweapon.o		\
			zone.o

EXTENDED_SO_OBJS	=			\
			extended.o		\
			group.o			\
			shadow.o

RENDERPASSES_SO_OBJS	=			\
			basicrenderpass.o	\
			blendedrenderpass.o	\
			renderpasses.o

all:			$(PLUGINS)

core.so:		$(CORE_SO_OBJS)
			gcc -shared -o $@ $(CORE_SO_OBJS) $(LIBS)

extended.so:		$(EXTENDED_SO_OBJS)
			gcc -shared -o $@ $(EXTENDED_SO_OBJS) $(LIBS)

renderpasses.so:	$(RENDERPASSES_SO_OBJS)
			gcc -shared -o $@ $(RENDERPASSES_SO_OBJS) $(LIBS)

%.o:			%.c *.h
			gcc -c -o $@ $< $(CFLAGS)

clean:
			rm -f $(PLUGINS) *.o
