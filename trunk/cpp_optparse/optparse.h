/*
 * optparse.h
 *
 * An option parser for C++ modeled after the optparse Python
 * module.
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

		bool operator == (const std::string & option);

		std::string shrt_flag;
		std::string lng_flag;
		std::string destination;
};

class OptionParser
{
	public:
		OptionParser (std::string usage="");
		~OptionParser ();

		void add_option (std::string shrt_flag, std::string lng_flag, std::string destination, std::string dfault="");
		void parse_args (int argc, char **argv);
		void error (std::string msg);
		void help ();

		map<std::string, std::string> options;
		vector<std::string> arguments;

	private:
		std::string use_msg;
		vector<Option> opts;

		void find_opt (char **argv, int index);
};

/*** The End ***/

