# -march=i686 speeds this up quite a bit on my machine (even more so
# than -march=athlon-xp) so if this looks like a recent x86 machine,
# stick that in CFLAGS
CFLAGS  += $(shell if grep mmx /proc/cpuinfo > /dev/null; then echo -march=i686; fi)

# -O3 and -ffast-math should make it go much faster on any system
CFLAGS  += -O3 -ffast-math

# Disable glibc versions of functions that have faster versions
# as gcc inlines. This should speed up trig on some systems.
CFLAGS  += -D__NO_INLINE__

PKGS    += libglade-2.0 gtk+-2.0
CFLAGS  += '-DGLADEDIR="data"'
CFLAGS  += $(shell pkg-config --cflags $(PKGS))
LIBS    += $(shell pkg-config --libs $(PKGS))

BIN     = fyre

OBJS    = \
	src/main.o		\
	src/de-jong.o		\
	src/explorer.o		\
	src/color-button.o	\
	src/animation.o		\
	src/chunked-file.o	\
	src/curve-editor.o	\
	src/spline.o		\
	src/explorer-tools.o	\
	src/explorer-animation.o \
	src/cell-renderer-transition.o \
	src/cell-renderer-bifurcation.o \
	src/histogram-imager.o	\
	src/iterative-map.o \
	src/parameter-holder.o	\
	src/bifurcation-diagram.o \
	src/math-util.o		\
	src/avi-writer.o	\
	src/parameter-editor.o	\
	src/histogram-view.o	\
	src/animation-render-ui.o \
	src/screensaver.o


$(BIN): $(OBJS)
	gcc -o $@ $(OBJS) $(LIBS)

%.o: %.c src/*.h
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) src/*.o
