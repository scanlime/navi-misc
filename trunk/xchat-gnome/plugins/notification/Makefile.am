libdir = $(xchatlibdir)/plugins

lib_LTLIBRARIES = notification.la
notification_la_SOURCES = eggtrayicon.h eggtrayicon.c notification.c
notification_la_LDFLAGS = -avoid-version -module

INCLUDES = $(COMMON_CFLAGS) \
		   $(GTK_CFLAGS) \
		   $(GUI_CFLAGS) \
		   -I$(srcdir)/.. \
		   -I$(top_srcdir)/src/fe-gnome \
		   -DEGG_COMPILATION \
		   -DGTK_DISABLE_DEPRECATED \
		   -DGDK_DISABLE_DEPRECATED \
		   -DG_DISABLE_DEPRECATED \
		   -DXCHATSHAREDIR="\"$(datadir)/xchat\""
