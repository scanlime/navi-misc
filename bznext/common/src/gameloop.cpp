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
// game loop.cpp
// ***** TODO
//
// Add other OS stuff
//
#include "gameloop.h"
#include "events.h"
#include "osfile.h"
#include <SDL/SDL.h>
// C RunTime Header Files
#include <stdlib.h>
#include <map>
#include <string>
#include <stdio.h>


void OSSleep ( float time )
{
#ifdef _WIN32
  Sleep((DWORD)(1000.0f * time));
#else
  usleep((unsigned int )(100000 * time));
#endif
}

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
}

CBaseGameLoop::~CBaseGameLoop()
{
}

bool CBaseGameLoop::Main ( const char* arg)
{
  args.Set(arg);
  return Run();
}

bool CBaseGameLoop::Main ( int argc, char *argv[] )
{
  args.Set(argc,argv);
  return Run();
}

void CBaseGameLoop::ClearScene ( void )
{
//	return;

	mWindow->removeAllViewports();
	GetSceneManager()->removeAllCameras();
	GetSceneManager()->clearScene();
	ChooseSceneManager();
	CreateCamera();
	CreateViewports();

	TextureManager::getSingleton().setDefaultNumMipMaps(5);
	input.Init(GetRenderWindow());

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

	input.Init(GetRenderWindow());
  timer.Init();

  if(OnInit())
  {
    OnKill();
    return false;
  }
  mRoot->addFrameListener(this);

  while ( !mWindow->isClosed() && mRoot->renderOneFrame())
    OSSleep(0.001f);

  if (!mWindow->isClosed())
    mRoot->shutdown();

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
    prefs.Init((char*)GetPrefsName());

    bool bNew = !prefs.ItemExists("VERSION");
    bool configed = false;
    bool force = args.Exists("ForceConfig");

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
      prefs.SetItem("VERSION",1);
      prefs.Update();
    }
    return configed;
}

bool CBaseGameLoop::SetupResources ( void )
{
  // Don't Load resource paths from config file since it don't do recursion
  // as for the root dir, use our spiffy OSDir class and get all the stuff that is in there
  // fire in zips as zip
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
  OnFrameEnd();
  return true;
}

void CBaseGameLoop::OnFrameEnd ( void )
{
}

bool CBaseGameLoop::Process ( void )
{
  SDL_Event event;

  bool bDone = false;

  //get all the events and dispatch them
  // then call game loop
  while (SDL_PollEvent(&event)) 
  {
    switch(event.type)
    {
      case SDL_QUIT:
        bDone = true;
      break;

      case SDL_ACTIVEEVENT:
        if (event.active.gain == 1)
	  OnActivate();
        else
	  OnDeactivate();
      break;
    }
    CallEventHandaler(event.type,&event);
  }  
  
  input.Process();
  timer.Update();
	
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

// provide your own version of these

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

const char*CBaseGameLoop:: GetGameName ( void )
{
	return "SomeGame";
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

void CBaseGameLoop::SetGameStartString ( const char *startString )
{
	if(!startString)
		gameStartString = "";
	else
		gameStartString = startString;
}




