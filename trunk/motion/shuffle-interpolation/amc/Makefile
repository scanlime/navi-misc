install: splice shiftsplice
	@cp -v splice ../bin/
	@cp -v shiftsplice ../bin/

splice: splice.o amc.o 
	g++ -Wall -o splice splice.o amc.o 

shiftsplice: shiftsplice.o amc.o
	g++ -Wall -o shiftsplice shiftsplice.o amc.o

shiftsplice.o: shiftsplice.cxx amc.cxx amc.h
	g++ -Wall -c shiftsplice.cxx

splice.o: splice.cxx amc.h
	g++ -Wall -c splice.cxx

amc.o: amc.cxx amc.h
	g++ -Wall -c amc.cxx


clean:
	rm -vf splice shiftsplice *.o

