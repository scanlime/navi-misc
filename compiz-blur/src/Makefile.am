libblur_la_LDFLAGS = -module -avoid-version -no-undefined
libblur_la_LIBADD = $(PACKAGE_LIBS)
libblur_la_SOURCES = blur.c

INCLUDES =			\
	$(PACKAGE_CFLAGS)	\
	-I$(top_srcdir)/include

moduledir = /usr/lib/compiz
module_LTLIBRARIES =	\
	libblur.la
