/*
 * optparse.h
 *
 * An option parser for C++ modeled after the optparse Python
 * module. Create an instance of OptionParser in your program
 * and you can add options to it then call the method
 * parse_args with argc and argv and it will parse them. The
 * '-h' and '--help' options are built in, they print the
 * usage message.
 *
 * W. Evan Sheehan <evan_sheehan@nrel.gov>
 *
 */

#include <map.h>
#include <vector.h>
#include <string>

/* Use this type to determine what we're storing, naming
 * convention is the same as the optparse python module.
 * Even with STORE_FALSE and STORE_TRUE the values are
 * going to be stored as strings, false is "0" true is "1"
 * so you can just atoi() the value and use that for tests.
 */
typedef enum {STORE_FALSE=0, STORE_TRUE, STORE} action_t;

class Option
{
	public:
		Option (std::string shrt, std::string lng, std::string dest,
				action_t act=STORE, std::string hlp="");
		~Option ();

		/* Override the == operator so that when you compare an Option
		 * to a string it will check the string against shrt_flag and lng_flag
		 * and return true if either match.
		 */
		bool operator == (const std::string & option);

		action_t action;					// define what we store
		std::string shrt_flag;		// short flag, something like -o
		std::string lng_flag;			// long flag, something like --outfile
		std::string help;					// help message about the option
		std::string destination;	// the key used to store this option in the
															// parser.
};

class OptionParser
{
	public:
		OptionParser (std::string usage="", bool show_opts=true);
		~OptionParser ();

		/* Add an option to the parser. */
		void add_option (std::string shrt_flag, std::string lng_flag,
				std::string destination, action_t act=STORE, std::string dfault="", std::string help="");
		/* Parse the commandline args. */
		void parse_args (int argc, char **argv);
		/* Print an error message: this->use_msg followed by msg. Exit. */
		void error (std::string msg);
		/* Print the help message (use_msg) and exit. */
		void help ();

		map<std::string, std::string> options;	// Dictionary of the options
		vector<std::string> arguments;			// List of the arguments with no flag

	private:
		bool help_msg_show_opts;
		std::string use_msg;	// usage message.
		vector<Option> opts;	// List of the options the parser knows about.

		/* Helper for finding which Option (if any) argv[index] matches. */
		void find_opt (char **argv, int &index);
};

/*** The End ***/

