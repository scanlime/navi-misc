bin_PROGRAMS = rtgraph-demo

INCLUDES = -I ../include $(PACKAGE_CFLAGS)

rtgraph_demo_SOURCES =			\
	pagestorage.c			\
	pagestorage-posix.c		\
	demo.c				\
	bptree.c


#	channel.c			\

noinst_HEADERS =			\
	bptree.h			\
	mem-util.h			\
	pagestorage.h			\
	rtgmem.h

rtgraph_demo_LDADD = \
	$(PACKAGE_LIBS)
