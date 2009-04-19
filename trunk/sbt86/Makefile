CFLAGS  = -Os -g `sdl-config --cflags`
LDFLAGS = -Os -g `sdl-config --libs`

PROGRAM = bt

OBJ = bt_tutorial.o sbt86.o

.PHONY: sizeprof all clean

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c *.h
	$(CC) -c -o $@ $< $(CFLAGS)

bt_tutorial.c: bt_tutorial.py sbt86.py TUT.EXE
	python $<

sizeprof: $(PROGRAM)
	@nm --size-sort -S $< | egrep -v " [bBsS] "

clean:
	rm -f $(PROGRAM) $(OBJ) bt_*.c
