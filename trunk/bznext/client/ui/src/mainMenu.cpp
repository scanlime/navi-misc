/* 
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
#include "mainMenu.h"

CMainMenu::CMainMenu()
{
	CBaseUIPanel::CBaseUIPanel();
}

CMainMenu::~CMainMenu()
{

}

CMainMenu::Init ( CBaseGameLoop * pGameLoop )
{
	CBaseUIPanel::Init(pGameLoop);

	// init other stuff here
}

void CMainMenu::Attach ( void )
{
	Overlay* o = (Overlay*)OverlayManager::getSingleton().getByName("menu/mainMenu");
	o->show();

	gameLoop->GetCamera()->moveRelative(Vector3(0,1.5f,0));

	// set up a very very simple scene
	gameLoop->GetSceneManager()->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	Light* sun = gameLoop->GetSceneManager()->createLight("Sun");
	sun->setPosition(Vector3(100,100,100));
	sun->setDiffuseColour(1,1,1);
	sun->setType(Light::LT_POINT);

	gameLoop->GetSceneManager()->setSkyBox(true, "grassland_skybox",5000,true,Quaternion(1.57079632f,Vector3(1,0,0)));

	Mesh* mesh = MeshManager::getSingleton().createPlane("GroundPlane", Plane (Vector3(0,1,0),Vector3(0,0,0),Vector3(1,0,0)),800, 800,1,1,true,1,60,60);
	mesh->getSubMeshIterator().getNext()->setMaterialName("ground_mat");
	Entity *mGroundEntity = gameLoop->GetSceneManager()->createEntity("Ground","GroundPlane");
	SceneNode * mGroundNode = static_cast<SceneNode*>(gameLoop->GetSceneManager()->getRootSceneNode()->createChild());
	mGroundNode->attachObject(mGroundEntity);
}

void CMainMenu::Release ( void )
{
}

tePanelReturn CMainMenu::Process ( std::string &next )
{
	Vector3 YawAxis(0,0,1);
	Vector3 PitchAxis(1,0,0);
	float MoveFactor = 15.0f *gameLoop->GetTimer().GetFrameTime();
	gameLoop->GetCamera()->rotate(YawAxis,-MoveFactor);
	return ePanelContinue;
}