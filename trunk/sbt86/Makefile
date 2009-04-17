#BIN = MENU.EXE
BIN = LAB.EXE

sbt: sbt.c sbt86.h
	gcc -o sbt sbt.c -g `sdl-config --cflags --libs` \
		2>&1 | grep -v "comparison is always"

sbt.c: sbt86.py $(BIN)
	python sbt86.py $(BIN) > sbt.c

