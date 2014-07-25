/*
 * Screamers.cpp - main entry point for the client
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

#include <vector>
#include <string>
#include <iostream>

#include <tnl.h>
#include <tnlNetInterface.h>
#include <tnlLog.h>

#include "Application.h"
#include "GameConnection.h"
#include "Log.h"
#include "Screamers.h"
#include "TextUtils.h"
#include "Version.h"

int appMain (std::vector<std::string> options)
{
	WindowedApplication app;

	// We need to set up OGRE and all that jazz before we can initialize anything else
	if (!app.setup ())
		return 1;

	std::cout << "version " << VERSION << std::endl;

	ClientLog log;

	std::string server = "localhost";
	if (options.size () == 1)
		server = options[0];

	// connect to the server specified
	GameConnection connection;
	MainLoop::getSingleton ().addTimeout (&connection, 20);

	connection.connectToServer (server.c_str (), 27050);

	app.main ();
	return 0;
}

// platform-specific main functions, 'cuz one of those platforms uses a silly one
#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	std::string cli  = lpCmdLine;
	std::vector<std::string> options = TextUtils::tokenize (cli, " \t", 0, true);
	return appMain(options);
}
#else
int main (int argc, char *argv[])
{
	std::vector<std::string> options;
	for (int i = 1; i < argc; i++)
		options.push_back (std::string (argv[i]));

	return appMain (options);
}
#endif
