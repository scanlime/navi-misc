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
#include "ui.h"

CUserInterface::CUserInterface()
{
	gameLoop = NULL;
}

CUserInterface::CUserInterface ( CBaseGameLoop * pGameLoop )
{
	Set(pGameLoop);
}

CUserInterface::~CUserInterface()
{

}

void CUserInterface::Set ( CBaseGameLoop * pGameLoop )
{
	gameLoop = pGameLoop;
}

void CUserInterface::Init ( void )
{

}
void CUserInterface::Attach ( void )
{

}

void CUserInterface::Release ( void )
{

}

bool CUserInterface::Think ( void )
{
	bool quit = gameLoop->GetInput()->KeyDown(KC_ESCAPE);

	if (!quit)
	{
		// do some stuff here
	}

	return quit;
}
