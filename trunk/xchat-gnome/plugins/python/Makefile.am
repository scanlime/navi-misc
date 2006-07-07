plugindir = $(pkglibdir)/plugins
plugin_LTLIBRARIES = python.la

python_la_SOURCES = python.c

python_la_CPPFLAGS = \
	-I$(srcdir)/..		\
	$(AM_CPPFLAGS)

python_la_CFLAGS = \
	$(PYTHON_CFLAGS)	\
	$(DEPENDENCIES_CFLAGS)	\
	$(AM_CFLAGS)

python_la_LDFLAGS =		\
	-avoid-version		\
	-module			\
	$(PYTHON_LIB_LOC)	\
	$(AM_LDFLAGS)

python_la_LIBADD = $(PYTHON_LIBS)
