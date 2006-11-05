plugindir = $(pkglibdir)/plugins
plugin_LTLIBRARIES = notifyosd.la

notifyosd_la_SOURCES = notify-osd.c

notifyosd_la_CPPFLAGS = \
	-I$(srcdir)/..				\
	-I$(top_srcdir)/src/fe-gnome		\
	-DXCHATSHAREDIR="\"$(pkgdatadir)\""	\
	$(AM_CPPFLAGS)

notifyosd_la_CFLAGS = \
	$(DEPENDENCIES_CFLAGS)	\
	$(LIBNOTIFY_CFLAGS)	\
	$(AM_CFLAGS)

notifyosd_la_LDFLAGS = \
	-avoid-version 	\
	-module		\
	$(AM_LDFLAGS)

notifyosd_la_LIBADD = $(LIBNOTIFY_LIBS)
