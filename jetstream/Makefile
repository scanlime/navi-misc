# Quick hack of a makefile (tm)


OBJS = \
	src/Spline.o		\
	src/SplineSceneNode.o	\
	src/Noise.o		\
	src/MathT.o		\
	src/SurfaceInverter.o	\
	src/ColorModulator.o	\
	src/SceneLights.o	\
	src/PerlinDisplacement.o \
	src/ViewingFrustum.o	\
	src/SurfaceGenerator.o	\
	src/Surface.o		\
	src/DetailSettings.o	\
	src/SphereGenerator.o	\
	src/SuperquadricGenerator.o \
	src/Lightmap.o		\
	src/JetCOWRegistry.o	\
	src/JetCOW.o		\
	src/Mutex.o		\
	src/Engine.o		\
	src/EmbeddedPGserver.o	\
	src/PGTexture.o		\
	src/Camera.o		\
	src/Skybox.o		\
	src/VideoRecorder.o	\
	src/nuppelvideo/RTjpegN.o \
	src/nuppelvideo/minilzo.o \
	src/nuppelvideo/NuppelWriter.o \
	src/nuppelvideo/rgb2yuv420.o \
	src/ParticleSystem.o	\
	src/Scene.o		\
	src/AnimatedVector.o	\
	src/Quat.o		\
	src/TriangleGenerator.o	\
	src/QuadGenerator.o	\
	src/main.o		\
	src/SoundManagerNew.o	\
	src/PythonWrapper.o

HEADERS = \
	src/Spline.h		\
	src/SplineSceneNode.h	\
	src/MathT.h		\
	src/SurfaceInverter.h	\
	src/ColorModulator.h	\
	src/SceneLights.h	\
	src/VertexBuffer.h	\
	src/Matrix.h		\
	src/Surface.h		\
	src/SphereGenerator.h	\
	src/SuperquadricGenerator.h \
	src/JetCOW.h		\
	src/JetCOWRegistry.h	\
	src/EmbeddedPGserver.h	\
	src/PGTexture.h		\
	src/Camera.h		\
	src/Skybox.h		\
	src/ConfigProvider.h	\
	src/Engine.h		\
	src/ParticleSystem.h	\
	src/Util.h		\
	src/VideoRecorder.h	\
	src/Vector.h		\
	src/nuppelvideo/RTjpegN.h \
	src/nuppelvideo/minilzo.h \
	src/nuppelvideo/lzoconf.h \
	src/nuppelvideo/nuppelvideo.h \
	src/nuppelvideo/NuppelWriter.h \
	src/ParticleSystem.h	\
	src/Scene.h		\
	src/Mutex.h		\
	src/AnimatedVector.h	\
	src/Quat.h		\
	src/Noise.h		\
	src/Lightmap.h		\
	src/TriangleGenerator.h	\
	src/QuadGenerator.h	\
	src/SoundManagerNew.h	\
	src/DetailSettings.h	\
	src/ViewingFrustum.h	\
	src/SurfaceGenerator.h	\
	src/PerlinDisplacement.h


CXXFLAGS += -Isrc

BIN = jetstream

# Use ccache if it's available
CXX = g++
CC = gcc
PATH := /usr/bin/ccache:$(PATH)

# Embedding pgserver:
# We could use a dynamic library for pgserver, but in a
# real world situation it's better to statically link it
# since this program will require a specific configuration
# of pgserver.

# Libs for a static pgserver
#LIBS += -lGL `sdl-config --libs` -ljpeg -lpng -lfreetype /usr/lib/libpgserver.a -lutil -lGLU -lopenal
#CXXFLAGS += `sdl-config --cflags`

# Libs for a dynamic pgserver
LIBS += -lpgserver -lutil -lopenal
CXXFLAGS += `sdl-config --cflags`

# Statically link with python
LIBS += /usr/lib/python2.2/config/libpython2.2.a -export-dynamic

# Shut up about the warning db_cxx.h causes
CXXFLAGS += -Wno-deprecated

# The FMOD sound library
#LDFLAGS += -lfmod-3.6
#CXXFLAGS += -Ifmod

# libdb3 database, c++ interface
LDFLAGS += -ldb_cxx-3.2

# Boost.Python library for python integration
CXXFLAGS += -I/usr/include/python2.2
LDFLAGS += -lboost_python

# Debugging
# (This makes jetstream take terribly long to link, due to Boost cruft!)
#CXXFLAGS += -g

# Optimization
# (It will run VERY much slower without inlining, so you almost certainly
#  don't ever want to compile this without -O3!)
#
# Recommended optimizations:
#
OPTFLAGS  = -O3 -fschedule-insns -falign-functions=4
OPTFLAGS += -funroll-all-loops -finline-limit=800 -ffast-math -fforce-addr
#
# CPU-brand-specific optimization:
#
INTELFLAGS = -march=pentium4 -fprefetch-loop-arrays -mfpmath=sse
AMDFLAGS   = -march=athlon-xp -fprefetch-loop-arrays -mfpmath=sse
#OPTFLAGS += $(shell grep -qi intel /proc/cpuinfo && echo "$(INTELFLAGS)")
OPTFLAGS += $(shell grep -qi amd /proc/cpuinfo   && echo "$(AMDFLAGS)")
CXXFLAGS += $(OPTFLAGS)
CFLAGS += $(OPTFLAGS)

# gprof profiling
#CXXFLAGS += -pg
#CFLAGS += -pg
#LDFLAGS += -pg

# Static linking
#LDFLAGS += -static

# Relative to the media_src directory
XWTCOMPILE = ../util/xwtcompile.py

all: $(BIN) data

data: \
	data/holographic.th data/textures.th data/mainmenu.wt data/hud.wt data/console.wt \
	data/term_c64.th data/term_retro.th data/term_wet.th data/term_white.th

$(BIN): $(OBJS)
	$(CXX) -o $@ $(LDFLAGS) $(OBJS) $(LIBS) 

%.o: %.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

data/holographic.th: media_src/holographic.ths
	cd media_src && themec holographic.ths && mv holographic.th ../data

data/textures.th: media_src/textures.ths
	cd media_src && themec textures.ths && mv textures.th ../data

data/term_c64.th: media_src/term_c64.ths
	cd media_src && themec term_c64.ths && mv term_c64.th ../data

data/term_retro.th: media_src/term_retro.ths
	cd media_src && themec term_retro.ths && mv term_retro.th ../data

data/term_wet.th: media_src/term_wet.ths
	cd media_src && themec term_wet.ths && mv term_wet.th ../data

data/term_white.th: media_src/term_white.ths
	cd media_src && themec term_white.ths && mv term_white.th ../data

data/mainmenu.wt: media_src/mainmenu.xwt
	cd media_src && $(XWTCOMPILE) mainmenu.xwt ../data/mainmenu.wt

data/hud.wt: media_src/hud.xwt
	cd media_src && $(XWTCOMPILE) hud.xwt ../data/hud.wt

data/console.wt: media_src/console.xwt
	cd media_src && $(XWTCOMPILE) console.xwt ../data/console.wt

clean:
	rm -f $(OBJS) $(BIN) data/*.th data/*.wt

it:	all
work:
so:
