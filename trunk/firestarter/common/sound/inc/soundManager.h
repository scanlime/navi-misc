/* Firestarter
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

#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

#include "Singleton.h"
#include "osfile.h"

#include <vector>

class CSoundManager : public Singleton<CSoundManager>
{
public:
	CSoundManager();
	~CSoundManager();

	void Activate ( void );
	void Deactivate ( void );

	void Update ( void );

	void Clear ( void );

	bool AddBGMFolder ( const char* dir );
	void StartBGMPlaylist ( void );
	void StopBGMPlaylist ( void );

protected:
	std::vector<COSFile> playlist;
};

#endif //_SOUND_MANAGER_H_