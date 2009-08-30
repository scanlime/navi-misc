INCLUDES = -I. -I$(top_srcdir) -I$(top_builddir)

bin_PROGRAMS = fx2pipe
fx2pipe_SOURCES = main.cc \
	fx2pipe.h fx2pipe.cc \
	../firmware/fx2pipe_static.cc
fx2pipe_LDADD = ../usb_io/lib_usb_io.a ../lib/lib_fx2pipe_supp.a -lusb 

# fx2pipe_LDADD = $(QTLIBS) -L/usr/X11R6/lib -lX11 -lXft
