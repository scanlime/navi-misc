PKGS	+= gtk+-2.0
CFLAGS	+= $(shell pkg-config --cflags $(PKGS))
LIBS	+= $(shell pkg-config --libs $(PKGS))

BIN	= henon-heiles

OBJS	= \
	henon-heiles-plot.o	\
	histogram-imager.o	\
	parameter-holder.o	\
	runge-kutta.o

$(BIN):	$(OBJS)
	gcc -o $@ $(OBJS) $(LIBS)

%.o:	%.c *.h
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(BIN) *.o
