libexecdir = $(CAMSERV_MODDIR)
datadir = @datadir@/camserv

CPPFLAGS +=-DDATDIR=\"${datadir}\" 
LDFLAGS += -export-dynamic

bin_PROGRAMS = camserv
camserv_LDADD        = $(LIBLTDL) $(top_srcdir)/common/libcommon.a
camserv_DEPENDENCIES = $(LIBLTDL)
camserv_SOURCES= \
	camconfig.c	\
	camserv.c	\
	filter.c	\
	main_loop.c	\
	picture_loop.c	\
	shm.c		\
	video.c 

libgdk_pixbuf_filter_la_LIBADD  = $(GDKPIXBUF_LIBS)
libgdk_pixbuf_filter_la_SOURCES = gdkpixbuf_filter.c
libimlib2_filter_la_LIBADD      = $(IMLIB2_LIBS)
libimlib2_filter_la_SOURCES     = imlib2_filter.c
libjpg_filter_la_SOURCES        = jpg_filter.c jpgstuff.c
librand_filter_la_SOURCES       = rand_filter.c 
libtext_filter_la_SOURCES       = text_filter.c fixfont.c
libvideo_basic_la_SOURCES       = video_basic.c 
libvideo_fbsd_bttv_la_SOURCES   = video_fbsd_bttv.c
libvideo_v4l_la_SOURCES         = video_v4l.c

if HAVE_GDKPIXBUF
gdk_ltlibs = libgdk_pixbuf_filter.la
endif

if VIDEO_V4L
v4l_ltlibs = libvideo_v4l.la
endif

if VIDEO_FBSD_BTTV
fbsd_bttv_ltlibs = libvideo_fbsd_bttv.la
endif

if JPEG_VALID
jpg_ltlibs = libjpg_filter.la
endif

if HAVE_IMLIB2
imlib2_ltlibs = libimlib2_filter.la
endif

libexec_LTLIBRARIES = 	\
	${jpg_ltlibs}		\
	librand_filter.la	\
	libtext_filter.la	\
	${v4l_ltlibs}           \
	${imlib2_ltlibs}	\
	${gdk_ltlibs}		\
	${fbsd_bttv_ltlibs}	\
	libvideo_basic.la


EXTRA_DIST=alpha_map.h 				\
	$(libgdk_pixbuf_filter_la_SOURCES) 	\
	$(libimlib2_filter_la_SOURCES)		\
	$(libjpg_filter_la_SOURCES)		\
	$(libvideo_fbsd_bttv_la_SOURCES)	\
	$(libvideo_v4l_la_SOURCES)

