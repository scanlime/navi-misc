// Input.cpp : Defines the entry point for the DLL application.
//

#include "input.h"

CInputManager::CInputManager()
{
	mInputDevice = NULL;
}

CInputManager::~CInputManager()
{
	PlatformManager::getSingleton().destroyInputReader(mInputDevice);
	mInputDevice = NULL;
}

void CInputManager::Init ( RenderWindow *theWindow )
{
	mInputDevice = PlatformManager::getSingleton().createInputReader();
	mInputDevice->initialise(theWindow,true, false);
}

void CInputManager::Process ( void )
{
	mInputDevice->capture();
}

bool CInputManager::KeyDown ( int key )
{
	return mInputDevice->isKeyDown((Ogre::KeyCode)key);
}
