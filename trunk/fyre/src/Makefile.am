bin_PROGRAMS = fyre

INCLUDES = $(BINRELOC_CFLAGS) $(PACKAGE_CFLAGS) '-DGLADEDIR="data"'

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
	prefix.c

fyre_LDADD = $(BINRELOC_LIBS) $(PACKAGE_LIBS)
