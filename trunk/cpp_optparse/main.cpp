/*
 * main.cpp
 *
 * Quick little test of my optparse class.
 *
 * W. Evan Sheehan <evan_sheehan@nrel.gov>
 *
 */

#include <iostream>

#include "optparse.h"

int
main (int argc, char **argv)
{
	OptionParser parser("usage: test [options] dummy");

	parser.add_option("-i", "--infile", "infile");
	parser.add_option("-s", "", "savefile");
	parser.add_option("","-boo", "boo", "3");

	parser.parse_args(argc, argv);
	if (parser.arguments.size() < 1)
		parser.error("Incorrect number of arguments.");

	cout << "infile: " << parser.options["infile"] << endl
		<< "boo: " << parser.options["boo"] << endl
		<< "savefile: " << parser.options["savefile"] << endl;

	return 0;
}
/*** The End ***/

