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
#include "gameManager.h"

CGameManger::CGameManger()
{
	gameLoop = NULL;
}

CGameManger::CGameManger ( CBaseGameLoop * pGameLoop )
{
	Set(pGameLoop);
}

CGameManger::~CGameManger()
{

}

void CGameManger::Set ( CBaseGameLoop * pGameLoop )
{
	gameLoop = pGameLoop;
}

void CGameManger::Init ( void )
{

}
void CGameManger::Attach ( void )
{

}

void CGameManger::Release ( void )
{

}

bool CGameManger::Think ( void )
{
		return false;
}
