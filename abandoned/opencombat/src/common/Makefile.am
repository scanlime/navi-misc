noinst_LIBRARIES = libCommon.a

AM_CFLAGS = -D_XOPEN_SOURCE

libCommon_a_SOURCES  =			\
	BZDBCache.cpp			\
	Bundle.cpp			\
	BundleMgr.cpp			\
	CommandManager.cpp		\
	ConfigFileManager.cpp		\
	Country.cpp			\
	ErrorHandler.cpp		\
	FileManager.cpp			\
	Flag.cpp			\
	KeyManager.cpp			\
	Language.cpp			\
	OSFile.cpp			\
	PlayerState.cpp			\
	PositionTracker.cpp		\
	ShotUpdate.cpp			\
	StateDatabase.cpp		\
	Team.cpp			\
	TimeBomb.cpp			\
	TimeKeeper.cpp			\
	VotingBooth.cpp			\
	WordFilter.cpp			\
	bz_Locale.cpp			\
	bzfio.cpp			\
	bzsignal.c			\
	global.cpp			\
	md5.cpp			\
	GeoUtils.cpp

if BUILD_REGEX
libCommon_a_LIBADD = \
	../regex/libregex.a
endif

MAINTAINERCLEANFILES = \
	Makefile.in

EXTRA_DIST = \
	README
