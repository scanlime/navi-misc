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

// base input manager

class CBaseDevice
{
public:
	virtual ~CBaseDevice() = 0;

	virtual void init ( RenderWindow *theWindow ) = 0;
	virtual void kill ( void ) = 0;

	virtual void process ( void ) = 0;
};

class COgreKeyboard : public CBaseDevice , public KeyListener
{
public:
	COgreKeyboard();
	virtual ~COgreKeyboard();

	virtual void init ( RenderWindow *theWindow );
	virtual void kill ( void );

	virtual void process ( void );

protected:
	EventProcessor*		eventProcessor;
};


// ogre keyboard
COgreKeyboard::COgreKeyboard()
{
	eventProcessor = NULL;
}

COgreKeyboard::~COgreKeyboard()
{
	if (eventProcessor)
		kill();
}

void COgreKeyboard::init ( RenderWindow *theWindow )
{
	if (eventProcessor)
		kill();

	eventProcessor = new EventProcessor();
	eventProcessor->initialise(theWindow);
	eventProcessor->addKeyListener(this);

	eventProcessor->startProcessingEvents()
}

void COgreKeyboard::kill ( void )
{
}

void COgreKeyboard::process ( void )
{

}


struct CInputManager::trInfo
{
	//some devices
};


CInputManager::CInputManager()
{
	mInputDevice = NULL;
}

CInputManager::~CInputManager()
{
	if (mInputDevice)
		return;
	PlatformManager::getSingleton().destroyInputReader(mInputDevice);
	mInputDevice = NULL;
}

void CInputManager::Init ( RenderWindow *theWindow )
{
}

void CInputManager::Process ( void )
{
	mInputDevice->capture();
}

void CInputManager::init ( RenderWindow *theWindow )
{
	eventProcessor = new EventProcessor;

	eventProcessor->initialise(theWindow);

}
void CInputManager::update ( void )
{
}


// new API


