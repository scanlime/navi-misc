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
// worldDrawables.h

#ifndef _WORLD_DRAWABLES_H_
#define _WORLD_DRAWABLES_H_

#include "baseObject.h"
#include "drawManager.h"

class CSkyboxObjectFactory : public CBaseDrawableFactory
{
public:
	virtual CBaseDrawable* New ( CBaseObject* parent );
	virtual void Delete ( CBaseDrawable* object );
};

class CSkyObject : public CBaseDrawable
{
public:
	CSkyObject();
	virtual ~CSkyObject();
	virtual void Init ( void );
	virtual void Think ( void );

protected:
};

class CGroundObjectFactory : public CBaseDrawableFactory
{
public:
	virtual CBaseDrawable* New ( CBaseObject* parent );
	virtual void Delete ( CBaseDrawable* object );
};

class CGroundObject : public CBaseDrawable
{
public:
	CGroundObject();
	virtual ~CGroundObject();
	virtual void Init ( void );
	virtual void Think ( void );

protected:
	SceneNode* mGroundNode;
};

class CWallObjectFactory : public CBaseDrawableFactory
{
public:
	virtual CBaseDrawable* New ( CBaseObject* parent );
	virtual void Delete ( CBaseDrawable* object );
};

class CWallObject : public CBaseDrawable
{
public:
	CWallObject();
	virtual ~CWallObject();
	virtual void Init ( void );
	virtual void Think ( void );

protected:
	SceneNode* mWallNode[4];
};

#endif //_WORLD_DRAWABLES_H_
