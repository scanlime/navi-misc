all:	trackd.i
	swig -python trackd.i
	gcc -c trackd_wrap.c -I/usr/freeware/include/python2.2 -I/usr/local/trackd/trackdAPI/include
	ld -shared trackd_wrap.o -o _trackd.so /usr/local/trackd/trackdAPI/lib32/libtrackdAPI.a /usr/lib32/libC.so
