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
#include "testGame.h"
#include "input.h"

// the drawables for this game
#include "worldDrawables.h"


void CTestGame::registerFactory (const char* name, CBaseDrawableFactory* factory)
{
	CDrawManager::instance().Register(factory,name);
	factoryList.push_back(factory);
}

void CTestGame::Init ( void )
{
	// do one time init stuff here
	registerFactory("sky",new CSkyboxObjectFactory);
	registerFactory("ground",new CGroundObjectFactory);
}

void CTestGame::Kill ( void )
{
	std::vector<CBaseDrawableFactory*>::iterator	itr = factoryList.begin();
	while (itr != factoryList.end())
		delete(*(itr++));
}

void CTestGame::Attach ( void )
{
	trWorldInfo	info;

	info.ambientColor[0] = info.ambientColor[1] = info.ambientColor[2] = 0.5f;
	info.sunPos[0] = info.sunPos[1] = info.sunPos[2] = 100;
	info.sunColor[0] = info.sunColor[1] = info.sunColor[2] = 0.75f;
	info.groundSize = 800;
	info.groundTextureRepeat = 60;

	info.groundTexture = "ground_mat";
	info.skybox = "malrav1sky";

	world.Load(info,true);
}

void CTestGame::Release ( void )
{
}

bool CTestGame::Think ( void )
{
	if (CInputManager::instance().KeyDown(KEY_ESCAPE))
		return true;
	// do some game like things
	return false;
}
