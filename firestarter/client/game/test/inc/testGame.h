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

#ifndef _TEST_GAME_H_
#define _TEST_GAME_H_

#include "gameManager.h"
#include "world.h"

class CTestGame : public CBaseGame
{
	public:
		CTestGame(){gameLoop = NULL;}
		CTestGame ( CBaseGameLoop * pGameLoop ){Set(pGameLoop);}
		virtual	~CTestGame(){return;}

		virtual void Init ( void );
		virtual void Attach ( void );
		virtual void Release ( void );

		virtual bool Think ( void );

	protected:
	CTestWorld			world;
};

#endif //_TEST_GAME_H_