/* XXXXX
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

#include "bznext.h"

CBZNextLoop::CBZNextLoop()
{
	SetQuit(false);
	inUI = true;
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

	inUI = !args.GetDataB("forceGame");

	// init the shit
	ui.Set(this);
	ui.Init();
	game.Set(this);
	game.Init();

	if (inUI)
		ui.Attach();
	else
		game.Attach();

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
}

