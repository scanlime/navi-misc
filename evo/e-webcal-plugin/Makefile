all: e-calendar-http.so

e-calendar-http.o: e-calendar-http.c
	$(CC) -g -o $@ $^ -shared -c `pkg-config --cflags glib-2.0 libxml-2.0 gtk+-2.0 libgnome-2.0 evolution-shell-2.2 evolution-data-server-1.2` -I/usr/include/evolution-data-server-1.2

e-calendar-http.so: e-calendar-http.o
	$(CC) -shared -o $@ $^ `pkg-config --libs glib-2.0 libxml-2.0 gconf-2.0 gtk+-2.0 libgnome-2.0 evolution-data-server-1.2`

