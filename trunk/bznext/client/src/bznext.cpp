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
	SetQuit(false);
	inUI = false;
	numScreenShots = 0;
}

CBZNextLoop::~CBZNextLoop()
{

}

bool CBZNextLoop::OnInit ( void )
{
	// some default values
	SetGameName("susan");
	SetGameStartString("test");

	// init the shit
	ui.Set(this);
	ui.Init();
	game.Set(this);
	game.Init();

	if (inUI)
		ui.Attach();
	else
		game.Attach();

	bool showDebug = args.Exists("showDebug");

#ifdef _SHOW_FRAME_OVERLAY
		showDebug = true;
#endif
		showDebugOverlay(showDebug);
  return false;
}

bool CBZNextLoop::OnKill( void )
{
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
	if (prefs.ItemExists("dataDir"))
		return prefs.GetItemS("dataDir");
  return "../data";
}

const char* CBZNextLoop::GetPrefsName ( void )
{
	 return "bznext/client.prefs";
}

const char* CBZNextLoop:: GetWindowName ( void )
{
	return "BZFlag::Next();";
}

const char* CBZNextLoop:: GetGameName ( void )
{
	return "bzflag";
}

bool CBZNextLoop::GameLoop ( void )
{
	// check for sreenshot
	float togleTime = 0.5f;
	if (GetInput().KeyDown(KEY_SYSRQ) && (lastScreenShotTime < GetTimer().GetTime()+togleTime))
	{
		char tmp[20];
		sprintf(tmp, "screenshot_%d.png", ++numScreenShots);
		GetRenderWindow()->writeContentsToFile(tmp);
		lastScreenShotTime = GetTimer().GetTime();
		GetRenderWindow()->setDebugText(String("Wrote ") + tmp);
	}

	if (inUI)
	{
		if (ui.Think())
		{
			ui.Release();
			inUI = false;

			if (!game.GameActive())
				game.Attach();
		}
	}
	else
	{
		if (game.Think())
		{
			game.Release();
			inUI = true;
			ui.Attach();
		}
	}
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
  guiTris->setCaption(tris + StringConverter::toString((unsigned int)stats.triangleCount));

  GuiElement* guiDbg = GuiManager::getSingleton().getGuiElement("Core/DebugText");
  guiDbg->setCaption(GetRenderWindow()->getDebugText());
}
