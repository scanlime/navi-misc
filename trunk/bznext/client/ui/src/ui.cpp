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
#include "ui.h"

// the individual menu items
#include "mainMenu.h"

CUserInterface::CUserInterface()
{
	gameLoop = NULL;
}

CUserInterface::CUserInterface ( CBaseGameLoop * pGameLoop )
{
	Set(pGameLoop);
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

void CUserInterface::Set ( CBaseGameLoop * pGameLoop )
{
	gameLoop = pGameLoop;
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

	panelmap::iterator itr = panels.begin();
	while (itr != panels.end())
		(itr++)->second->Init(gameLoop);

	panelStack.push_back(panels["mainMenu"]);
}

void CUserInterface::Attach ( void )	// prepare the current panel for display
{
	gameLoop->ClearScene();
	panelStack[panelStack.size()-1]->Attach();
}

void CUserInterface::Release ( void )	// let the panel display go, we are going to the game
{
	panelStack[panelStack.size()-1]->Release();
	gameLoop->ClearScene();
}

bool CUserInterface::Think ( void )
{
	bool quit = gameLoop->GetInput().KeyDown(KEY_ESCAPE);	//kinda lame but it's what we do during dev, one key to bail
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
					gameLoop->ClearScene();
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
					gameLoop->ClearScene();
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
		gameLoop->SetQuit(true);

	return exitUI;
}
