plugindir = $(pkglibdir)/plugins
plugin_LTLIBRARIES = netmonitor.la

netmonitor_la_SOURCES =	net-monitor.c

netmonitor_la_CPPFLAGS = \
	-I$(srcdir)/..		\
	$(AM_CPPFLAGS)

netmonitor_la_CFLAGS = \
	$(DEPENDENCIES_CFLAGS)	\
	$(DBUS_CFLAGS)		\
	$(AM_CFLAGS)

netmonitor_la_LDFLAGS = \
	-avoid-version 		\
	-module			\
	$(AM_LDFLAGS)

netmonitor_la_LIBADD = $(DBUS_LIBS)
