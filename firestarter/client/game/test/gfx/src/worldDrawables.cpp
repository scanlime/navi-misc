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
// worldDrawables.cpp

#include "worldDrawables.h"

// skybox factory
CBaseDrawable* CSkyboxObjectFactory::New ( CBaseObject* parent )
{
	CBaseDrawable* obj = new CSkyObject;
	obj->Set(parent);
	return obj;
}

void CSkyboxObjectFactory::Delete ( CBaseDrawable* object )
{
	delete(object);
}

// skybox object
CSkyObject::CSkyObject()
{

}

CSkyObject::~CSkyObject()
{
}

void CSkyObject::Init ( void )
{
}

void CSkyObject::Think ( void )
{

}

