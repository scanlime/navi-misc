bin_PROGRAMS = rtgraph-demo

INCLUDES = -I ../include $(PACKAGE_CFLAGS)

rtgraph_demo_SOURCES =			\
	pagestorage.c			\
	pageatom.c			\
	pagestorage-posix.c		\
	bptree.c			\
	demo.c


#	channel.c			\

noinst_HEADERS =			\
	rtgmem.h			\
	pagestorage.h			\
	pageatom.h			\
	bptree.h

rtgraph_demo_LDADD = \
	$(PACKAGE_LIBS)
