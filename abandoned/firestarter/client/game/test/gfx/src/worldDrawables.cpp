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
	Entity  *ball;
	SceneNode *ballNode;

	// make the Zaxis stuff
	ball = CFirestarterLoop::instance().GetSceneManager()->createEntity("zball", "zball.mesh");
	ballNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	ballNode->attachObject(ball);
	ballNode->translate(0,0,10);
	ballNode->rotate(Vector3(1,0,0),90);
#endif //_DEBUG

	 // tufts
	std::vector<trFlairDef> *flairGroups = (std::vector<trFlairDef>*)parent->GetValueI("flairGroups");

	if (flairGroups)
	{
		std::vector<trFlairDef>::iterator itr = flairGroups->begin();

		int baseName = 0;
		while (itr != flairGroups->end())
		{

			float xyRange = itr->range;
			float scaleRange = itr->scale[1] - itr->scale[0];

			for (int i = 0; i < itr->count; i++)
			{
				char name[512];
				sprintf(name,"flair:flair%d-%d",baseName,i);

				float pos[3];
				pos[0] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange + itr->center[0];
				pos[1] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange + itr->center[1];
				pos[2] = 0.0f;

				Entity* tuftEnt = CFirestarterLoop::instance().GetSceneManager()->createEntity(name, itr->mesh.c_str());
				SceneNode* tuftNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
				tuftNode->attachObject(tuftEnt);
				tuftNode->rotate(Vector3(0,0,1),(((float)rand()/(float)RAND_MAX)*360));
				tuftNode->translate(pos[0],pos[1],pos[2]); 

				float scale = (((float)rand()/(float)RAND_MAX)* scaleRange) + itr->scale[0];
				tuftNode->scale(scale,scale,scale);
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
	mWallNode[0] = mWallNode[1] = mWallNode[2] = mWallNode[3] = NULL;
}

CWallObject::~CWallObject()
{
}

void CWallObject::Init ( void )
{
	float x = parent->GetValueF("groundSizeX");
	float y = parent->GetValueF("groundSizeY");
	float h = parent->GetValueF("wallHeight");
	float rep = parent->GetValueF("groundTextureRepeat");
	std::string	wallTex = parent->GetValueS("wallTexture");

	Mesh* mesh = MeshManager::getSingleton().createPlane("wall0", Plane (Vector3(0,1,0),Vector3(0,0,0),Vector3(1,0,0)),x,h,1,1,true,1,rep,h/x*rep);
	if (mesh && mesh->getSubMeshIterator().hasMoreElements())
	{
		mesh->getSubMeshIterator().getNext()->setMaterialName(wallTex.c_str());
	}
	Entity *mWallEntity = CFirestarterLoop::instance().GetSceneManager()->createEntity("wall01","wall0");
	mWallEntity->setRenderQueueGroup(RENDER_QUEUE_2);
	mWallNode[0] = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	mWallNode[0]->attachObject(mWallEntity);
	mWallNode[0]->pitch(90.0f);
	mWallNode[0]->translate(0,y*0.5f,h*0.5f);

	Mesh* mesh2 = MeshManager::getSingleton().createPlane("wall1", Plane (Vector3(0,1,0),Vector3(0,0,0),Vector3(1,0,0)),x,h,1,1,true,1,rep,h/x*rep);
	if (mesh2 && mesh2->getSubMeshIterator().hasMoreElements())
	{
		mesh2->getSubMeshIterator().getNext()->setMaterialName(wallTex.c_str());
	}
	Entity *mWallEntity2 = CFirestarterLoop::instance().GetSceneManager()->createEntity("wall02","wall1");
	mWallEntity2->setRenderQueueGroup(RENDER_QUEUE_2);
	mWallNode[1] = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	mWallNode[1]->attachObject(mWallEntity2);
	mWallNode[1]->roll(180.0);
	mWallNode[1]->pitch(90.0f);
	mWallNode[1]->translate(0,-y*0.5f,h*0.5f);

	Mesh* mesh3 = MeshManager::getSingleton().createPlane("wall2", Plane (Vector3(0,1,0),Vector3(0,0,0),Vector3(1,0,0)),y,h,1,1,true,1,rep,h/y*rep);
	if (mesh3 && mesh3->getSubMeshIterator().hasMoreElements())
	{
		mesh3->getSubMeshIterator().getNext()->setMaterialName(wallTex.c_str());
	}
	Entity *mWallEntity3 = CFirestarterLoop::instance().GetSceneManager()->createEntity("wall03","wall2");
	mWallEntity3->setRenderQueueGroup(RENDER_QUEUE_2);
	mWallNode[2] = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	mWallNode[2]->attachObject(mWallEntity3);
	mWallNode[2]->roll(90.0);
	mWallNode[2]->pitch(90.0f);
	mWallNode[2]->translate(-x*0.5f,0,h*0.5f);


	Mesh* mesh4 = MeshManager::getSingleton().createPlane("wall3", Plane (Vector3(0,1,0),Vector3(0,0,0),Vector3(1,0,0)),y,h,1,1,true,1,rep,h/y*rep);
	if (mesh4 && mesh4->getSubMeshIterator().hasMoreElements())
	{
		mesh4->getSubMeshIterator().getNext()->setMaterialName(wallTex.c_str());
	}
	Entity *mWallEntity4 = CFirestarterLoop::instance().GetSceneManager()->createEntity("wall04","wall3");
	mWallEntity4->setRenderQueueGroup(RENDER_QUEUE_2);
	mWallNode[3] = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
	mWallNode[3]->attachObject(mWallEntity4);
	mWallNode[3]->roll(-90.0);
	mWallNode[3]->pitch(90.0f);
	mWallNode[3]->translate(x*0.5f,0,h*0.5f);
}

void CWallObject::Think ( void )
{
}
