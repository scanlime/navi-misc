CFLAGS =   -O2 -Wall -fPIC `xmms-config --cflags` -I ../include
LFLAGS =   -shared -fPIC -lpthread `xmms-config --cflags`

rwand-analyzer.so: rwand-analyzer.o
	$(CC)  -o rwand-analyzer.so rwand-analyzer.o $(LFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o core *.so*
