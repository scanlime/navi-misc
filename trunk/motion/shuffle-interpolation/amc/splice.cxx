#include <stdlib.h>
#include <iostream>
#include <stdlib.h>

#include "amc.h"

int
main(int argc, char **argv)
{
	if (argc != 3) {
		cerr << "Usage: splice amc1 amc2" << endl;
		exit(EXIT_FAILURE);
	}

	amc a(argv[1]);
	amc b(argv[2]);
	amc c(a,b);

	cout << "#!FILE PROCESSED" << endl;
	cout << ":FULLY-SPECIFIED" << endl;
	cout << ":DEGREES" << endl;

	c.dumpall();

	return EXIT_SUCCESS;
}
