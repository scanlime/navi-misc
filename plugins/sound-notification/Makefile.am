plugindir = $(pkglibdir)/plugins
plugin_LTLIBRARIES = soundnotification.la

soundnotification_la_SOURCES =	\
	sound-notification.c

soundnotification_la_CPPFLAGS = \
	-I$(top_srcdir)/src/fe-gnome		\
	-I$(srcdir)/..				\
	-DXCHATSHAREDIR="\"$(pkgdatadir)\""	\
	$(AM_CPPFLAGS)

soundnotification_la_CFLAGS = \
	$(DEPENDENCIES_CFLAGS)			\
	$(AM_CFLAGS)

soundnotification_la_LDFLAGS = \
	-avoid-version 	\
	-module		\
	$(AM_LDFLAGS)
