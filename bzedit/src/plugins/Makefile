PKGS		+= gobject-2.0 glib-2.0 gtk+-2.0
CFLAGS		+= $(shell pkg-config --cflags $(PKGS))
CFLAGS		+= -fPIC -I..
LIBS		+= $(shell pkg-config --libs $(PKGS))

PLUGINS		=		\
		core.so		\
		test.so

CORE_SO_OBJS	=		\
		box.o		\
		core.o		\
		teleporter.o	\
		world.o

TEST_SO_OBJS	=		\
		pyramid.o	\
		test.o

all:		$(PLUGINS)

core.so:	$(CORE_SO_OBJS)
		gcc -shared -o $@ $(CORE_SO_OBJS) $(LIBS)

test.so:	$(TEST_SO_OBJS)
		gcc -shared -o $@ $(TEST_SO_OBJS) $(LIBS)

%.o:		%.c *.h
		gcc -c -o $@ $< $(CFLAGS)

clean:
		rm -f $(PLUGINS) *.o
