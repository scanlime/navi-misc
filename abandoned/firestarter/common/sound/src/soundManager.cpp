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
#include "prefs.h"

template <>
CSoundManager* Singleton<CSoundManager>::_instance = (CSoundManager*)0;

CSoundManager::CSoundManager()
{
	//std::vector<COSFile> playlist;
}

CSoundManager::~CSoundManager()
{
}

typedef enum
{
	eStandardSound = 0,
	eBasicSound,
	eOptionalSound,
	eNoSound
}teSoundPrefs;

typedef enum
{
	eWin32SoundSystem = 0,
	eLinuxSoundSystem,
	eMacSoundSystem,
	eUnknowSoundSystem
}teSoundOS;

void CSoundManager::Activate ( void )
{
	// pick the sound system for the OS
	// read from the prefs to see what we want
	teSoundPrefs preferdSound = (teSoundPrefs)(CPrefsManager::instance().GetItemI("soundAPI"));
		
	teSoundOS	soundOS = eUnknowSoundSystem;

#if defined(WIN32) || defined(_WIN64) || defined(__CYGWIN32__) || defined(__WATCOMC__)
	soundOS = eWin32SoundSystem;
#elif defined(__linux__)
	soundOS = eLinuxSoundSystem;
#elif defined(__APPLE__)
	soundOS = eMacSoundSystem;
#endif 

	if (soundOS == eUnknowSoundSystem || preferdSound == eNoSound)
		FSOUND_SetOutput(FSOUND_OUTPUT_NOSOUND);
	else
	{
		switch (soundOS)
		{
			case eMacSoundSystem:
				FSOUND_SetOutput(FSOUND_OUTPUT_MAC);
				break;

			case eWin32SoundSystem:
				switch (preferdSound)
				{
					case eStandardSound:
						FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
						break;

					case eBasicSound:
						FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);
						break;

					case eOptionalSound:
						FSOUND_SetOutput(FSOUND_OUTPUT_ASIO);
						break;
				}
				break;

			case eLinuxSoundSystem:
				switch (preferdSound)
				{
					case eStandardSound:
						FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
						break;

					case eBasicSound:
						FSOUND_SetOutput(FSOUND_OUTPUT_OSS);
						break;

					case eOptionalSound:
						FSOUND_SetOutput(FSOUND_OUTPUT_ALSA);
						break;
				}
				break;
		}
	}
	
	// allocate some ram for the bloddy thing
	FSOUND_SetMemorySystem(malloc(4*1024*1024), 4*1024*1024, NULL, NULL, NULL); 

	//Select sound card (0 = default)
	int soundDriver = CPrefsManager::instance().GetItemI("soundDriver");
	FSOUND_SetDriver(soundDriver);

	FSOUND_Init(44100, 32, FSOUND_INIT_USEDEFAULTMIDISYNTH);
	{
		printf("Error!\n");
		printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));
		return 1;
	}
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

