/* XXXXX
* gameManager.h :
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

#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include "gameloop.h"
#include <vector>
#include <map>
#include <string>

class CBaseGame
{
public:
	CBaseGame(){gameLoop = NULL;}
	CBaseGame ( CBaseGameLoop * pGameLoop ){Set(pGameLoop);}
	virtual	~CBaseGame(){return;}
	virtual void Set ( CBaseGameLoop * pGameLoop ){gameLoop = pGameLoop;}

	virtual void Init ( void ) = 0;
	virtual void Attach ( void ) = 0;
	virtual void Release ( void ) = 0;

	virtual bool Think ( void ) = 0;

protected:
	CBaseGameLoop	*gameLoop;
};

class CGameManger
{
public:
		CGameManger();
		CGameManger ( CBaseGameLoop * pGameLoop );
		~CGameManger();

		void Set ( CBaseGameLoop * pGameLoop );

		void Init ( void );
		void Attach ( void );
		void Release ( void );

		bool Think ( void );
	
		bool GameActive ( void );

protected:
	CBaseGameLoop * gameLoop;
	bool						activeGame;

	void RegisterGameModules ( void );

	typedef std::map<std::string,CBaseGame*>	gameModuleMap;
	gameModuleMap	gameModules;
	CBaseGame*	theGame;
};

#endif //_GAME_MANAGER_H_