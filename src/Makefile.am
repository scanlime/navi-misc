# $Id: Makefile.am,v 1.5 2002/06/19 07:07:44 chipx86 Exp $

bin_PROGRAMS = tvbuggy

tvbuggy_SOURCES = \
	Configuration.cc \
	Configuration.h \
	Channel.cc \
	Channel.h \
	HashTable.cc \
	HashTable.h \
	LinkedList.cc \
	LinkedList.h \
	main.cc \
	Node.cc \
	Node.h \
	Program.cc \
	Program.h \
	Schedule.h \
	Schedule.cc \
	ScheduledProgram.cc \
	ScheduledProgram.h \
	tvbuggy.h \
	utils.cc \
	utils.h

tvbuggy_LDADD = $(XML_LIBS)

INCLUDES = \
    -I$(top_srcdir) \
	-I$(includedir) \
	-I$(prefix)/include \
	$(XML_CFLAGS)

