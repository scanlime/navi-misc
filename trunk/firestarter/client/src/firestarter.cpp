/* Firestarter
* bznext.cpp :
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

#include "firestarter.h"
#include "input.h"
#include "syncedClock.h"
#include "prefs.h"
#include "commandargs.h"
#include "gameManager.h"

template <>
CFirestarterLoop* Singleton<CFirestarterLoop>::_instance = (CFirestarterLoop*)0;

CFirestarterLoop::CFirestarterLoop()
{
	SetQuit(false);
	inUI = true;
	numScreenShots = 0;
}

CFirestarterLoop::~CFirestarterLoop()
{

}
void CFirestarterLoop::verifyDefaults ( void )
{
	CPrefsManager	&prefs = CPrefsManager::instance();

	if (!prefs.ItemExists("PlayerName"))
		prefs.SetItem("PlayerName","VelvetPants");

	if (!prefs.ItemExists("PlayerMesh"))
		prefs.SetItem("PlayerMesh","mk3.mesh");

	if (!prefs.ItemExists("syncUpdateTime"))
		prefs.SetItem("syncUpdateTime",60.0f);

	prefs.Update();
}

bool CFirestarterLoop::OnInit ( void )
{
	// some default values
	SetGameName("susan");
	SetGameStartString("test");

	inUI = !CCommandLineArgs::instance().GetDataB("forceGame");

	// verify there is some default
	verifyDefaults();

	// init the shit
	ui.Init();
	CGameManger &game = CGameManger::instance();
	game.Init();

	// contact the list server and see if we can get some lists
	listServer.get();

	if (inUI)
		ui.Attach();
	else
		game.Attach();

	return false;
}

bool CFirestarterLoop::OnKill( void )
{
  return false;
}

bool CFirestarterLoop::OnActivate ( void )
{
  return false;
}

bool CFirestarterLoop::OnDeactivate ( void )
{
  return false;
}

SceneType CFirestarterLoop::GetSceneType ( void )
{
  return ST_GENERIC;
}

const char* CFirestarterLoop::GetCameraName ( void )
{
  return "playerCam";
}

const char*  CFirestarterLoop::GetRootResDir ( void )
{
	if (CPrefsManager::instance().ItemExists("dataDir"))
		return CPrefsManager::instance().GetItemS("dataDir");
  return "../data";
}

const char* CFirestarterLoop::GetPrefsName ( void )
{
	 return "firestarter/client.prefs";
}

const char* CFirestarterLoop:: GetWindowName ( void )
{
	return "Firestarter::Smack the tank up";
}

bool CFirestarterLoop::GameLoop ( void )
{
	// check for sreenshot
	CGameManger &game = CGameManger::instance();
	float togleTime = 0.5f;
	if (CInputManager::instance().KeyDown(KEY_SYSRQ) && (lastScreenShotTime < CSyncedClock::instance().GetTime()+togleTime))
	{
		char tmp[128];
		sprintf(tmp, "screenshot_%d.png", ++numScreenShots);
		GetRenderWindow()->writeContentsToFile(tmp);
		lastScreenShotTime = CSyncedClock::instance().GetTime();
		//GetRenderWindow()->setDebugText(String("Wrote ") + tmp);
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

void CFirestarterLoop::OnFrameEnd ( void )
{
}

