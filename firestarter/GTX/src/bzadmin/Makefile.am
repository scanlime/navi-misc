AM_cppFLAGS = -DBUILDING_BZADMIN
INCLUDES = -I@top_srcdir@/src/bzflag

MAINTAINERCLEANFILES = \
	ServerLink.cpp \
	Makefile.in

if BZADMIN_INCLUDED
bin_PROGRAMS = bzadmin
endif

if HAVE_CURSES
CURSES_SRC = CursesUI.h CursesUI.cpp CursesMenu.h CursesMenu.cpp
else
CURSES_SRC =

EXTRA_bzadmin_SOURCES = \
	CursesMenu.h \
	CursesMenu.cpp \
	CursesUI.h \
	CursesUI.cpp
endif

bzadmin_SOURCES = \
	AutoCompleter.h \
	AutoCompleter.cpp \
	BZAdminClient.h \
	BZAdminClient.cpp \
	BZAdminUI.h \
	BZAdminUI.cpp \
	colors.h \
	$(CURSES_SRC) \
	OptionParser.h \
	OptionParser.cpp \
	PlayerInfo.h \
	ServerLink.cpp \
	StdBothUI.h \
	StdBothUI.cpp \
	StdInUI.h \
	StdInUI.cpp \
	StdOutUI.h \
	StdOutUI.cpp \
	UIMap.h \
	UIMap.cpp \
	bzadmin.cpp \
	curses_wrapper.h

ServerLink.cpp: @top_srcdir@/src/bzflag/ServerLink.cpp
	cp -pR @top_srcdir@/src/bzflag/ServerLink.cpp .

LDADD = \
	@CURSES_LIB@ \
	../net/libNet.a			\
	../common/libCommon.a

bzadmin_LDFLAGS = ../date/buildDate.o
