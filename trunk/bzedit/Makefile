CC			= gcc
CXX			= g++
CFLAGS			= -Wall -g `pkg-config --cflags libgnomeui-2.0 gtkgl-2.0 gdl-1.0` -Iinclude -ITextureManager
LDFLAGS			= `pkg-config --libs libgnomeui-2.0 gtkgl-2.0 gdl-1.0 libgsf-1`
CELLRCFLAGS		= -Wall -g `pkg-config --cflags libbonobo-2.0 libgnome-2.0 gtk+-2.0`
CELLRLDFLAGS		= `pkg-config --libs libbonobo-2.0`
OBJECTS			= cell-renderer-captioned-image.o cell-renderer-toggle-image.o bzedit.o application.o toolbar.o editboxes.o listbox.o viewbox.o preferences.o 3dtypes.o model.o globals.o object.o map.o camera.o world.o document.o walls.o ground.o box.o pyramid.o teleporter.o base.o
LIBS			= lib/TextureManager.a
EXE			= bzedit

$(EXE):			$(OBJECTS) $(LIBS)
			$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) $(CELLRLDFLAGS) $(LIBS)

clean:
			rm -f *.o $(EXE) core

bzedit.o:		bzedit.cpp
application.o:		application.cpp
toolbar.o:		toolbar.cpp
editboxes.o:		editboxes.cpp
listbox.o:		listbox.cpp
viewbox.o:		viewbox.cpp
preferences.o:		preferences.cpp
3dtypes.o:		3dtypes.cpp
model.o:		model.cpp
globals.o:		globals.cpp
object.o:		object.cpp
map.o:			map.cpp
camera.o:		camera.cpp
world.o:		world.cpp
document.o:		document.cpp
walls.o:		walls.cpp
ground.o:		ground.cpp
box.o:			box.cpp
pyramid.o:		pyramid.cpp
teleporter.o:		teleporter.cpp
base.o:			base.cpp

cell-renderer-captioned-image.o:	cell-renderer-captioned-image.c
					$(CC) $(CELLRCFLAGS) -c -o $@ $<

cell-renderer-toggle-image.o:		cell-renderer-toggle-image.c
					$(CC) $(CELLRCFLAGS) -c -o $@ $<

.cpp.o:
			$(CXX) $(CFLAGS) -c -o $@ $<
