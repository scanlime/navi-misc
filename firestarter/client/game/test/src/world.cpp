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
// world.cpp

#include "world.h"

CTestWorld::CTestWorld()
{
		gameLoop = NULL;
}

CTestWorld::CTestWorld ( CBaseGameLoop * pGameLoop )
{
	Set(pGameLoop);
}

CTestWorld::~CTestWorld()
{

}

void CTestWorld::Set ( CBaseGameLoop * pGameLoop )
{
	gameLoop = pGameLoop;
}

void CTestWorld::Load ( bool draw )
{
	gameLoop->GetSceneManager()->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	Light* sun = gameLoop->GetSceneManager()->createLight("Sun");
	sun->setPosition(Vector3(100,100,100));
	sun->setDiffuseColour(1,1,1);
	sun->setType(Light::LT_POINT);

	gameLoop->GetSceneManager()->setSkyBox(true, "grassland_skybox",5000,true,Quaternion(1.57079632f,Vector3(1,0,0)));


	Mesh* mesh = MeshManager::getSingleton().createPlane("TheGround", Plane (Vector3(0,1,0),Vector3(0,0,0),Vector3(1,0,0)),800, 800,1,1,true,1,60,60);
	if (mesh && mesh->getSubMeshIterator().hasMoreElements())
		mesh->getSubMeshIterator().getNext()->setMaterialName("ground_mat");
	Entity *mGroundEntity = gameLoop->GetSceneManager()->createEntity("Ground1","TheGround");
	SceneNode* mGroundNode = static_cast<SceneNode*>(gameLoop->GetSceneManager()->getRootSceneNode()->createChild());
	mGroundNode->attachObject(mGroundEntity);
}

const char* CTestWorld::GetValueS ( const char *item )
{
	return NULL;
}

bool CTestWorld::Think ( void )
{
	return false;
}
