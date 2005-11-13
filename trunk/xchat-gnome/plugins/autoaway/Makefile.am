libdir = $(xchatlibdir)/plugins

lib_LTLIBRARIES = autoaway.la

autoaway_la_SOURCES =			\
	xscreensaver.c xscreensaver.h	\
	gscreensaver.c gscreensaver.h	\
	autoaway.c autoaway.h

autoaway_la_LDFLAGS = $(DBUS_LIBS) -avoid-version -module



INCLUDES = $(COMMON_CFLAGS) \
		   $(GTK_CFLAGS) \
		   $(GUI_CFLAGS) \
		   $(DBUS_CFLAGS) \
		   -I$(srcdir)/.. \
		   -I$(top_srcdir)/src/fe-gnome \
		   -DXCHATSHAREDIR="\"$(datadir)/xchat\""

if DO_DBUS
INCLUDES += -DUSE_DBUS
endif
