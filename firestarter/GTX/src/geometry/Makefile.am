SUBDIRS = models

MAINTAINERCLEANFILES = \
	Makefile.in

if CLIENT_INCLUDED
noinst_LIBRARIES = libGeometry.a
endif

libGeometry_a_SOURCES  =		\
	ViewFrustum.cpp			\
	Ray.cpp				\
	SceneNode.cpp			\
	WallSceneNode.cpp		\
	QuadWallSceneNode.cpp		\
	TriWallSceneNode.cpp		\
	PolyWallSceneNode.cpp		\
	EighthDimSceneNode.cpp		\
	EighthDBaseSceneNode.cpp	\
	EighthDBoxSceneNode.cpp		\
	EighthDPyrSceneNode.cpp		\
	ShellSceneNode.cpp		\
	TracerSceneNode.cpp		\
	PTSceneNode.cpp			\
	BoltSceneNode.cpp		\
	LaserSceneNode.cpp		\
	SphereSceneNode.cpp		\
	FlagSceneNode.cpp		\
	FlagWarpSceneNode.cpp		\
	TankSceneNode.cpp		\
	BillboardSceneNode.cpp

libGeometry_a_LIBADD =	\
	models/tank/high_barrel.o	\
	models/tank/high_body.o		\
	models/tank/high_ltread.o	\
	models/tank/high_rtread.o	\
	models/tank/high_turret.o	\
	models/tank/low_barrel.o	\
	models/tank/low_body.o		\
	models/tank/low_ltread.o	\
	models/tank/low_rtread.o	\
	models/tank/low_turret.o	\
	models/tank/medium_barrel.o	\
	models/tank/medium_body.o	\
	models/tank/medium_ltread.o	\
	models/tank/medium_rtread.o	\
	models/tank/medium_turret.o

EXTRA_DIST = \
	README