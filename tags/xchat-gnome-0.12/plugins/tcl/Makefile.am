libdir = $(XCHATLIBDIR)/plugins

lib_LTLIBRARIES = tcl.la
tcl_la_SOURCES =		\
	printevents.h		\
	tclplugin.c		\
	tclplugin.h
tcl_la_LDFLAGS = -avoid-version -module
tcl_la_LIBADD = $(TCL_LIBS)
INCLUDES =			\
	$(TCL_CFLAGS)		\
	$(DEPENDENCIES_CFLAGS)	\
	-I$(srcdir)/..
