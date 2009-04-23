# Normal CFLAGS and LDFLAGS for the program as a whole
CFLAGS = -Os -g `sdl-config --cflags`
LDFLAGS = `sdl-config --libs`

# Special CFLAGS for sbt-generated code. We care less about this code
# being debuggable, and effective size optimization really matters.
# These options are important! See the gcc manpage for details.
#
# This can make compiles take a really long time. For debugging, it
# might be useful to specify an empty CFLAGS_BT on the command line.
#CFLAGS_BT = -Os -fweb -frtl-abstract-sequences -fomit-frame-pointer

PROGRAM = bt

OBJ_MAIN = emulator.o
#OBJ_BT = bt_game.o bt_lab.o bt_menu.o bt_play.o bt_tutorial.o
OBJ_BT = bt_tutorial.o

OBJ_ALL = $(OBJ_MAIN) $(OBJ_BT)

.PHONY: sizeprof all clean

all: $(PROGRAM)

$(PROGRAM): $(OBJ_ALL)
	$(CC) -o $@ $(OBJ_ALL) $(LDFLAGS)

$(OBJ_MAIN): %.o: %.c *.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_BT): %.o: %.c sbt86.h
	$(CC) -c -o $@ $< $(CFLAGS_BT)

%.c: %.py sbt86.py bt_common.py
	python $<

sizeprof: $(PROGRAM)
	@nm --size-sort -S $< | egrep -v " [bBsS] "

clean:
	rm -f $(PROGRAM) $(OBJ_ALL) bt_*.c
