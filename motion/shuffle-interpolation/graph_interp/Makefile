install: interpolate
	@cp -v interpolate ../bin/

interpolate: interpolate.o
	g++ -g -Wall interpolate.o ../amc/amc.o ../amc_graph/amc_graph.o ../graph_node/graph_node.o -o interpolate


interpolate.o: interpolate.cxx conversion.h
	g++ -g -Wall -c interpolate.cxx

conversion.h:
	./build_convert.pl > conversion.h

clean:
	rm -fv interpolate conversion.h *.o



