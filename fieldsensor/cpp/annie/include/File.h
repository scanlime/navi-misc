#ifndef _FILE_H
#define _FILE_H
#include <fstream>
#include <string>
#include "defines.h"

namespace annie
{
/** The idea is that this class will be used to read in information from text files
  * created by the "save" function in various annie classes.
  * When a file is opened, we check that the first non-commented line contains
  * "ANNIE_FILE <ver>" where <ver> is the version number. This has been done
  * to allow for future changes to file formats used by ANNIE.
  * All save functions should save in the same format. Current version is 1.0.
  *
  * Comments in an ANNIE file are given by a '#'. The rest of the line following '#'
  * is ignored. The member functions of this class return values ignoring any
  * and all comments that may have appeared in between.
  */
class File
{
private:
	std::string _filename;
	std::ifstream _file;
	bool _isOpen;
	void _next();
public:
	///Creates an empty File object
	File();

	///Opens a given filename in the File object
	/** @param filename The name of the file to be opened.
	  * \throws Throws an Exception if the first line of the file is not ANNIE_FILE
	  *			or the version of the ANNIE file is an incorrect one (not supported by
	  *			this compilation of code)
	  */
	File(std::string filename);

	///Explicitly opens a given filename in the File object
	/** @param filename The name of the file to be opened.
	  * \throws Exception if the first line of the file is not ANNIE_FILE
	  *			or the version of the ANNIE file is an incorrect one (not supported by
	  *			this compilation of code)
	  * \throws Exception if another file is already opened and hasn't been closed.
	  */
	void open(std::string filename);
	
	///Reads one character from the file
	char readChar();

	///Returns an integer read from the file
	int readInt();

	///Returns a real read from the file
	real readDouble();

	///Returns a "word" (a string with no word separators/delimiters) read from the file
	std::string readWord();

	///Closes the file
	void close();

	///Returns a complete line
	std::string readLine();

	///checks if the file has reached the end
	/** @return true if the file has reached the end, false otherwise*/
	bool eof();
};

}; //namespace annie
#endif // define _FILE_H

