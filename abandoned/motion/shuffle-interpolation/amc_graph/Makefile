install: creategraph
	@cp -v creategraph ../bin/

creategraph: creategraph.o amc_graph.o
	g++ -g -Wall -o creategraph creategraph.o amc_graph.o ../graph_node/graph_node.o ../amc/amc.o

creategraph.o: creategraph.cxx ../graph_interp/conversion.h
	g++ -g -Wall -c creategraph.cxx

amc_graph.o: amc_graph.cxx amc_graph.h ../graph_node/graph_node.o
	g++ -g -Wall -c amc_graph.cxx

#../graph_interp/conversion.h:
#	../graph_interp/build_convert.pl > ../graph_interp/conversion.h

../graph_node/graph_node.o:
	make -f ../graph_node/Makefile

clean:
	rm -fv creategraph *.o
