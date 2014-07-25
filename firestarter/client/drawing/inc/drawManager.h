/* Firestarter
* drawManager.h :
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

#ifndef _DRAW_MANAGER_H_
#define _DRAW_MANAGER_H_

#include "gameloop.h"
#include "baseObject.h"
#include "Singleton.h"
#include <string>
#include <map>
#include <vector>

class CBaseDrawable
{
public:
	CBaseDrawable(){return;}
	CBaseDrawable( CBaseObject* pParent ){Set(pParent);}
	virtual ~CBaseDrawable(){return;}

	void Set ( CBaseObject* pParent ){parent = pParent;}

	virtual void Init ( void ) = 0;
	virtual void Think ( void ) = 0;

	// name stuff
	void SetName ( const char* name ){className = name;}
	const char* GetName ( void ){return className.c_str();}
protected:
	CBaseObject			*parent;
	std::string			className;
};

class CBaseDrawableFactory
{
public:
	CBaseDrawableFactory(){return;}
	virtual ~CBaseDrawableFactory(){return;}

	virtual CBaseDrawable* New ( CBaseObject* parent ) = 0;
	virtual void Delete ( CBaseDrawable* object ) = 0;
};

class CDrawManager : public Singleton<CDrawManager>
{
public:
  CDrawManager();
  ~CDrawManager();

	void Init ( void );
	void Register ( CBaseDrawableFactory* factory, const char* name );
	
	int New ( const char* name, CBaseObject* parent );
	bool Delete ( int item );
	void ClearAll ( void );

	void ThinkAll ( void );

protected:
	typedef std::map<std::string,CBaseDrawableFactory*> factoryMap;
	factoryMap	factories;
	typedef std::map<int,CBaseDrawable*> drawableMap;
	drawableMap	drawables;
	int							lastID;
};

#endif //_DRAW_MANAGER_H_

