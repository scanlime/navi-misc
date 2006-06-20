libdir = $(XCHATLIBDIR)/plugins

lib_LTLIBRARIES = netmonitor.la
netmonitor_la_SOURCES =	net-monitor.c
netmonitor_la_LDFLAGS = -avoid-version -module
netmonitor_la_LIBADD = $(DBUS_LIBS)
INCLUDES =		\
	$(DEPENDENCIES_CFLAGS)	\
	$(DBUS_CFLAGS)		\
	-I$(srcdir)/..
