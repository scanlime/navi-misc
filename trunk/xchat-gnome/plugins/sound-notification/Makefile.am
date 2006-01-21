libdir = $(XCHATLIBDIR)/plugins

lib_LTLIBRARIES = soundnotification.la
soundnotification_la_SOURCES =	\
	sound-notification.c
soundnotification_la_LDFLAGS = -avoid-version -module
INCLUDES =					\
	$(DEPENDENCIES_CFLAGS)			\
	-I$(top_srcdir)/src/fe-gnome		\
	-I$(srcdir)/..				\
	-DXCHATSHAREDIR="\"$(XCHATSHAREDIR)\""
