install: shuffle
	@cp -v shuffle ../bin/

shuffle: shuffle.o runge.o vect.o function.o 
	g++ -g shuffle.o runge.o vect.o function.o ../amc/amc.o -o shuffle

shuffle.o: shuffle.cxx
	g++ -g -c shuffle.cxx

runge.o: runge.cxx runge.h
	g++ -g -c runge.cxx

vect.o: vect.cxx vect.h
	g++ -g -c vect.cxx

function.o: function.cxx function.h
	g++ -g -c function.cxx

inter.o: inter.cxx inter.h
	g++ -g -c inter.cxx

clean:
	rm -vf shuffle *.o

