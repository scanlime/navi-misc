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
	}
}

void CTestWorld::AddWorldObject ( trObjectInfo &info )
{

}

const char* CTestWorld::GetValueS ( const char *item )
{
	std::string label = item;

	if (label == "groundTexture")
		return worldInfo.groundTexture.c_str();
	if (label == "skybox")
		return worldInfo.skybox.c_str();

	return NULL;
}

float CTestWorld::GetValueF ( const char *item )
{
	std::string label = item;

	if (label == "groundSize")
		return worldInfo.groundSize;
	if (label == "groundTextureRepeat")
		return worldInfo.groundTextureRepeat;

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
	return 0;
}

bool CTestWorld::Think ( void )
{
	return false;
}
