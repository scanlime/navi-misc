/* 3dScreamers
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
// world.cpp

#include "world.h"


CTestWorld::CTestWorld()
{
		gameLoop = NULL;
}

CTestWorld::CTestWorld ( CBaseGameLoop * pGameLoop )
{
	Set(pGameLoop);
}

CTestWorld::~CTestGame()
{

}

void CTestWorld::Set ( CBaseGameLoop * pGameLoop )
{
	gameLoop = pGameLoop;
}

void CTestWorld::Load ( bool draw )
{

}

bool CTestWorld::Think ( void )
{
	return false;
}
