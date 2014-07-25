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

// player factory
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
	lastRot = 0;
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

	std::string	mesh = parent->GetValueS("mesh");
	if (!mesh.size())
		mesh = "MK3.mesh";

	std::string name = parent->GetValueS("name");

	Entity* mShip1 = CFirestarterLoop::instance().GetSceneManager()->createEntity(name.c_str(), mesh.c_str());
	parent->GetMaterial(NULL,temp);
	Mesh::SubMeshIterator	&itr = mShip1->getMesh()->getSubMeshIterator();
	while (itr.hasMoreElements())
	{
		SubMesh *sub = itr.getNext();
		sub->setMaterialName(temp);
	}

	vis = parent->Visible();
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

	float rot[3];
	parent->GetRot(rot);
	rot[2] += 90.0f;

	float pos[3];
	parent->GetPos(pos);
	pos[2] += 1.5f;

	node->rotate(Vector3(0,0,1),rot[2]-lastRot);
	node->translate(pos[0],pos[1],pos[2]-100000); 
	lastRot = rot[2];

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
		float rot[3];
		parent->GetRot(rot);
		rot[2] += 90.0f;

		float pos[3];
		parent->GetPos(pos);
		pos[2] += 1.5f;

		node->rotate(Vector3(0,0,1),rot[2]-lastRot);
		node->setPosition(pos[0],pos[1],pos[2]); 

	//	node->translate(0,0,1.5f); 

		lastRot = rot[2];
	}
}

// shot drawables
CBaseDrawable* CShotObjectFactory::New ( CBaseObject* parent )
{
	CBaseDrawable* obj = new CShotDrawObject;
	obj->Set(parent);
	return obj;
}

void CShotObjectFactory::Delete ( CBaseDrawable* object )
{
	delete(object);
}

CShotDrawObject::CShotDrawObject()
{
	node = NULL;
}

CShotDrawObject::~CShotDrawObject()
{
	if (node)
		CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->removeAndDestroyChild(node->getName());
}

void CShotDrawObject::Init ( void )
{
	std::string	mesh = parent->GetValueS("mesh");
	if (!mesh.size())
		mesh = "shot.mesh";

	std::string name = parent->GetValueS("name");

	Entity* shot = CFirestarterLoop::instance().GetSceneManager()->createEntity(name.c_str(), mesh.c_str());
	node = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());

	if (!node)
		return;
	node->attachObject(shot);

	lastRot = 0;
	float rot[3];
	parent->GetRot(rot);
	rot[2] += 90.0f;

	float pos[3];
	parent->GetPos(pos);
	//pos[2] += 1.5f;

	node->rotate(Vector3(0,0,1),rot[2]-lastRot);
	node->translate(pos[0],pos[1],pos[2]); 
	lastRot = rot[2];
}

void CShotDrawObject::Think ( void )
{
	float rot[3];
	parent->GetRot(rot);
	rot[2] += 90.0f;

	float pos[3];
	parent->GetPos(pos);

	node->rotate(Vector3(0,0,1),rot[2]-lastRot);
	node->setPosition(pos[0],pos[1],pos[2]); 
	lastRot = rot[2];
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
	lastRot = 0;
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
	float camBump = 0.25f;

	Camera *cam = CFirestarterLoop::instance().GetCamera();

	cam->setFixedYawAxis(true);

	float deg2rad = 0.017453292519943295769236907684886f;

	cam->setPosition(pos[0],pos[1],pos[2]+camBump);

	cam->rotate(Vector3(0,0,1),rot[2]-lastRot);

	lastRot = rot[2];
//	cam->setDirection(0,0,rot[2]*deg2rad);
}

