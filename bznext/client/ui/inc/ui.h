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
// ui.h


#ifndef _UI_H_
#define _UI_H_

#include "gameloop.h"

class CUserInterface
{
public:
		CUserInterface();
		CUserInterface ( CBaseGameLoop * pGameLoop );
		~CUserInterface();

		void Set ( CBaseGameLoop * pGameLoop );

		void Init ( void );
		void Attach ( void );
		void Release ( void );

		bool Think ( void );
protected:
	CBaseGameLoop * gameLoop;
};

#endif //_UI_H_