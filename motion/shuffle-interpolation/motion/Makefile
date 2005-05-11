install: shuffle
	@cp -v shuffle ../bin/

shuffle: shuffle.o runge.o vect.o function.o 
	g++ shuffle.o runge.o vect.o function.o ../amc/amc.o -o shuffle

shuffle.o: shuffle.cxx
	g++ -c shuffle.cxx

runge.o: runge.cxx runge.h
	g++ -c runge.cxx

vect.o: vect.cxx vect.h
	g++ -c vect.cxx

function.o: function.cxx function.h
	g++ -c function.cxx

inter.o: inter.cxx inter.h
	g++ -c inter.cxx

clean:
	rm -vf shuffle *.o

