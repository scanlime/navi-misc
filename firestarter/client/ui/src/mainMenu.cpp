/* Firestarter
* mainMenu.cpp : 
*
* Copyright (C) 2004 Jeffrey Myers
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* email: jeffm2501@sbcglobal.net
*/
#include "mainMenu.h"
#include "firestarter.h"
#include "input.h"
#include "syncedClock.h"

CMainMenu::CMainMenu()
{
	CBaseUIPanel::CBaseUIPanel();
}

CMainMenu::~CMainMenu()
{

}

void CMainMenu::Init ( void  )
{
	CBaseUIPanel::Init();

	// init other stuff here
	ships[0] = ships[1] = ships[2] = ships[3] = NULL;

	currentItem = 0;
}

void CMainMenu::Attach ( void )
{
	CFirestarterLoop &gameLoop = CFirestarterLoop::instance();

	mainMenu = (Overlay*)OverlayManager::getSingleton().getByName("menu/mainMenu");
	mainMenu->show();

	gameLoop.GetCamera()->moveRelative(Vector3(0,1.5f,0));

	// set up a very very simple scene
	gameLoop.GetSceneManager()->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	Light* sun = gameLoop.GetSceneManager()->createLight("Sun");
	sun->setPosition(Vector3(100,100,100));
	sun->setDiffuseColour(1,1,1);
	sun->setType(Light::LT_POINT);

	gameLoop.GetSceneManager()->setSkyBox(true, "grassland_skybox",5000,true,Quaternion(1.57079632f,Vector3(1,0,0)));

	Mesh* mesh = MeshManager::getSingleton().createPlane("GroundPlane", Plane (Vector3(0,1,0),Vector3(0,0,0),Vector3(1,0,0)),800, 800,1,1,true,1,60,60);
	if (mesh && mesh->getSubMeshIterator().hasMoreElements())
	{
		mesh->getSubMeshIterator().getNext()->setMaterialName("ground_mat");
	}
	Entity *mGroundEntity = gameLoop.GetSceneManager()->createEntity("Ground","GroundPlane");
	mGroundNode = static_cast<SceneNode*>(gameLoop.GetSceneManager()->getRootSceneNode()->createChild());
	mGroundNode->attachObject(mGroundEntity);

	// make some ships
	float	z = 1.50f;
	float rad = 8.0f;

	Entity* mShip1 = gameLoop.GetSceneManager()->createEntity("MK3-1", "MK3.mesh");
	mShip1->getMesh()->getSubMeshIterator().getNext()->setMaterialName("RedkMK3");
	ships[0] = static_cast<SceneNode*>(gameLoop.GetSceneManager()->getRootSceneNode()->createChild());
	ships[0]->attachObject(mShip1);
	ships[0]->rotate(Vector3(0,0,1),180);
	ships[0]->translate(rad,0,z); 

	Entity* mShip2 = gameLoop.GetSceneManager()->createEntity("MK3-2", "MK3.mesh");
	mShip2->getMesh()->getSubMeshIterator().getNext()->setMaterialName("BlackMK3");
	ships[1] = static_cast<SceneNode*>(gameLoop.GetSceneManager()->getRootSceneNode()->createChild());
	ships[1]->attachObject(mShip2);
	ships[1]->rotate(Vector3(0,0,1),-90);
	ships[1]->translate(0,rad,z); 

	Entity* mShip3 = gameLoop.GetSceneManager()->createEntity("MK3-3", "MK3.mesh");
	mShip3->getMesh()->getSubMeshIterator().getNext()->setMaterialName("YellowMK3");
	ships[3] = static_cast<SceneNode*>(gameLoop.GetSceneManager()->getRootSceneNode()->createChild());
	ships[3]->attachObject(mShip3);
	ships[3]->rotate(Vector3(0,0,1),90);
	ships[3]->translate(0,-rad,z); 

	Entity* mShip4 = gameLoop.GetSceneManager()->createEntity("MK3-4", "MK3.mesh");
	ships[2] = static_cast<SceneNode*>(gameLoop.GetSceneManager()->getRootSceneNode()->createChild());
	ships[2]->attachObject(mShip4);
	ships[2]->rotate(Vector3(0,0,1),0);
	ships[2]->translate(-rad,0,z); 


	//random tufts

	int numTuft = 500;
	float xyRange = 200;

	for (int i = 0; i < numTuft; i++)
	{
		char name[512];
		sprintf(name,"tuft:tuft1-%d",i);

		float pos[3];
		pos[0] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange;
		pos[1] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange;
		pos[2] = 0.0f;

		Entity* tuftEnt = CFirestarterLoop::instance().GetSceneManager()->createEntity(name, "tuft1.mesh");
		tuftEnt->getMesh()->getSubMeshIterator().getNext()->setMaterialName("tuft:tuft1");
		//	tuftEnt->setRenderQueueGroup(RENDER_QUEUE_9);
		SceneNode* tuftNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
		tuftNode->attachObject(tuftEnt);
		tuftNode->rotate(Vector3(0,0,1),(((float)rand()/(float)RAND_MAX)*360));
		tuftNode->translate(pos[0],pos[1],pos[2]); 
	}

	for (int i = 0; i < numTuft; i++)
	{
		char name[512];
		sprintf(name,"tuft:tuft2-%d",i);

		float pos[3];
		pos[0] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange;
		pos[1] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange;
		pos[2] = 0.0f;

		Entity* tuftEnt = CFirestarterLoop::instance().GetSceneManager()->createEntity(name, "tuft1.mesh");
		tuftEnt->getMesh()->getSubMeshIterator().getNext()->setMaterialName("tuft:tuft2");
		//	tuftEnt->setRenderQueueGroup(RENDER_QUEUE_9);
		SceneNode* tuftNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
		tuftNode->attachObject(tuftEnt);
		tuftNode->rotate(Vector3(0,0,1),(((float)rand()/(float)RAND_MAX)*360));
		tuftNode->translate(pos[0],pos[1],pos[2]); 
	}

	for (int i = 0; i < numTuft; i++)
	{
		char name[512];
		sprintf(name,"tuft:tuft3-%d",i);

		float pos[3];
		pos[0] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange;
		pos[1] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange;
		pos[2] = 0.0f;

		Entity* tuftEnt = CFirestarterLoop::instance().GetSceneManager()->createEntity(name, "tuft1.mesh");
		tuftEnt->getMesh()->getSubMeshIterator().getNext()->setMaterialName("tuft:tuft3");
		//	tuftEnt->setRenderQueueGroup(RENDER_QUEUE_9);
		SceneNode* tuftNode = static_cast<SceneNode*>(CFirestarterLoop::instance().GetSceneManager()->getRootSceneNode()->createChild());
		tuftNode->attachObject(tuftEnt);
		tuftNode->rotate(Vector3(0,0,1),(((float)rand()/(float)RAND_MAX)*360));
		tuftNode->translate(pos[0],pos[1],pos[2]); 
	}
}

void CMainMenu::Release ( void )
{
	CFirestarterLoop &gameLoop = CFirestarterLoop::instance();

	mainMenu->hide();
	mainMenu = NULL;

	ships[0] = ships[1] = ships[2] = ships[3] = NULL;

	mGroundNode = NULL;

	gameLoop.ClearScene();
}

tePanelReturn CMainMenu::Process ( std::string &next )
{
	CFirestarterLoop &gameLoop = CFirestarterLoop::instance();

	Vector3 YawAxis(0,0,1);
	Vector3 PitchAxis(1,0,0);
	float MoveFactor = 15.0f *CSyncedClock::instance().GetFrameTime();
	gameLoop.GetCamera()->rotate(YawAxis,-MoveFactor);

	float rotSpeed = 60.0f * CSyncedClock::instance().GetFrameTime();
	if (ships[0])
	{
		ships[0]->rotate(Vector3(0,0,1),rotSpeed);
		ships[1]->rotate(Vector3(0,0,1),-rotSpeed);
		ships[2]->rotate(Vector3(0,0,1),rotSpeed);
		ships[3]->rotate(Vector3(0,0,1),-rotSpeed);
	}
	if (CInputManager::instance().KeyDown(KEY_RETURN))
	{
		// put somethign not lame here]
		switch (currentItem)
		{
		case 0:
			next = "joinMenu";
			return ePanelNext;
			break;

		case 1:
			next = "settingsMenu";
			return ePanelNext;
			break;
		}
	}
	// do the menu stuff here
	if (CInputManager::instance().KeyDown(KEY_DOWN))
		currentItem ++;
	else if (CInputManager::instance().KeyDown(KEY_UP))
		currentItem --;

	if (currentItem < 0)
		currentItem = 0;
	else if (currentItem > 1)
		currentItem = 1;

	GuiContainer* selction = mainMenu->getChild("main/selection");
	selction->setPosition(-200-64-10,175+currentItem*100);

	if (CInputManager::instance().KeyDown(KEY_Q))
		return ePanelBack;

	return ePanelContinue;
}