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
// Game Loop.h

#ifndef _GAME_LOOP_H_
#define _GAME_LOOP_H_

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "commandargs.h"
#include "prefs.h"
#include "input.h"
#include "timer.h"

#include <string>

using namespace Ogre;

class CDrawManager;

// main stuff
// the app needs to provide a derived CBaseGameLoop
// as the base on will just quit out

class  CBaseGameLoop : public FrameListener
{
public:
  CBaseGameLoop();
  virtual ~CBaseGameLoop();

  bool Main ( const char* args );
  bool Main ( int argc, char *argv[] );

  virtual bool OnInit ( void );
  virtual bool OnKill( void );

  virtual bool OnActivate ( void );
  virtual bool OnDeactivate ( void );

  virtual bool GameLoop ( void );
  virtual void OnFrameEnd ( void );

	virtual const char* GetWindowName ( void );
  virtual SceneType GetSceneType ( void );
  virtual const char* GetCameraName ( void );
  virtual const char* GetRootResDir ( void );
	virtual const char* GetPluginsDir ( void );
	virtual const char* GetPrefsName ( void );

	virtual const char* GetGameName ( void );

 // info methods.

  Camera* GetCamera ( void ) {return mCamera;}
  Root* GetRoot ( void ) {return mRoot;}
  SceneManager* GetSceneManager ( void ) {return mSceneMgr;}
  RenderWindow* GetRenderWindow ( void ) {return mWindow;}

  CCommandLineArgs& GetArgs ( void ) {return args;}
  CPrefsManager& GetPrefs ( void ) {return prefs;}

  CInputManager& GetInput ( void ) {return input;}
  CTimer& GetTimer ( void ) {return timer;}
  CDrawManager* GetDrawManager ( void ) {return drawManager;}

	// common operations
	void ClearScene ( void );

  // OGRE frame listener
  bool frameStarted(const FrameEvent& evt);
  bool frameEnded(const FrameEvent& evt);

	// global quit handaler
	void SetQuit ( bool set ) {quit = set;}

	void SetGameName ( const char* name );
	void SetGameStartString ( const char *startString );

protected:
	bool LoadPlugins ( void );
  bool SetupConfigure ( void );
  bool SetupResources ( void );
  bool Process( void );
  bool Run ( void );

  void ChooseSceneManager ( void );
  void CreateCamera ( void );
  void CreateViewports ( void );

	void updateStats ( void );
	void showDebugOverlay(bool show);

  CCommandLineArgs  args;
  CPrefsManager     prefs;
  CInputManager			input;
  CTimer            timer;
	CDrawManager			*drawManager;

  Root *mRoot;
  Camera* mCamera;
  SceneManager* mSceneMgr;
  RenderWindow* mWindow;


	bool				quit;
	std::string	gameName;
	std::string gameStartString;

	bool				showDebug;
};

#endif //_GAME_LOOP_H_

