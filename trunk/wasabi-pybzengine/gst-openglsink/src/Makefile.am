# location where plug-in will be installed
openglsinkdir= $(libdir)/gstreamer-@GST_MAJORMINOR@

# change libgstplugin.la to something more suitable
openglsink_LTLIBRARIES = libgstopenglsink.la

# sources used to compile this plug-in
libgstopenglsink_la_SOURCES = gstopenglsink.c

# flags used to compile this plugin
# we use the GST_LIBS flags because we might be using plug-in libs
libgstopenglsink_la_CFLAGS = $(GST_LIBS_CFLAGS)
libgstopenglsink_la_LDFLAGS = $(GST_LIBS_LIBS)

# headers we need but don't want installed
noinst_HEADERS = gstopenglsink.h
