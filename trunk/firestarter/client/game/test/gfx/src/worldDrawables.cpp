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
#include "world.h"

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
	CFirestarterLoop::instance().GetSceneManager()->setSkyBox(true, parent->GetValueS("skybox"),parent->GetValueF("groundSizeX")*4,true,Quaternion(1.57079632f,Vector3(1,0,0)));
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
	Mesh* mesh = MeshManager::getSingleton().createPlane("GroundPlane", Plane (Vector3(0,1,0),Vector3(0,0,0),Vector3(1,0,0)),parent->GetValueF("groundSizeX"), parent->GetValueF("groundSizeY"),1,1,true,1,parent->GetValueF("groundTextureRepeat"),parent->GetValueF("groundTextureRepeat"));
	if (mesh && mesh->getSubMeshIterator().hasMoreElements())
	{
		mesh->getSubMeshIterator().getNext()->setMaterialName(parent->GetValueS("groundTexture"));
	}
	Entity *mGroundEntity = CFirestarterLoop::instance().GetSceneManager()->createEntity("Ground","GroundPlane");
	mGroundEntity->setRenderQueueGroup(RENDER_QUEUE_2);
	mGroundNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	mGroundNode->attachObject(mGroundEntity);

#ifdef _DEBUG
	// make the Xaxis stuff
	Entity  *ball = CFirestarterLoop::instance().GetSceneManager()->createEntity("xball", "xball.mesh");
	SceneNode *ballNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	ballNode->attachObject(ball);
	ballNode->translate(10,0,0);

	// make the Yaxis stuff
	ball = CFirestarterLoop::instance().GetSceneManager()->createEntity("yball", "yball.mesh");
	ballNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	ballNode->attachObject(ball);
	ballNode->translate(0,10,0);
	ballNode->rotate(Vector3(1,0,0),90);

	// make the Zaxis stuff
	ball = CFirestarterLoop::instance().GetSceneManager()->createEntity("zball", "zball.mesh");
	ballNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	ballNode->attachObject(ball);
	ballNode->translate(0,0,10);
	ballNode->rotate(Vector3(1,0,0),90);
#endif //_DEBUG

	 // tufts
	std::vector<trTuftDef> *tufts = (std::vector<trTuftDef>*)parent->GetValueI("tufts");

	if (tufts)
	{
		std::vector<trTuftDef>::iterator itr = tufts->begin();

		int baseName = 0;
		while (itr != tufts->end())
		{

			float xyRange = itr->range;

			for (int i = 0; i < itr->count; i++)
			{
				char name[512];
				sprintf(name,"tuft:tuft%d-%d",baseName,i);

				float pos[3];
				pos[0] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange + itr->center[0];
				pos[1] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange + itr->center[1];
				pos[2] = 0.0f;

				Entity* tuftEnt = CFirestarterLoop::instance().GetSceneManager()->createEntity(name, itr->mesh.c_str());
				//tuftEnt->getMesh()->getSubMeshIterator().getNext()->setMaterialName("tuft:tuft1");
				//	tuftEnt->setRenderQueueGroup(RENDER_QUEUE_9);
				SceneNode* tuftNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
				tuftNode->attachObject(tuftEnt);
				tuftNode->rotate(Vector3(0,0,1),(((float)rand()/(float)RAND_MAX)*360));
				tuftNode->translate(pos[0],pos[1],pos[2]); 
			}
			itr++;
			baseName++;
		}
	}
}

void CGroundObject::Think ( void )
{
}

// wall factory
CBaseDrawable* CWallObjectFactory::New ( CBaseObject* parent )
{
	CBaseDrawable* obj = new CWallObject;
	obj->Set(parent);
	return obj;
}

void CWallObjectFactory::Delete ( CBaseDrawable* object )
{
	delete(object);
}

// wall object
CWallObject::CWallObject()
{
	mGroundNode = NULL;
}

CWallObject::~CWallObject()
{
}

void CWallObject::Init ( void )
{
	Mesh* mesh = MeshManager::getSingleton().createPlane("wall0", Plane (Vector3(0,1,0),Vector3(0,0,0),Vector3(1,0,0)),parent->GetValueF("groundSizeX"), parent->GetValueF("groundSizeY"),1,1,true,1,parent->GetValueF("groundTextureRepeat"),parent->GetValueF("groundTextureRepeat"));
	if (mesh && mesh->getSubMeshIterator().hasMoreElements())
	{
		mesh->getSubMeshIterator().getNext()->setMaterialName(parent->GetValueS("wallTexture"));
	}
	Entity *mWallEntity[0] = CFirestarterLoop::instance().GetSceneManager()->createEntity("Ground","GroundPlane");
	mGroundEntity->setRenderQueueGroup(RENDER_QUEUE_2);
	mGroundNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	mGroundNode->attachObject(mGroundEntity);
}

void CWallObject::Think ( void )
{
}
