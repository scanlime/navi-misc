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
#include "testGame.h"

CGameManger::CGameManger()
{
	gameLoop = NULL;
	theGame = NULL;
}

CGameManger::CGameManger ( CBaseGameLoop * pGameLoop )
{
	Set(pGameLoop);
}

CGameManger::~CGameManger()
{
	if (theGame)
		theGame->Release();

	gameModuleMap::iterator itr =	gameModules.begin();
	while (itr != gameModules.end())
	{
		delete(itr->second);
		itr++;
	}
	gameModules.clear();
	theGame = NULL;
}

void CGameManger::Set ( CBaseGameLoop * pGameLoop )
{
	gameLoop = pGameLoop;
}

void CGameManger::Init ( void )
{
	RegisterGameModules();
	activeGame = true;
}

void CGameManger::RegisterGameModules ( void )
{
	gameModules["susan"] = new CTestGame;

	gameModuleMap::iterator itr =	gameModules.begin();
	while (itr != gameModules.end())
	{
		itr->second->Set(gameLoop);
		itr++;
	}
}

void CGameManger::Attach ( void )
{
	if (theGame)
		theGame->Release();
	theGame = NULL;
//	gameLoop->ClearScene();

	std::string name = gameLoop->GetGameName();
	gameModuleMap::iterator itr =	gameModules.find(name);
	if (itr != gameModules.end())
	{
		theGame = itr->second;
		theGame->Attach();
	}
}

void CGameManger::Release ( void )
{
	if (theGame)
		theGame->Release();
	gameLoop->ClearScene();
	theGame = NULL;
}

bool CGameManger::Think ( void )
{
	if (!theGame)
		return true;
	return theGame->Think();
}

bool CGameManger::GameActive ( void )
{	
	return theGame!= NULL;
}

