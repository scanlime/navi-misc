libdir = $(xchatlibdir)/plugins

lib_LTLIBRARIES = notifyosd.la
notifyosd_la_SOURCES = notify-osd.c
notifyosd_la_LDFLAGS = -avoid-version -module
notifyosd_la_LIBADD = $(LIBNOTIFY_LIBS)

INCLUDES =				\
	$(COMMON_CFLAGS)		\
	$(GTK_CFLAGS)			\
	$(GUI_CFLAGS)			\
	-I$(srcdir)/..			\
	-I$(top_srcdir)/src/fe-gnome	\
	$(LIBNOTIFY_CFLAGS)		\
	-DXCHATSHAREDIR="\"$(datadir)/xchat\""
