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

#include "bznext.h"

CBZNextLoop::CBZNextLoop()
{
  quit = false;
}

CBZNextLoop::~CBZNextLoop()
{

}

bool CBZNextLoop::OnInit ( void )
{
  showDebugOverlay(true);

  GetSceneManager()->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

	// use the ogre input for now
	mInputDevice = PlatformManager::getSingleton().createInputReader();
	mInputDevice->initialise(GetRenderWindow(),true, false);

  mSceneMgr->setSkyBox(true, "grassland_skybox",5000,true,Quaternion(1.57079632f,Vector3(1,0,0)));

  float height = 1.25f;

  ViewPoint vp = GetSceneManager()->getSuggestedViewpoint(true);
  GetCamera()->setPosition(Vector3(0,-50,height));
  GetCamera()->setOrientation(Quaternion(1.57079632f,Vector3(1,0,0)));
  return false;
}

bool CBZNextLoop::OnKill( void )
{
 // delete(mInputDevice);
  return false;
}

bool CBZNextLoop::OnActivate ( void )
{
  return false;
}

bool CBZNextLoop::OnDeactivate ( void )
{
  return false;
}

SceneType CBZNextLoop::GetSceneType ( void )
{
  return ST_GENERIC;
}

const char* CBZNextLoop::GetCameraName ( void )
{
  return "BZPlayerCam";
}

const char*  CBZNextLoop::GetRootResDir ( void )
{
  return "../data";
}

const char* CBZNextLoop::GetPrefsName ( void )
{
	 return "bznext/client.prefs";
}

void CBZNextLoop::ProcessInput ( void )
{
  Real MoveFactor = 45.0 * GetTimer().GetFrameTime();
  mInputDevice->capture();

  GetCamera()->setFixedYawAxis(true);

  // Move the ship node!
  if(mInputDevice->isKeyDown(Ogre::KC_UP))
    GetCamera()->moveRelative(Vector3(0,0,-MoveFactor));// MoveFactor,Node::TS_PARENT);

  if(mInputDevice->isKeyDown(Ogre::KC_DOWN))
    GetCamera()->moveRelative(Vector3(0,0,MoveFactor));// GetCamera()->pitch( -MoveFactor,Node::TS_PARENT );

  if(mInputDevice->isKeyDown(Ogre::KC_PGUP))
    GetCamera()->moveRelative(Vector3(0,MoveFactor*0.25f,0));// MoveFactor,Node::TS_PARENT);

  if(mInputDevice->isKeyDown(Ogre::KC_PGDOWN))
    GetCamera()->moveRelative(Vector3(0,-MoveFactor*0.25f,0));// GetCamera()->pitch( -MoveFactor,Node::TS_PARENT );

  Vector3 YawAxis(0,0,1);
  Vector3 PitchAxis(1,0,0);

  if(mInputDevice->isKeyDown(Ogre::KC_LEFT))
   GetCamera()->rotate(YawAxis,MoveFactor);

  if(mInputDevice->isKeyDown(Ogre::KC_RIGHT))
    GetCamera()->rotate(YawAxis,-MoveFactor);

  if(mInputDevice->isKeyDown(Ogre::KC_INSERT))
    GetCamera()->pitch (MoveFactor);

  if(mInputDevice->isKeyDown(Ogre::KC_DELETE))
    GetCamera()->pitch (-MoveFactor);

   if(mInputDevice->isKeyDown(Ogre::KC_ESCAPE))
      quit = true;
}

bool CBZNextLoop::GameLoop ( void )
{
  ProcessInput();
  return quit;
}
void CBZNextLoop::OnFrameEnd ( void )
{
  updateStats();
}

void CBZNextLoop::showDebugOverlay ( bool show )
{
  Overlay* o = (Overlay*)OverlayManager::getSingleton().getByName("Core/DebugOverlay");
  if (!o)
    Except( Exception::ERR_ITEM_NOT_FOUND, "Could not find overlay Core/DebugOverlay",
    "showDebugOverlay" );
  if (show)
  {
    o->show();
  }
  else
  {
    o->hide();
  }
}

void CBZNextLoop::updateStats(void)
{
  static String currFps = "Current FPS: ";
  static String avgFps = "Average FPS: ";
  static String bestFps = "Best FPS: ";
  static String worstFps = "Worst FPS: ";
  static String tris = "Triangle Count: ";

  // update stats when necessary
  GuiElement* guiAvg = GuiManager::getSingleton().getGuiElement("Core/AverageFps");
  GuiElement* guiCurr = GuiManager::getSingleton().getGuiElement("Core/CurrFps");
  GuiElement* guiBest = GuiManager::getSingleton().getGuiElement("Core/BestFps");
  GuiElement* guiWorst = GuiManager::getSingleton().getGuiElement("Core/WorstFps");

  const RenderTarget::FrameStats& stats = GetRenderWindow()->getStatistics();

  guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
  guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
  guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
    +" "+StringConverter::toString(stats.bestFrameTime)+" ms");
  guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
    +" "+StringConverter::toString(stats.worstFrameTime)+" ms");

  GuiElement* guiTris = GuiManager::getSingleton().getGuiElement("Core/NumTris");
  guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

  GuiElement* guiDbg = GuiManager::getSingleton().getGuiElement("Core/DebugText");
  guiDbg->setCaption(GetRenderWindow()->getDebugText());
}
