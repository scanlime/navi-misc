CFLAGS  = `pkg-config --cflags gtk+-2.0` -O3 -g -ffast-math -march=i686
LIBS    = `pkg-config --libs gtk+-2.0`

BIN     = de-jong-explorer
OBJS    = de-jong-explorer.o color-button.o
HEADERS = src/color-button.h

$(BIN): $(OBJS)
	gcc -o $@ $(OBJS) $(LIBS)

.c.o: $(HEADERS)
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) *.o
