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
// drawManager.h

#ifndef _DRAW_MANAGER_H_
#define _DRAW_MANAGER_H_

#include "gameloop.h"
#include "baseObject.h"
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
	void Set ( CBaseGameLoop *pGameLoop ){gameLoop = pGameLoop;}

	virtual void Init ( void ) = 0;
	virtual void Think ( void ) = 0;

	// name stuff
	void SetName ( const char* name ){className = name;}
	const char* GetName ( void ){return className.c_str();}
protected:
	CBaseGameLoop		*gameLoop;
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

class  CDrawManager
{
public:
  CDrawManager();
  ~CDrawManager();

	void Init ( CBaseGameLoop * pGameLoop );
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

	CBaseGameLoop		*gameLoop;
	int							lastID;
};

#endif //_DRAW_MANAGER_H_

