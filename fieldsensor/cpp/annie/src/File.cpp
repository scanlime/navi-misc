#include "../include/File.h"
#include "../include/Exception.h"
#include "../include/defines.h"
#include <cstdlib>

using namespace std;
namespace annie
{
File::File()
{	_isOpen=false;	}

File::File(string filename)
{	_isOpen=false;	open(filename);	}

void
File::open(string filename)
{
	if (_isOpen)
		throw Exception("File::open() - Another file is already open");
	this->_filename=filename;
	_file.open(filename.c_str(),ios::in);
	if (!_file)
		throw Exception("File::open() - Couldn't open the file for reading");
	if (readWord().compare("ANNIE_FILE"))
		throw Exception("File::open() - The file doesn't appear to be an annie file");
	if (readWord().compare(ANNIE_VERSION))
		throw Exception("File::open() - The file is annie's file, but not the right version");
}

void
File::_next()
{
	char temp;
	ws(_file);
	while (_file.peek()=='#')
	{
		do	
		{
			_file.get(temp);
		} 
		while (temp!='\n' && !_file.eof());
		ws(_file);
	}
}

char
File::readChar()
{
	char c='\0';
	_next();
	if (!_file.eof())
		_file>>c;
	return c;
}

int
File::readInt()
{
	int i=0;
	_next();
	if (!_file.eof())
		_file>>i;
	ws(_file);
	return i;
}

real
File::readDouble()
{
	real d=0;
	_next();
	if (!_file.eof())
		_file>>d;
	ws(_file);
	return d;
}

string
File::readWord()
{	
	string s("");
	_next();
	if (!_file.eof())
		_file>>s;
	ws(_file);
	return s;
}

string
File::readLine()
{
	string s;
	_next();
	getline(_file,s,'\n');
	int commentPos=s.find_first_of('#',0);
	return s.substr(0,commentPos);
}

void
File::close()
{
	if (_isOpen)
	{
		_file.close();
		_isOpen=false;
	}
}

bool
File::eof()
{	return _file.eof() || (_file.peek()==-1);}

}; //namespace annie

