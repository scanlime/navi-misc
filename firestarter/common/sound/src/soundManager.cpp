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

#include "soundManager.h"
#include "fmod.h"

template <>
CSoundManager* Singleton<CSoundManager>::_instance = (CSoundManager*)0;

CSoundManager::CSoundManager()
{
	//std::vector<COSFile> playlist;
}

CSoundManager::~CSoundManager()
{
}

void CSoundManager::Activate ( void )
{
}

void CSoundManager::Deactivate ( void )
{
}


void CSoundManager::Update ( void )
{
}


void CSoundManager::Clear ( void )
{
}


bool CSoundManager::AddBGMFolder ( const char* dir )
{
	return false;
}

void CSoundManager::StartBGMPlaylist ( void 
{
}

void CSoundManager::StopBGMPlaylist ( void 
{
}

