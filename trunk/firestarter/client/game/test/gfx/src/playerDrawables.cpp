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
// playerDrawables.cpp

#include "playerDrawables.h"
#include "firestarter.h"

// skybox factory
CBaseDrawable* CPlayerObjectFactory::New ( CBaseObject* parent )
{
	CBaseDrawable* obj = new CPlayerDrawObject;
	obj->Set(parent);
	return obj;
}

void CPlayerObjectFactory::Delete ( CBaseDrawable* object )
{
	delete(object);
}

// player tank object
CPlayerDrawObject::CPlayerDrawObject()
{
}

CPlayerDrawObject::~CPlayerDrawObject()
{
}

void CPlayerDrawObject::Init ( void )
{
	Entity* mShip1 = CFirestarterLoop::instance().GetSceneManager()->createEntity(parent->GetValueS("name"), parent->GetValueS("mesh"));
	mShip1->getMesh()->getSubMeshIterator().getNext()->setMaterialName(parent->GetValueS("meterial"));

	node = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	node->attachObject(mShip1);

	node->rotate(Vector3(0,0,1),parent->GetValueF("rotz"));
	node->translate(parent->GetValueF("posx"),parent->GetValueF("posy"),parent->GetValueF("posz")); 
}

void CPlayerDrawObject::Think ( void )
{
	node->rotate(Vector3(0,0,1),parent->GetValueF("rotz"));
	node->translate(parent->GetValueF("posx"),parent->GetValueF("posy"),parent->GetValueF("posz")); 
}

