/* XXXXX
* gameManager.cpp :
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

