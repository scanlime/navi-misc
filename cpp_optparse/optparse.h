/*
 * optparse.h
 *
 * An option parser for C++ modeled after the optparse Python
 * module. Create an instance of OptionParser in your program
 * and you can add options to it then call the method parse_args
 * with argc and argv and it will parse them. The '-h' and '--help'
 * options are built in, they print the usage message.
 *
 * W. Evan Sheehan <evan_sheehan@nrel.gov>
 *
 */

#include <map.h>
#include <vector.h>
#include <string>

class Option
{
	public:
		Option (std::string shrt, std::string lng, std::string dest);
		~Option ();

		/* Override the == operator so that when you compare an Option
		 * to a string it will check the string against shrt_flag and lng_flag
		 * and return true if either match.
		 */
		bool operator == (const std::string & option);

		std::string shrt_flag;		// short flag, something like -o
		std::string lng_flag;			// long flag, something like --outfile
		std::string destination;	// the key used to store this option in the
															// parser.
};

class OptionParser
{
	public:
		OptionParser (std::string usage="");
		~OptionParser ();

		/* Add an option to the parser. */
		void add_option (std::string shrt_flag, std::string lng_flag, std::string destination, std::string dfault="");
		/* Parse the commandline args. */
		void parse_args (int argc, char **argv);
		/* Print an error message: this->use_msg followed by msg. Exit. */
		void error (std::string msg);
		/* Print the help message (use_msg) and exit. */
		void help ();

		map<std::string, std::string> options;	// Dictionary of the options
		vector<std::string> arguments;			// List of the arguments with no flag

	private:
		std::string use_msg;	// usage message.
		vector<Option> opts;	// List of the options the parser knows about.

		/* Helper for finding which Option (if any) argv[index] matches. */
		void find_opt (char **argv, int index);
};

/*** The End ***/

