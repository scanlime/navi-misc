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
	ships[0] = ships[1] = ships[2] = ships[3] = NULL;
}

void CMainMenu::Attach ( void )
{
	mainMenu = (Overlay*)OverlayManager::getSingleton().getByName("menu/mainMenu");
	mainMenu->show();

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
	mGroundNode = static_cast<SceneNode*>(gameLoop->GetSceneManager()->getRootSceneNode()->createChild());
	mGroundNode->attachObject(mGroundEntity);

	// make some ships
	float	z = 1.50f;
	float rad = 8.0f;

	Entity* mShip1 = gameLoop->GetSceneManager()->createEntity("MK3-1", "MK3.mesh");
	mShip1->getMesh()->getSubMeshIterator().getNext()->setMaterialName("RedkMK3");
	ships[0] = static_cast<SceneNode*>(gameLoop->GetSceneManager()->getRootSceneNode()->createChild());
	ships[0]->attachObject(mShip1);
	ships[0]->rotate(Vector3(0,0,1),180);
	ships[0]->translate(rad,0,z); 

	Entity* mShip2 = gameLoop->GetSceneManager()->createEntity("MK3-2", "MK3.mesh");
	mShip2->getMesh()->getSubMeshIterator().getNext()->setMaterialName("BlackMK3");
	ships[1] = static_cast<SceneNode*>(gameLoop->GetSceneManager()->getRootSceneNode()->createChild());
	ships[1]->attachObject(mShip2);
	ships[1]->rotate(Vector3(0,0,1),-90);
	ships[1]->translate(0,rad,z); 

	Entity* mShip3 = gameLoop->GetSceneManager()->createEntity("MK3-3", "MK3.mesh");
	mShip3->getMesh()->getSubMeshIterator().getNext()->setMaterialName("YellowMK3");
	ships[3] = static_cast<SceneNode*>(gameLoop->GetSceneManager()->getRootSceneNode()->createChild());
	ships[3]->attachObject(mShip3);
	ships[3]->rotate(Vector3(0,0,1),90);
	ships[3]->translate(0,-rad,z); 

	Entity* mShip4 = gameLoop->GetSceneManager()->createEntity("MK3-4", "MK3.mesh");
//	mShip4->getMesh()->getSubMeshIterator().getNext()->setMaterialName("BlackMK3");
	ships[2] = static_cast<SceneNode*>(gameLoop->GetSceneManager()->getRootSceneNode()->createChild());
	ships[2]->attachObject(mShip4);
	ships[2]->rotate(Vector3(0,0,1),0);
	ships[2]->translate(-rad,0,z); 
}

void CMainMenu::Release ( void )
{
	mainMenu->hide();
//	OverlayManager::getSingleton().unload(mainMenu);
	mainMenu = NULL;

/*	gameLoop->GetSceneManager()->getRootSceneNode()->removeAllChildren();
	gameLoop->GetSceneManager()->destroyAllOverlays();
	gameLoop->GetSceneManager()->removeAllEntities();
	gameLoop->GetSceneManager()->removeAllLights();

	return;
	ships[0]->detachAllObjects();
	gameLoop->GetSceneManager()->destroySceneNode(ships[0]->getName());
	ships[1]->detachAllObjects();
	gameLoop->GetSceneManager()->destroySceneNode(ships[1]->getName());
	ships[2]->detachAllObjects();
	gameLoop->GetSceneManager()->destroySceneNode(ships[2]->getName());
	ships[3]->detachAllObjects();
	gameLoop->GetSceneManager()->destroySceneNode(ships[3]->getName()); */
	ships[0] = ships[1] = ships[2] = ships[3] = NULL;

//	mGroundNode->detachAllObjects();
//	gameLoop->GetSceneManager()->destroySceneNode(mGroundNode->getName());
	mGroundNode = NULL;

	gameLoop->ClearScene();
}

tePanelReturn CMainMenu::Process ( std::string &next )
{
	Vector3 YawAxis(0,0,1);
	Vector3 PitchAxis(1,0,0);
	float MoveFactor = 15.0f *gameLoop->GetTimer().GetFrameTime();
	gameLoop->GetCamera()->rotate(YawAxis,-MoveFactor);

	float rotSpeed = 60.0f * gameLoop->GetTimer().GetFrameTime();
	if (ships[0])
	{
		ships[0]->rotate(Vector3(0,0,1),rotSpeed);
		ships[1]->rotate(Vector3(0,0,1),-rotSpeed);
		ships[2]->rotate(Vector3(0,0,1),rotSpeed);
		ships[3]->rotate(Vector3(0,0,1),-rotSpeed);
	}
	if (gameLoop->GetInput().KeyDown(KEY_RETURN))
	{
		// put somethign not lame here
		gameLoop->SetGameName("susan");
		gameLoop->SetGameStartString("test");
		return ePanelStart;
	}

	if (gameLoop->GetInput().KeyDown(KEY_Q))
		return ePanelBack;

	return ePanelContinue;
}