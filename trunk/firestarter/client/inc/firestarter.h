/* Firestarter
* bznext.h :
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

#ifndef _FIRESTARTER_LOOP_H_
#define _FIRESTARTER_LOOP_H_

// main stuff
// the app needs to provide a derived CBaseGameLoop
// as the base on will just quit out

#include "gameloop.h"
#include "ui.h"
#include "Singleton.h"
#include "listserver.h"

class  CFirestarterLoop : public Singleton<CFirestarterLoop>, public CBaseGameLoop
{
public:
  CFirestarterLoop();
  virtual ~CFirestarterLoop();

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

	CListServerClientConnection & getListServer ( void ) { return listServer;}


protected:
	// global flags
	bool				inUI;
	int					numScreenShots;
	float				lastScreenShotTime;

	// UI core
	CUserInterface	ui;

	// list server
	CListServerClientConnection	listServer;

	// defaults
	void verifyDefaults ( void );
};

#endif //_FIRESTARTER_LOOP_H_

