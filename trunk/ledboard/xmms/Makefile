CC = gcc
INCLUDEDIR = -I/usr/local/include/xmms -I/usr/X11R6/share/include -I/usr/include/gtk-1.2
CFLAGS =   -O2 -Wall $(INCLUDEDIR) -fPIC `glib-config --cflags`
LFLAGS =   -shared -fPIC -lpthread -L/usr/local/lib -ldl -lm -L/usr/X11R6/lib -lXpm -lX11

ledboard.so: ledboard.o
	$(CC)  -o ledboard.so ledboard.o $(LFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< 

clean:
	rm -f *.o core *.so* 
