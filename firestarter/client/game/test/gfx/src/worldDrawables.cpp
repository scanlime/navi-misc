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
#include "firestarter.h"

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
	CFirestarterLoop::instance().GetSceneManager()->setSkyBox(true, parent->GetValueS("skybox"),parent->GetValueF("groundSize")*4,true,Quaternion(1.57079632f,Vector3(1,0,0)));
}

void CSkyObject::Think ( void )
{
}

// ground factory
CBaseDrawable* CGroundObjectFactory::New ( CBaseObject* parent )
{
	CBaseDrawable* obj = new CGroundObject;
	obj->Set(parent);
	return obj;
}

void CGroundObjectFactory::Delete ( CBaseDrawable* object )
{
	delete(object);
}

// skybox object
CGroundObject::CGroundObject()
{
	mGroundNode = NULL;
}

CGroundObject::~CGroundObject()
{
}

void CGroundObject::Init ( void )
{
	Mesh* mesh = MeshManager::getSingleton().createPlane("GroundPlane", Plane (Vector3(0,1,0),Vector3(0,0,0),Vector3(1,0,0)),parent->GetValueF("groundSize"), parent->GetValueF("groundSize"),1,1,true,1,parent->GetValueF("groundTextureRepeat"),parent->GetValueF("groundTextureRepeat"));
	if (mesh && mesh->getSubMeshIterator().hasMoreElements())
	{
		mesh->getSubMeshIterator().getNext()->setMaterialName(parent->GetValueS("groundTexture"));
	}
	Entity *mGroundEntity = CFirestarterLoop::instance().GetSceneManager()->createEntity("Ground","GroundPlane");
	mGroundNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	mGroundNode->attachObject(mGroundEntity);
}

void CGroundObject::Think ( void )
{
}
