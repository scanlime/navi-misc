bin_PROGRAMS = fyre

INCLUDES = \
	$(BINRELOC_CFLAGS)		\
	$(PACKAGE_CFLAGS)		\
	$(EXR_CFLAGS)			\
	$(GNET_CFLAGS)			\
	'-DFYRE_DATADIR="data"'		\
	'-DDATADIR="$(prefix)/share"'

fyre_LDADD = \
	$(BINRELOC_LIBS)		\
	$(PACKAGE_LIBS)			\
	$(EXR_LIBS)			\
	$(GNET_LIBS)			\
	$(WIN32_LIBS)

fyre_DEPENDENCIES = \
	$(WIN32_LIBS)

fyre-win32-res.o: fyre.rc *.ico
	$(WINDRES) fyre.rc fyre-win32-res.o

if HAVE_EXR
EXR_SRC = \
	exr.cpp
else
EXR_SRC =
endif

if HAVE_GNET
GNET_SRC = \
	remote-server.c			\
	remote-client.c			\
	discovery-server.c		\
	discovery-client.c		\
	explorer-cluster.c		\
	cluster-model.c
else
GNET_SRC =
endif

if HAVE_GETOPT
GETOPT_SRC =
else
GETOPT_SRC = \
	getopt.c \
	getopt1.c
endif

fyre_SOURCES = 				\
	main.c				\
	de-jong.c			\
	explorer.c			\
	color-button.c			\
	animation.c			\
	chunked-file.c			\
	curve-editor.c			\
	spline.c			\
	explorer-tools.c		\
	explorer-animation.c		\
	explorer-about.c		\
	explorer-history.c		\
	cell-renderer-transition.c	\
	cell-renderer-bifurcation.c	\
	histogram-imager.c		\
	iterative-map.c			\
	parameter-holder.c		\
	bifurcation-diagram.c		\
	math-util.c			\
	gui-util.c			\
	avi-writer.c			\
	parameter-editor.c		\
	histogram-view.c		\
	animation-render-ui.c		\
	screensaver.c			\
	batch-image-render.c		\
	probability-map.c		\
	prefix.c			\
	image-fu.c			\
	$(EXR_SRC)			\
	$(GETOPT_SRC)			\
	$(GNET_SRC)

noinst_HEADERS =			\
	animation.h			\
	animation-render-ui.h		\
	avi-writer.h			\
	batch-image-render.h		\
	bifurcation-diagram.h		\
	cell-renderer-bifurcation.h	\
	cell-renderer-transition.h	\
	chunked-file.h			\
	cluster-model.h			\
	color-button.h			\
	curve-editor.h			\
	de-jong.h			\
	explorer.h			\
	gui-util.h			\
	histogram-imager.h		\
	histogram-view.h		\
	iterative-map.h			\
	math-util.h			\
	parameter-editor.h		\
	parameter-holder.h		\
	prefix.h			\
	screensaver.h			\
	spline.h			\
	var-int.h			\
	remote-server.h			\
	remote-client.h			\
	probability-map.h		\
	discovery-server.h		\
	discovery-client.h              \
        platform.h			\
	image-fu.h
