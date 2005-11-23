libdir = $(XCHATLIBDIR)/plugins

lib_LTLIBRARIES = tcl.la
tcl_la_SOURCES =		\
	tclplugin.c		\
	tclplugin.h
tcl_la_LDFLAGS = -avoid-version -module
tcl_la_LIBADD = $(TCL_LIBS)
tcl_la_CFLAGS =			\
	$(TCL_CFLAGS)		\
	$(DEPENDENCIES_CFLAGS)	\
	-I$(srcdir)/..
