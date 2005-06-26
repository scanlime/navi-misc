/*
 * MainMenu.cpp - main menu pages for the client
 *
 * Copyright (C) 2005 David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <iostream>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include "MainMenu.h"

namespace Screamers {
namespace Client {

MainMenu::MainMenu ()
{
	CEGUI::WindowManager &manager = CEGUI::WindowManager::getSingleton ();
	sheet = manager.loadWindowLayout ((CEGUI::utf8*) "main-menu.layout");
	manager.getWindow ((CEGUI::utf8*) "MainMenu/JoinButton")->subscribeEvent (CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&MainMenu::handleJoin, this));
	manager.getWindow ((CEGUI::utf8*) "MainMenu/QuitButton")->subscribeEvent (CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&MainMenu::handleQuit, this));
}

MainMenu::~MainMenu ()
{
}

bool MainMenu::handleJoin (const CEGUI::EventArgs &e)
{
	std::cout << "join pressed!\n";
	return true;
}

bool MainMenu::handleQuit (const CEGUI::EventArgs &e)
{
	std::cout << "quit pressed!\n";
	return true;
}

}
}
