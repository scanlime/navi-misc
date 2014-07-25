/* Firestarter
* commandargs.h :
*
* Copyright (C) 2004 Jeffrey Myers
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* email: jeffm2501@sbcglobal.net
*/

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
#include "Singleton.h"

class CCommandLineArgs : public Singleton<CCommandLineArgs>
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