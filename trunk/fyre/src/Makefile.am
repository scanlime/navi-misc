bin_PROGRAMS = fyre

INCLUDES = $(BINRELOC_CFLAGS) $(PACKAGE_CFLAGS) $(EXR_CFLAGS) '-DGLADEDIR="data"'

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
	cell-renderer-transition.c	\
	cell-renderer-bifurcation.c	\
	histogram-imager.c		\
	iterative-map.c			\
	parameter-holder.c		\
	bifurcation-diagram.c		\
	math-util.c			\
	avi-writer.c			\
	parameter-editor.c		\
	histogram-view.c		\
	animation-render-ui.c		\
	screensaver.c			\
	prefix.c			\
	exr.cpp

noinst_HEADERS =			\
	animation.h			\
	animation-render-ui.h		\
	avi-writer.h			\
	bifurcation-diagram.h		\
	cell-renderer-bifurcation.h	\
	cell-renderer-transition.h	\
	chunked-file.h			\
	color-button.h			\
	curve-editor.h			\
	de-jong.h			\
	explorer.h			\
	histogram-imager.h		\
	histogram-view.h		\
	iterative-map.h			\
	math-util.h			\
	parameter-editor.h		\
	parameter-holder.h		\
	prefix.h			\
	screensaver.h			\
	spline.h			\
	var-int.h

fyre_LDADD = \
	$(BINRELOC_LIBS)		\
	$(PACKAGE_LIBS)			\
	$(EXR_LIBS)

