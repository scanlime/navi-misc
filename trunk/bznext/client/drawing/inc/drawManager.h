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
// drawManager.h

#ifndef _DRAW_MANAGER_H_
#define _DRAW_MANAGER_H_

#include "gameloop.h"
#include "baseObject.h"

class  CBZNextLoop
{
public:
  CBZNextLoop();
  virtual ~CBZNextLoop();

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
	virtual const char* GetPrefsName ( void );

	virtual const char* GetGameName ( void );

protected:
	// global flags
	bool				inUI;
	int					numScreenShots;
	float				lastScreenShotTime;

	// game manager
	CGameManger			game;

	// UI core
	CUserInterface	ui;
};

#endif //_DRAW_MANAGER_H_

