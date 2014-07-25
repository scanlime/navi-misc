if CLIENT_INCLUDED
bin_PROGRAMS = opencombat
endif

MAINTAINERCLEANFILES = \
	Makefile.in

bzflagGTX_SOURCES =			\
	ActionBinding.cpp		\
	ActionBinding.h			\
	AudioMenu.cpp			\
	AudioMenu.h			\
	AutoPilot.cpp			\
	BackgroundRenderer.cpp		\
	BackgroundRenderer.h		\
	BaseLocalPlayer.cpp		\
	BaseLocalPlayer.h		\
	ComposeDefaultKey.cpp		\
	ComposeDefaultKey.h		\
	ControlPanel.cpp		\
	ControlPanel.h			\
	DisplayMenu.cpp			\
	DisplayMenu.h			\
	FlashClock.cpp			\
	FlashClock.h			\
	FormatMenu.cpp			\
	FormatMenu.h			\
	GuidedMissleStrategy.cpp	\
	GuidedMissleStrategy.h		\
	GUIOptionsMenu.cpp		\
	GUIOptionsMenu.h		\
	HelpMenu.cpp			\
	HelpMenu.h			\
	HUDDialog.cpp			\
	HUDDialog.h			\
	HUDDialogStack.cpp		\
	HUDDialogStack.h		\
	HUDRenderer.cpp			\
	HUDRenderer.h			\
	HUDui.cpp			\
	HUDui.h				\
	JoinMenu.cpp			\
	JoinMenu.h			\
	InputMenu.cpp			\
	InputMenu.h			\
	KeyboardMapMenu.cpp		\
	KeyboardMapMenu.h		\
	ListServer.h			\
	LocalPlayer.cpp			\
	LocalPlayer.h			\
	MainMenu.cpp			\
	MainMenu.h			\
	MainWindow.cpp			\
	MainWindow.h			\
	MenuDefaultKey.h		\
	MenuDefaultKey.cpp		\
	OptionsMenu.cpp			\
	OptionsMenu.h			\
	Player.cpp			\
	Player.h			\
	QuickKeysMenu.cpp		\
	QuickKeysMenu.h			\
	QuitMenu.cpp			\
	QuitMenu.h			\
	RadarRenderer.cpp		\
	RadarRenderer.h			\
	Region.cpp			\
	Region.h			\
	RegionPriorityQueue.cpp		\
	RegionPriorityQueue.h		\
	RemotePlayer.cpp		\
	RemotePlayer.h			\
	RobotPlayer.cpp			\
	RobotPlayer.h			\
	Roster.cpp			\
	Roster.h			\
	SaveWorldMenu.cpp		\
	SaveWorldMenu.h			\
	SceneBuilder.cpp		\
	SceneBuilder.h			\
	SceneRenderer.cpp		\
	SegmentedShotStrategy.cpp	\
	SegmentedShotStrategy.h		\
	ServerCommandKey.cpp		\
	ServerCommandKey.h		\
	ServerItem.cpp			\
	ServerItem.h			\
	ServerList.cpp			\
	ServerList.h			\
	ServerListCache.cpp		\
	ServerListCache.h		\
	ServerLink.cpp			\
	ServerLink.h			\
	ServerMenu.cpp			\
	ServerMenu.h			\
	ServerStartMenu.cpp		\
	ServerStartMenu.h		\
	ShockWaveStrategy.cpp		\
	ShockWaveStrategy.h		\
	ShotPath.cpp			\
	ShotPath.h			\
	ShotPathSegment.cpp		\
	ShotPathSegment.h		\
	ShotStrategy.cpp		\
	ShotStrategy.h			\
	SilenceDefaultKey.cpp		\
	SilenceDefaultKey.h		\
	StartupInfo.h			\
	TargetingUtils.cpp		\
	TargetingUtils.h		\
	World.cpp			\
	World.h				\
	WorldBuilder.cpp		\
	WorldBuilder.h			\
	WorldPlayer.cpp			\
	WorldPlayer.h			\
	bzflag.cpp			\
	callbacks.cpp			\
	callbacks.h			\
	clientCommands.cpp			\
	commands.h			\
	daylight.cpp			\
	daylight.h			\
	playing.cpp			\
	playing.h			\
	sound.cpp			\
	sound.h				\
	stars.cpp			\
	stars.h


if WIN32
RESOURCE = opencombat.res
else
RESOURCE =
endif

if BUILDZLIB
ZLIB = ../zlib/libz.a
else
ZLIB = -lz
endif

if MACOS
NACLIB =	\
	../platform/MacOSX/libMac.a
endif

if BUILD_REGEX
REGEX = \
	../regex/libregex.a
endif

LDADD =	\
	../3D/lib3D.a			\
	../net/libNet.a			\
	../common/libCommon.a		\
	../game/libGame.a		\
	../obstacle/libObstacle.a	\
	$(NACLIB)			\
	../platform/libPlatform.a	\
	../ogl/libGLKit.a		\
	../scene/libSceneDB.a		\
	../geometry/libGeometry.a	\
	../mediafile/libMediaFile.a	\
	$(REGEX)			\
	$(ZLIB)				\
	$(RESOURCE)			\
	$(SDL_LIBS)			\
	$(GLIBS)
# This is required to hook in ogg/vorbis libraries
#	$(ALIBS)

AM_CPPFLAGS = $(X_CFLAGS) $(SDL_CFLAGS)

opencombat_LDFLAGS = ../date/buildDate.o

opencombat.res: $(top_srcdir)/win32/bzflag.rc $(top_srcdir)/win32/opencombat.ico
	windres --include-dir=$(top_srcdir)/win32/ -i $(top_srcdir)/win32/opencombat.rc -o opencombat.res \
	-O coff
