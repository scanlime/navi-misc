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
// playerDrawables.h

#ifndef _PLAYER_DRAWABLES_H_
#define _PLAYER_DRAWABLES_H_

#include "baseObject.h"
#include "drawManager.h"

// player drawables
class CPlayerObjectFactory : public CBaseDrawableFactory
{
public:
	virtual CBaseDrawable* New ( CBaseObject* parent );
	virtual void Delete ( CBaseDrawable* object );
};

class CPlayerDrawObject : public CBaseDrawable
{
public:
	CPlayerDrawObject();
	virtual ~CPlayerDrawObject();
	virtual void Init ( void );
	virtual void Think ( void );

protected:
	SceneNode*	node;
	bool				vis;
	float			lastRot;
};

// shot drawables
class CShotObjectFactory : public CBaseDrawableFactory
{
public:
	virtual CBaseDrawable* New ( CBaseObject* parent );
	virtual void Delete ( CBaseDrawable* object );
};

class CShotDrawObject : public CBaseDrawable
{
public:
	CShotDrawObject();
	virtual ~CShotDrawObject();
	virtual void Init ( void );
	virtual void Think ( void );

protected:
	SceneNode*	node;
	float				lastRot;
};

// camera
class CCameraObjectFactory : public CBaseDrawableFactory
{
public:
	virtual CBaseDrawable* New ( CBaseObject* parent );
	virtual void Delete ( CBaseDrawable* object );
};

class CCameraDrawObject : public CBaseDrawable
{
public:
	CCameraDrawObject();
	virtual ~CCameraDrawObject();
	virtual void Init ( void );
	virtual void Think ( void );
protected:
	float			lastRot;
};

#endif //_PLAYER_DRAWABLES_H_
