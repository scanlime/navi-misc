/* Firestarter
* serverGameManager.cpp :
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

#include "serverGameManager.h"
#include "testGameServer.h"

template <>
CServerGameManger* Singleton<CServerGameManger>::_instance = (CServerGameManger*)0;


CServerGameManger::CServerGameManger()
{
	//std::map<std::string,CBaseServerGame*>	gameModules;
	gameModules.clear();
}

CServerGameManger::~CServerGameManger()
{
	kill();
}

void CServerGameManger::init ( void )
{
	if (gameModules.size() == 0)
		RegisterGameModules();
}

void CServerGameManger::kill ( void )
{	
	std::map<std::string,CBaseServerGame*>::iterator	itr = gameModules.begin();

	while (itr != gameModules.end())
	{
		itr->second->kill();
		delete(itr->second);
		itr++;
	}
	gameModules.clear();
}

CBaseServerGame* CServerGameManger::get ( const char* name )
{
	std::map<std::string,CBaseServerGame*>::iterator	itr = gameModules.find(std::string(name));
	if (itr == gameModules.end())
		return NULL;

	return itr->second;
}

void CServerGameManger::RegisterGameModules ( void )
{
	gameModules[std::string("susan")] = new CTestGameServer;
}
