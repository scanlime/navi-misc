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
// world.cpp

#include "world.h"
#include "drawManager.h"

CTestWorld::CTestWorld()
{
}

CTestWorld::~CTestWorld()
{

}

void CTestWorld::Load ( trWorldInfo &info, bool draw )
{
	worldInfo = info;
	if (draw)
	{
		worldDrawables[0] = CDrawManager::instance().New("sky",this);
		worldDrawables[1] = CDrawManager::instance().New("ground",this);
		worldDrawables[2] = CDrawManager::instance().New("walls",this);
	}
}

void CTestWorld::Load ( CNetworkMessage &message, bool draw )
{

	worldInfo.name = message.ReadStr(); 
	worldInfo.skybox = message.ReadStr(); 
	worldInfo.groundTexture = message.ReadStr(); 
	worldInfo.wallTexture = message.ReadStr(); 
	worldInfo.wallSize = message.ReadF();
	worldInfo.groundTextureRepeat = message.ReadF();
	worldInfo.groundSize[0] = message.ReadF();
	worldInfo.groundSize[1] = message.ReadF();
	message.ReadV(worldInfo.sunPos);
	message.ReadV(worldInfo.sunColor);
	message.ReadV(worldInfo.ambientColor);

	int flairCount = message.ReadI();
	trFlairDef	flair;

	flairGroups.clear();

	for (int i = 0; i < flairCount; i++)
	{
		flair.mesh = message.ReadStr();
		flair.count = message.ReadI();
		flair.center[0] = message.ReadF();
		flair.center[1] = message.ReadF();
		flair.range = message.ReadF();
		flair.scale[0] = message.ReadF();
		flair.scale[1] = message.ReadF();
		flairGroups.push_back(flair);
	}

	if (draw)
	{
		worldDrawables[0] = CDrawManager::instance().New("sky",this);
		worldDrawables[1] = CDrawManager::instance().New("ground",this);
		worldDrawables[2] = CDrawManager::instance().New("walls",this);
	}
}

void CTestWorld::AddWorldObject ( trObjectInfo &info )
{

}

void CTestWorld::AddTuft ( trFlairDef &tuft )
{	
	flairGroups.push_back(tuft);
}

const char* CTestWorld::GetValueS ( const char *item )
{
	std::string label = item;

	if (label == "groundTexture")
		return worldInfo.groundTexture.c_str();
	if (label == "wallTexture")
		return worldInfo.wallTexture.c_str();
	if (label == "skybox")
		return worldInfo.skybox.c_str();

	return NULL;
}

float CTestWorld::GetValueF ( const char *item )
{
	std::string label = item;

	if (label == "groundSizeX")
		return worldInfo.groundSize[0];
	if (label == "groundSizeY")
		return worldInfo.groundSize[1];
	if (label == "groundTextureRepeat")
		return worldInfo.groundTextureRepeat;	
	if (label == "wallHeight")
		return worldInfo.wallSize;


	if (label == "sunPosX")
		return worldInfo.sunPos[0];
	if (label == "sunPosY")
		return worldInfo.sunPos[1];
	if (label == "sunPosZ")
		return worldInfo.sunPos[2];

	if (label == "sunColorR")
		return worldInfo.sunColor[0];
	if (label == "sunColorG")
		return worldInfo.sunColor[1];
	if (label == "sunColorB")
		return worldInfo.sunColor[2];

	if (label == "ambientColorR")
		return worldInfo.ambientColor[0];
	if (label == "ambientColorG")
		return worldInfo.ambientColor[1];
	if (label == "ambientColorB")
		return worldInfo.ambientColor[2];

	return 0;
}

int CTestWorld::GetValueI ( const char *item )
{
	std::string label = item;

	if (label == "flairGroups")
		return (int)(&flairGroups);

	return 0;
}

bool CTestWorld::Think ( void )
{
	return false;
}
