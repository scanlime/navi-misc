bin_PROGRAMS = bzflagGTXserver

MAINTAINERCLEANFILES = \
	Makefile.in

bzflagGTXserver_SOURCES = \
	AccessControlList.cpp \
	AccessControlList.h \
	BZWError.cpp \
	BZWError.h \
	BZWReader.cpp \
	BZWReader.h \
	CmdLineOptions.cpp \
	CmdLineOptions.h \
	CustomBase.cpp \
	CustomBase.h \
	CustomBox.cpp \
	CustomBox.h \
	CustomGate.cpp \
	CustomGate.h \
	CustomLink.cpp \
	CustomLink.h \
	CustomPyramid.cpp \
	CustomPyramid.h \
	CustomWeapon.cpp \
	CustomWeapon.h \
	CustomWorld.cpp \
	CustomWorld.h \
	CustomZone.cpp \
	CustomZone.h \
	EntryZones.cpp \
	EntryZones.h \
	FlagHistory.cpp	\
	FlagHistory.h	\
	FlagInfo.cpp \
	FlagInfo.h \
	GameKeeper.cpp	\
	GameKeeper.h	\
	ListServerConnection.cpp \
	ListServerConnection.h \
	PackVars.h \
	Permissions.h \
	Permissions.cpp \
	RecordReplay.cpp \
	RecordReplay.h \
	RejoinList.cpp \
	RejoinList.h \
	Score.h	\
	Score.cpp	\
	SpawnPosition.cpp \
	SpawnPosition.h \
	TeamBases.cpp \
	TeamBases.h \
	TextChunkManager.cpp \
	TextChunkManager.h \
	WorldFileLocation.cpp \
	WorldFileLocation.h \
	WorldFileObject.cpp \
	WorldFileObject.h \
	WorldFileObstacle.cpp \
	WorldFileObstacle.h \
	WorldInfo.cpp \
	WorldInfo.h \
	WorldWeapons.cpp \
	WorldWeapons.h \
	commands.cpp \
	commands.h \
	bzfs.cpp \
	bzfs.h

if BUILD_REGEX
REGEX = \
	../regex/libregex.a
endif

LDADD = \
	../game/libGame.a	\
	../net/libNet.a		\
	../common/libCommon.a	\
	$(REGEX)

bzflagGTXserver_LDFLAGS = ../date/buildDate.o
