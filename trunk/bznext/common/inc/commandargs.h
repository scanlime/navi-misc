// comandargs.h

#ifndef _COMMAND_ARGS_H_
#define _COMMAND_ARGS_H_

#ifdef _WIN32
#pragma warning( disable : 4181 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif //_WIN32

#include <map>
#include <string>
#include "osfile.h"

class CCommandLineArgs
{
public:
	CCommandLineArgs();
	~CCommandLineArgs();
	CCommandLineArgs( int argc, char *argv[] );
	CCommandLineArgs( const char* szCommandLine );
	CCommandLineArgs( COSFile &file );

	void Set ( int argc, char *argv[] );
	void Set ( const char* szCommandLine );
	void Set ( COSFile &file );

	void Clear ( void );

	bool Exists ( const char* szKey );
	
	const char* GetDataS ( const char* szKey );
	int GetDataI ( const char* szKey );
	bool GetDataB ( const char* szKey );
	float GetDataF ( const char* szKey );

private:
	typedef std::map<std::string,std::string> tmCommandMap;
	tmCommandMap		commands;
};
#endif//_COMMAND_ARGS_H_