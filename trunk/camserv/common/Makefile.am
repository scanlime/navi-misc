INCLUDES = -I$(srcdir)/include -I${top_srcdir}/libltdl
CFLAGS += -g -DNDEBUG

common_sources =      \
	databuf.c     \
	grafxmisc.c   \
	hash.c        \
	list.c        \
	log.c	      \
	manager.c     \
	modinfo.c     \
	socket.c      \
	sockset.c     \
	sock_field.c

libcommonincdir = ${srcdir}/include

noinst_LIBRARIES    = libcommon.a
libcommon_a_SOURCES  = $(common_sources)
