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
// world.h

#ifndef _TEST_WORLD_H_
#define _TEST_WORLD_H_

#include "gameloop.h"
#include "baseObject.h"

class CTestWorld : public CBaseObject
{
	public:
		CTestWorld();
		CTestWorld ( CBaseGameLoop * pGameLoop );
		~CTestWorld();
		
		void Set ( CBaseGameLoop * pGameLoop );

		void Load ( bool draw );		
		bool Think ( void );

		// functions for the world drawable to call
		const char* GetValueS ( const char *item );

		// other stuff here for all the other world things that we need to do.

	protected:
		CBaseGameLoop * gameLoop;
		int							worldDrawable;
};

#endif //_TEST_WORLD_H_