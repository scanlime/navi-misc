libdir = $(XCHATLIBDIR)/plugins

lib_LTLIBRARIES = autoaway.la

noinst_HEADERS =	\
	xscreensaver.h	\
	gscreensaver.h	\
	autoaway.h

autoaway_la_SOURCES =	\
	xscreensaver.c	\
	gscreensaver.c	\
	autoaway.c

autoaway_la_LDFLAGS =	\
	-avoid-version	\
	-module

autoaway_la_LIBADD =	\
	$(DBUS_LIBS)

autoaway_la_CFLAGS =		\
	 $(DEPENDENCIES_CFLAGS)	\
	 $(DBUS_CFLAGS)		\
	 -I$(srcdir)/..

if ENABLE_DBUS
autoaway_la_CFLAGS += -DUSE_DBUS
endif
