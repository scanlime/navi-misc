CFLAGS =   -O2 -Wall -fPIC `xmms-config --cflags` -I ../../include
LFLAGS =   -shared -fPIC -lpthread `xmms-config --cflags`

rwand-brandon.so: rwand-brandon.o
	$(CC)  -o rwand-brandon.so rwand-brandon.o $(LFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o core *.so*
