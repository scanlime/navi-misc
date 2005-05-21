BIN = test

ALL: $(BIN)

test: optparse.o
	g++ -o test main.cpp optparse.o

optparse.o:
	g++ -c optparse.cpp

clean:
	rm *.o $(BIN)
