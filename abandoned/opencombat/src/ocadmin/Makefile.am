AM_cppFLAGS = -DBUILDING_OCADMIN
INCLUDES = -I@top_srcdir@/src/opencombat

MAINTAINERCLEANFILES = \
	ServerLink.cpp \
	Makefile.in

if OCADMIN_INCLUDED
bin_PROGRAMS = ocadmin
endif

if HAVE_CURSES
CURSES_SRC = CursesUI.h CursesUI.cpp CursesMenu.h CursesMenu.cpp
else
CURSES_SRC =

EXTRA_ocadmin_SOURCES = \
	CursesMenu.h \
	CursesMenu.cpp \
	CursesUI.h \
	CursesUI.cpp
endif

ocadmin_SOURCES = \
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

ServerLink.cpp: @top_srcdir@/src/opencombat/ServerLink.cpp
	cp -pR @top_srcdir@/src/opencombat/ServerLink.cpp .

LDADD = \
	@CURSES_LIB@ \
	../net/libNet.a			\
	../common/libCommon.a

ocadmin_LDFLAGS = ../date/buildDate.o
