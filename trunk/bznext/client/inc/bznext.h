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
// project24.h

#ifndef _BZNEXT_LOOP_H_
#define _BZNEXT_LOOP_H_

// main stuff
// the app needs to provide a derived CBaseGameLoop
// as the base on will just quit out

typedef enum
{
	eBox,
	ePyramid
}teWorldObjectType;

typedef struct 
{
	teWorldObjectType	type;
	float pos[3];
	float scale[3];
	float rot;
}trWorldObject;

#include "gameloop.h"

class  CBZNextLoop : public CBaseGameLoop
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

  virtual SceneType GetSceneType ( void );
  virtual const char* GetCameraName ( void );
  virtual const char* GetRootResDir ( void );
	virtual const char* GetPrefsName ( void );

protected:
  void ProcessInput ( void );
  void CreateGroundPlane ( void );
  void updateStats ( void );
  void showDebugOverlay(bool show);

  bool        quit;
  Entity*     mShip;
  SceneNode*  mShipNode;
  SceneNode*  mGroundNode;
  Entity*     mGroundEntity;

  int         excapeID;

  // ogre input stuff
  EventProcessor* mEventProcessor;
  InputReader* mInputDevice;

  std::vector<trWorldObject>	world;
};

#endif //_BZNEXT_LOOP_H_

