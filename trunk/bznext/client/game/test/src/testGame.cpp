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
#include "testGame.h"


void CTestGame::Init ( void )
{
	// do one time init stuff here
	world.Set(gameLoop);
}

void CTestGame::Attach ( void )
{
	world.Load(true);
}

void CTestGame::Release ( void )
{
}

bool CTestGame::Think ( void )
{
	if (gameLoop->GetInput().KeyDown(KEY_ESCAPE))
		return true;
	// do some game like things
	return false;
}
