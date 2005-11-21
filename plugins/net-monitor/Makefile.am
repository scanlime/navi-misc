libdir = $(xchatlibdir)/plugins

lib_LTLIBRARIES = netmonitor.la
netmonitor_la_SOURCES =	net-monitor.c
netmonitor_la_LDFLAGS = -avoid-version -module
netmonitor_la_LIBADD = $(DBUS_LIBS)

INCLUDES =			\
	$(COMMON_CFLAGS)	\
	-I$(srcdir)/..		\
	$(DBUS_CFLAGS)
