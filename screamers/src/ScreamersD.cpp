/*
 * ScreamersD.cpp - main entry point for the server
 *
 * Copyright (C) 2005 Screamers Group (see AUTHORS)
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

#include <string>
#include <iostream>

#include <tnl.h>
#include <tnlPlatform.h>

#include "Screamers.h"
#include "ConnectionManager.h"
#include "Log.h"
#include "MainLoop.h"
#include "Version.h"
#include "WorldParser.h"

int appMain (std::string commandLine)
{
	std::cout << "starting screamersd\nversion " << VERSION << std::endl;

	Log ("screamersd.log");

	World world;
	WorldParser parser (world);
	Server::ConnectionManager cm;

	parser.load (commandLine.c_str ());

	MainLoop::instance ().go ();

	return 0;
}

// platform-specific main functions, 'cuz one of those platforms uses a silly one
#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	std::string cli  = lpCmdLine;
	return appMain(cli);
}
#else
int main( int argc, char *argv[])
{
	// FIXME - this is bad! the shell does all of the globing for us, so we
	// really shouldn't do it on our own. makes more sense to use tokenize()
	// in the windows-specific function, since they just get one big string
	// (hahahahaha)
	std::string	cli;
	for ( int i = 1; i < argc; i++)
		cli += argv[i] + std::string(" ");

	return appMain(cli);
}
#endif
