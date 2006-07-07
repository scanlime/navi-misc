plugindir = $(pkglibdir)/plugins
plugin_LTLIBRARIES = tcl.la

tcl_la_SOURCES =		\
	printevents.h		\
	tclplugin.c		\
	tclplugin.h

tcl_la_CPPFLAGS = \
	-I$(srcdir)/..		\
	$(AM_CPPFLAGS)

tcl_la_CFLAGS = \
	$(TCL_CFLAGS)		\
	$(DEPENDENCIES_CFLAGS)	\
	$(AM_CFLAGS)

tcl_la_LDFLAGS = \
	-avoid-version		\
 	-module			\
 	$(AM_LDFLAGS)

tcl_la_LIBADD = $(TCL_LIBS)
