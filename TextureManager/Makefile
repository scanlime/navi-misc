CFLAGS				= -Wall -c -g

../lib/TextureManager.a:	TextureMngr.o FileLoaders.o SMELoad.o
				ar rc ../lib/TextureManager.a TextureMngr.o FileLoaders.o SMELoad.o

TextureMngr.o:			TextureMngr.h TextureMngr.cpp
				g++ $(CFLAGS) -DDEBUG TextureMngr.cpp

FileLoaders.o:			TextureMngr.h FileLoaders.cpp
				g++ $(CFLAGS) FileLoaders.cpp

SMELoad.o:			SMELoad.h SMELoad.cpp
				g++ $(CFLAGS) SMELoad.cpp

clean:
				rm -f *.o ../lib/TextureManager.a
