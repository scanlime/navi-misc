/* Firestarter
* prefs.h : 
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

#ifndef _PREFS_H_
#define _PREFS_H_

#ifdef _WIN32
#pragma warning( disable : 4786 )  // Disable warning message
#endif

#include <string>
#include <map>
#include "osfile.h"
#include "Singleton.h"

typedef std::map<std::string,std::string>	tmPrefsMap;

class CPrefsManager  : public Singleton<CPrefsManager>
{
public:
	CPrefsManager();
	~CPrefsManager();

	void Init ( char *szFileName );
	void Update ( void );

	int GetNumItems ( void );
	bool ItemExists ( const char* szName );
	void SetItem ( const char* szName, int iData );
	void SetItem ( const char* szName, float fData );
	void SetItem ( const char* szName, char* pData );

	int GetItemI ( const char* szName );
	float GetItemF ( const char* szName );
	const char* GetItemS ( const char* szName );

	const char* GetPrefsPath ( void );
	const char* GetPrefsFileName ( char *pathName  );
protected:
	tmPrefsMap		itemMap;
	COSFile			file;
};
#endif//_PREFS_H_



