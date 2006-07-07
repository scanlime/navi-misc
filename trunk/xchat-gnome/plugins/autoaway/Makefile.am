plugindir = $(pkglibdir)/plugins
plugin_LTLIBRARIES = autoaway.la

noinst_HEADERS =	\
	xscreensaver.h	\
	gscreensaver.h	\
	autoaway.h

autoaway_la_SOURCES =	\
	xscreensaver.c	\
	gscreensaver.c	\
	autoaway.c

autoaway_la_CPPFLAGS = \
	 -I$(srcdir)/..	\
	$(AM_CPPFLAGS)

if ENABLE_DBUS
autoaway_la_CPPFLAGS += -DUSE_DBUS
endif

autoaway_la_CFLAGS = \
	 $(DEPENDENCIES_CFLAGS)	\
	 $(DBUS_CFLAGS)		\
	$(AM_CFLAGS)

autoaway_la_LDFLAGS =	\
	-avoid-version	\
	-module		\
	$(AM_LDFLAGS)

autoaway_la_LIBADD =	\
	$(DBUS_LIBS)
