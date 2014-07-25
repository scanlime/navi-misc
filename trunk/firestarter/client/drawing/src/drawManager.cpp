/* Firestarter
* drawManager.cpp :
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

#include "drawManager.h"

template <>
CDrawManager* Singleton<CDrawManager>::_instance = (CDrawManager*)0;

CDrawManager::CDrawManager()
{
	lastID = -1;
}

CDrawManager::~CDrawManager()
{
	ClearAll();
}

void CDrawManager::Init ( void )
{

}

void CDrawManager::Register ( CBaseDrawableFactory* factory, const char* name )
{
	std::string className = name;
	if (!factory)
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
	object->Init();
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

void CDrawManager::ClearAll ( void )
{
	drawableMap::iterator itr = drawables.begin();

	while (itr != drawables.end())
	{
		factories[itr->second->GetName()]->Delete(itr->second);
		itr++;
	}
	drawables.clear();
	factories.clear();
}

void CDrawManager::ThinkAll ( void )
{
	drawableMap::iterator itr = drawables.begin();

	while (itr != drawables.end())
	{
		itr->second->Think();
		itr++;
	}
}

