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
// gameManager.h

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