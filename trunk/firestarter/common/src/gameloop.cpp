/* Firestarter
* gameloop.cpp :
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

// TODO
//
// Add other OS-specific stuff
//

#include "gameloop.h"
#include "events.h"
#include "osfile.h"

#include "input.h"
#include "syncedClock.h"
#include "prefs.h"
#include "commandargs.h"
#include "drawManager.h"

// C RunTime Header Files
#include <stdlib.h>
#include <map>
#include <string>
#include <stdio.h>

std::string GetUpperString( char *szData )
{
	const std::string	ret = szData;
#ifdef	_WIN32
	strupr((char*)ret.c_str());
#else
	char *data = (char*)ret.c_str();
	while(*data) {
		*data = toupper(*data);
		data++;
	}
#endif
	return ret;
}

CBaseGameLoop::CBaseGameLoop()
{
  mRoot = 0;
	showDebug = false;
}

CBaseGameLoop::~CBaseGameLoop()
{
}

bool CBaseGameLoop::Main ( const char* arg)
{
	CCommandLineArgs::instance().Set(arg);
  return Run();
}

bool CBaseGameLoop::Main ( int argc, char *argv[] )
{
  CCommandLineArgs::instance().Set(argc,argv);
  return Run();
}

void CBaseGameLoop::ClearScene ( void )
{
	mWindow->removeAllViewports();
	GetSceneManager()->removeAllCameras();
	GetSceneManager()->removeAllEntities();
	GetSceneManager()->clearScene();
	MeshManager::getSingleton().unloadAndDestroyAll();
	CreateCamera();
	CreateViewports();
	CInputManager::instance().Init(GetRenderWindow());
	showDebugOverlay(showDebug);
}

bool CBaseGameLoop::Run ( void )
{
  mRoot = new Root();

	LoadPlugins();
  SetupResources();
  
  if (!SetupConfigure())
    return false;

  ChooseSceneManager();
  CreateCamera();
  CreateViewports();

  TextureManager::getSingleton().setDefaultNumMipMaps(5);

	CInputManager::instance().Init(GetRenderWindow());
	CSyncedClock::instance().Init();
	
	// set up the draw manager
	CDrawManager::instance().Init();

	// load the debug overlay
	showDebug = CCommandLineArgs::instance().GetDataB("showDebug");
	showDebugOverlay(showDebug);

  if(OnInit())
  {
    OnKill();
    return false;
  }
  mRoot->addFrameListener(this);

	mRoot->startRendering();
  OnKill();
  return true;
}

bool CBaseGameLoop::LoadPlugins ( void )
{
	COSDir	rootDir;
	COSFile	file;
	std::map<std::string,bool>	resList;
	std::string extension;

	rootDir.SetStdDir(GetPluginsDir());
	std::string libExtenstion;
#ifdef _WIN32
		libExtenstion = "*.dll";
#else
	libExtenstion = "*.so";
#endif _WIN32

	while (rootDir.GetNextFile(file,libExtenstion.c_str(),true))
		mRoot->loadPlugin(file.GetFullOSPath());

	return true;
}

bool CBaseGameLoop::SetupConfigure ( void )
{
		CPrefsManager::instance().Init((char*)GetPrefsName());

    bool bNew = !CPrefsManager::instance().ItemExists("VERSION");
    bool configed = false;
    bool force = CCommandLineArgs::instance().Exists("ForceConfig");

		if ( !bNew && !force)
			configed =  mRoot->restoreConfig();
  
    if (!configed)
         configed = mRoot->showConfigDialog();

    if (configed)
    {
      mRoot->saveConfig();
			mWindow = mRoot->initialise(true,	GetWindowName());
    }
    if (configed && bNew)
    {
      CPrefsManager::instance().SetItem("VERSION",1);
      CPrefsManager::instance().Update();
    }
    return configed;
}

bool CBaseGameLoop::SetupResources ( void )
{
  // Don't load resource paths from config file since it doesn't do recursion.
  // As for the root dir, use our spiffy OSDir class and get all the stuff that is in there.
  // Fire in zips as zip.

  COSDir	rootDir;
  COSFile	file;
  std::map<std::string,bool>	resList;
  std::string extension;

  rootDir.SetStdDir(GetRootResDir());
  while (rootDir.GetNextFile(file,"*.*",true))
  {
	  extension = GetUpperString((char*)file.GetExtension());

	  if (extension=="ZIP")
	    resList[(std::string(file.GetOSName()))]=true;
	  else
	    resList[std::string(file.GetOSFileDir())]=false;
  }

  std::map<std::string,bool>::iterator	itr = resList.begin();
  while (itr!=resList.end())
  {
	  ResourceManager::addCommonArchiveEx( itr->first, itr->second ? "Zip" : "FileSystem" );
	  itr++;
  }

  return true;
}

bool CBaseGameLoop::frameStarted(const FrameEvent& evt)
{
  return !Process();
}

bool CBaseGameLoop::frameEnded(const FrameEvent& evt)
{
	if (showDebug)
		updateStats();

  OnFrameEnd();
  return true;
}

void CBaseGameLoop::OnFrameEnd ( void )
{
}

bool CBaseGameLoop::Process ( void )
{
  bool bDone = false;

  CInputManager::instance().Process();
  CSyncedClock::instance().Update();
	
  if (!bDone)
    bDone = GameLoop();

  return bDone;
}

void CBaseGameLoop::ChooseSceneManager ( void )
{
  mSceneMgr = mRoot->getSceneManager(GetSceneType());
}

void CBaseGameLoop::CreateCamera ( void )
{
  std::string name = "PlayerCam";
  if(GetCameraName())
    name = GetCameraName();

   mCamera = mSceneMgr->createCamera(name);
	 float height = 1.25f;
	 mCamera->setPosition(Vector3(0,0,height));
	 mCamera->setOrientation(Quaternion(1.57079632f,Vector3(1,0,0)));
   mCamera->setNearClipDistance(0.01f);
}

void CBaseGameLoop::CreateViewports ( void )
{
  Viewport* vp = mWindow->addViewport(mCamera);
  vp->setBackgroundColour(ColourValue(0,0,0));
}

// Provide your own version of these.

SceneType CBaseGameLoop::GetSceneType ( void )
{
  return ST_GENERIC;
}

const char* CBaseGameLoop::GetCameraName ( void )
{
  return "PlayerCam";
}

const char* CBaseGameLoop::GetRootResDir ( void )
{
	return "./data";
}

const char* CBaseGameLoop::GetPluginsDir ( void )
{
	return "./plugins";
}

const char* CBaseGameLoop::GetPrefsName ( void )
{
	return "std.prefs";
}

const char*CBaseGameLoop:: GetWindowName ( void )
{
	return "A Game Window";
}

bool	CBaseGameLoop::OnInit ( void )
{
	return true;
}

bool CBaseGameLoop::OnKill( void )
{
	return true;
}

bool	CBaseGameLoop::OnActivate ( void )
{
	return true;
}

bool CBaseGameLoop::OnDeactivate ( void )
{
	return true;
}

bool CBaseGameLoop::GameLoop ( void )
{
	return true;
}

void CBaseGameLoop::SetGameName ( const char* name )
{
	if(!name)
		gameName = "";
	else
		gameName = name;
}

const char* CBaseGameLoop::GetGameName (  void )
{
	return gameName.c_str();
}

void CBaseGameLoop::SetGameStartString ( const char *startString )
{
	if(!startString)
		gameStartString = "";
	else
		gameStartString = startString;
}

void CBaseGameLoop::showDebugOverlay ( bool show )
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
void CBaseGameLoop::updateStats(void)
{
	static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";

	// Update stats when necessary.
	
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