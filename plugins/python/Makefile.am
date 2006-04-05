libdir = $(XCHATLIBDIR)/plugins

lib_LTLIBRARIES = python.la
python_la_SOURCES = python.c
python_la_LIBADD = $(PYTHON_LIBS)
python_la_LDFLAGS =		\
	-avoid-version -module	\
	$(PYTHON_LIB_LOC)

INCLUDES =			\
	$(PYTHON_CFLAGS)	\
	$(DEPENDENCIES_CFLAGS)	\
	-I$(srcdir)/..
