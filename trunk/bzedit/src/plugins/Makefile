PKGS		+= gobject-2.0 glib-2.0 gtk+-2.0
CFLAGS		+= $(shell pkg-config --cflags $(PKGS))
CFLAGS		+= -fPIC
CFLAGS		+= -I..
LIBS		+= $(shell pkg-config --libs $(PKGS))

BASE_SO_OBJS	=		\
		base.o		\
		box.o		\
		world.o

all:		base.so

base.so:	$(BASE_SO_OBJS)
		gcc -shared -o $@ $(BASE_SO_OBJS) $(LIBS)

%.o:		%.c *.h
		gcc -c -o $@ $< $(CFLAGS)
