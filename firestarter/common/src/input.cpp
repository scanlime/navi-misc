/* Firestarter 
* Input.cpp : Defines the entry point for the DLL application.
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

#include "input.h"

// action stuff

CAction::CAction()
{
}

CAction::~CAction()
{
}

float CAction::getRelative(void)
{
	return 0;
}

float CAction::getAbs(void)
{
	return 0;
}

float CAction::getRaw(void)
{
	return 0;
}

float CAction::getMinRange ( void )
{
	return 0;
}

float CAction::getMaxRange ( void )
{
	return 0;
}

float CAction::getMaxRaw ( void )
{
	return 0;
}

float CAction::getMinRaw ( void )
{
	return 0;
}

// listener functions
void CAction::addListener ( CActionListener* listener )
{
	if (listener)
		listeners.push_back(listener);
}

void CAction::removeListener ( CActionListener *listener )
{
	if (!listener)
		return;

	tvActionListenerList::iterator itr = listeners.begin();

	while (itr != listeners.end())
	{
		if (listener == *itr)
			itr = listeners.erase(itr);
		else
			itr++;
	}
}

// base input manager

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

bool CInputManager::KeyDown ( InputKeyCode key )
{
	return mInputDevice->isKeyDown((Ogre::KeyCode)key);
}
