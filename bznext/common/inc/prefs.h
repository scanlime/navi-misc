/* Project24
 * Copyright (c) 2002 - 2003 Jeffrey Myers
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named license.txt that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
// prefs.h

#ifndef _PREFS_H_
#define _PREFS_H_

#ifdef _WIN32
#pragma warning( disable : 4786 )  // Disable warning message
#endif

#include <string>
#include <map>
#include "osfile.h"

typedef std::map<std::string,std::string>	tmPrefsMap;

class CPrefsManager
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

void SetPrefs ( CPrefsManager *prefs );
CPrefsManager* GetPrefs ( void );
#endif//_PREFS_H_



