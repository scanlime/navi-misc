/* 
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
#include "mainMenu.h"

CMainMenu::CMainMenu()
{
	CBaseUIPanel::CBaseUIPanel();
}

CMainMenu::~CMainMenu()
{

}

CMainMenu::Init ( CBaseGameLoop * pGameLoop )
{
	(CBaseUIPanel*)this->Init(pGameLoop);

	// init other stuff here
}

void CMainMenu::Attach ( void )
{
	//Overlay* o = (Overlay*)OverlayManager::getSingleton().getByName("Core/DebugOverlay");

}

void CMainMenu::Release ( void )
{
}

tePanelReturn CMainMenu::Process ( std::string &next )
{
	return ePanelContinue;
}