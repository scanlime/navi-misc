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

#include "drawManager.h"

CDrawManager::CDrawManager()
{
	gameLoop = NULL;
	lastID = -1;
}

CDrawManager::~CDrawManager()
{
	//drawables

	drawableMap::iterator itr = drawables.begin();

	while (itr != drawables.end())
	{
		factories[itr->second->GetName()]->Delete(itr->second);
		itr++;
	}
	drawables.clear();
	factories.clear();
}

void CDrawManager::Init ( CBaseGameLoop * pGameLoop )
{
	gameLoop = pGameLoop;
}

void CDrawManager::Register ( CBaseDrawableFactory* factory, const char* name )
{
	std::string className = name;
	if (factory)
		return;

	factories[name] = factory;
}

int CDrawManager::New ( const char* name, CBaseObject* parent )
{
	std::string className = name;
	factoryMap::iterator itr = factories.find(className);

	if (itr == factories.end())
		return -1;
	
	CBaseDrawable *object = itr->second->New(parent);
	if (!object)
		return -1;

	lastID++;

	object->SetName(name);
	object->Set(gameLoop);
	drawables[lastID] = object;

	return lastID;
}

bool CDrawManager::Delete ( int item )
{
	drawableMap::iterator itr = drawables.find(item);

	if (itr == drawables.end())
		return false;

	factories[itr->second->GetName()]->Delete(itr->second);
	drawables.erase(itr);

	return true;
}
