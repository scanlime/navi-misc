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
	vis = false;
	node = NULL;
}

CPlayerDrawObject::~CPlayerDrawObject()
{
	if (!node)
		return;

	if (vis)
		CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->removeAndDestroyChild(node->getName());
	else
		CFirestarterLoop::instance().GetSceneManager()->destroySceneNode(node->getName());
	node = NULL;
}

void CPlayerDrawObject::Init ( void )
{
	char	temp[512];

	Entity* mShip1 = CFirestarterLoop::instance().GetSceneManager()->createEntity(parent->GetValueS("name"), parent->GetValueS("mesh"));
	parent->GetMaterial(NULL,temp);
	mShip1->getMesh()->getSubMeshIterator().getNext()->setMaterialName(temp);

	vis = false;
	if (node)
	{
		if (vis)
			CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->removeAndDestroyChild(node->getName());
		else
			CFirestarterLoop::instance().GetSceneManager()->destroySceneNode(node->getName());
		node = NULL;
	}

	node = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	if (!node)
		return;

	node->attachObject(mShip1);

	node->rotate(Vector3(0,0,1),parent->GetValueF("rotz"));
	node->translate(parent->GetValueF("posx"),parent->GetValueF("posy"),parent->GetValueF("posz")); 

	vis = true;
}

void CPlayerDrawObject::Think ( void )
{
	// see if it's suposed to be visible and hide it if it's not, or show it if it is
	if (parent->Visible())
	{
		if (!vis)
		{
			CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->addChild(node);
			vis = true;
		}
	}
	else
	{
		if (vis)
		{
			CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->removeChild(node->getName());
			vis = false;
		}
	}

	if (vis)
	{
		node->rotate(Vector3(0,0,1),parent->GetValueF("rotz"));
		node->translate(parent->GetValueF("posx"),parent->GetValueF("posy"),parent->GetValueF("posz")); 
	}
}

// camera drawables
CBaseDrawable* CCameraObjectFactory::New ( CBaseObject* parent )
{
	CBaseDrawable* obj = new CCameraDrawObject;
	obj->Set(parent);
	return obj;

}

void CCameraObjectFactory::Delete ( CBaseDrawable* object ) 
{
	delete(object);
}

CCameraDrawObject::CCameraDrawObject()
{
}

CCameraDrawObject::~CCameraDrawObject()
{
}
void CCameraDrawObject::Init ( void )
{
}

void CCameraDrawObject::Think ( void )
{
	float pos[3];
	float rot[3];

	parent->GetPos(pos);
	parent->GetRot(rot);

	Camera *cam = CFirestarterLoop::instance().GetCamera();

	cam->setFixedYawAxis(true);

	cam->setPosition(pos[0],pos[1],pos[2]);
	cam->setDirection(rot[0],rot[1],rot[2]);
}

