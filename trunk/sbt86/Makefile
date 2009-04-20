CFLAGS  = -Os -g `sdl-config --cflags`
LDFLAGS = -Os -g `sdl-config --libs`

PROGRAM = bt

OBJ = bt_tutorial.o bt_menu.o bt_play.o bt_game.o bt_lab.o sbt86.o

.PHONY: sizeprof all clean

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c *.h
	$(CC) -c -o $@ $< $(CFLAGS)

%.c: %.py sbt86.py bt_common.py
	python $<

sizeprof: $(PROGRAM)
	@nm --size-sort -S $< | egrep -v " [bBsS] "

clean:
	rm -f $(PROGRAM) $(OBJ) bt_*.c
