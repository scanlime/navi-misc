/* Firestarter
* ui.cpp :
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
#include "ui.h"

#include "firestarter.h"
#include "input.h"

// the individual menu items
#include "mainMenu.h"
#include "joinMenu.h"

CUserInterface::CUserInterface()
{
}

CUserInterface::~CUserInterface()
{
	panelmap::iterator itr = panels.begin();
	while (itr != panels.end())
	{
		delete(itr->second);
		itr++;
	}
	panels.clear();
	panelStack.clear();
}


void CUserInterface::Init ( void )
{
	// init any states and stuff that is needed here
	// load pannels or whatnot
	LoadPanels();
}

void CUserInterface::LoadPanels ( void )	// load and init all panels
{
	panels["mainMenu"] = (CBaseUIPanel*)new CMainMenu;
	panels["joinMenu"] = (CBaseUIPanel*)new CJoinMenu;

	panelmap::iterator itr = panels.begin();
	while (itr != panels.end())
		(itr++)->second->Init();

	panelStack.push_back(panels["mainMenu"]);
}

void CUserInterface::Attach ( void )	// prepare the current panel for display
{
	//gameLoop->ClearScene();
	panelStack[panelStack.size()-1]->Attach();
}

void CUserInterface::Release ( void )	// let the panel display go, we are going to the game
{
	panelStack[panelStack.size()-1]->Release();
}

bool CUserInterface::Think ( void )
{
	CFirestarterLoop &gameLoop = CFirestarterLoop::instance();

	bool quit = CInputManager::instance().KeyDown(KEY_ESCAPE);	//kinda lame but it's what we do during dev, one key to bail
	bool exitUI = false;	// do we exit to the game or not?

	if (!quit)
	{
		std::string	next;		// run the current panel
		tePanelReturn	ret = panelStack[panelStack.size()-1]->Process(next);
		// then check it's results
		switch (ret)
		{
			default:	// just keep on going
			case ePanelContinue:
				break;

			case ePanelExit:	// we are done with the app for some reason
				quit = true;
			break;

			case ePanelBack:	// go back to the last paenl
				if (panelStack.size()>1)	// pop us off the stack.
				{
					panelStack[panelStack.size()-1]->Release();
					panelStack.pop_back();
		//			gameLoop->ClearScene();
					panelStack[panelStack.size()-1]->Attach();
				}
				else	// if there are no more panels I guess we are done
					quit = true;
			break;

			case ePanelNext:	// move to some new panel
			{
				panelmap::iterator itr = panels.find(next);
				if (itr != panels.end())	// if a panel by that name exists then go to it ( don't get in a loop )
				{
					panelStack[panelStack.size()-1]->Release();
					panelStack.push_back(itr->second);
		//			gameLoop->ClearScene();
					panelStack[panelStack.size()-1]->Attach();
				}
			}
			break;

			case ePanelStart:	// exit the UI and begin the game
				exitUI = true;
			break;
		}
	}

	if (quit)	// tell the game loop to exit the app
		gameLoop.SetQuit(true);

	return exitUI;
}
