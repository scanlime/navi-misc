/* Firestarter
* firestarterd.cpp :
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

#include "firestarterd.h"
#include "commandargs.h"
#include "prefs.h"
#include "events.h"
#include "timer.h"

trServerInfo		serverInfo;
CListServerServerConnection	serverListServerConnection;

float	lastListServerUpdate;
float	listServerUpdateTime = 120;

bool registerAsPublic = true;

bool getServerInfo ( void )
{
	CCommandLineArgs	&args = CCommandLineArgs::instance();
	CPrefsManager &prefs = CPrefsManager::instance();

	// get the name of the server
	if (args.Exists("name"))
		serverInfo.name = args.GetDataS("name");
	else if (prefs.ItemExists("servername"))
		serverInfo.name = prefs.GetItemS("servername");
	else
		serverInfo.name = "some_unknown_server";

	// get the public address
	if (args.Exists("address"))
		serverInfo.address = args.GetDataS("address");
	else if (prefs.ItemExists("serveraddress"))
		serverInfo.address = prefs.GetItemS("servernaddress");
	else
		serverInfo.address = "localhost";

	// get the game type
	if (args.Exists("game"))
		serverInfo.game = args.GetDataS("game");
	else if (prefs.ItemExists("game"))
		serverInfo.game = prefs.GetItemS("game");
	else
		serverInfo.game = "default";

	// get the OS
#ifdef _WIN32
		serverInfo.os = "Windows";
#else
		serverInfo.os = "linux";
#endif

	// get the port
	if (args.Exists("port"))
		serverInfo.port = args.GetDataI("port");
	else if (prefs.ItemExists("port"))
		serverInfo.port = prefs.GetItemI("port");
	else
		serverInfo.port = 2501;

	serverInfo.version = 1.0f;
	serverInfo.currentPlayers = 0;

	// get max players
	if (args.Exists("maxplayers"))
		serverInfo.maxPlayers = args.GetDataI("maxplayers");
	else if (prefs.ItemExists("maxplayers"))
		serverInfo.maxPlayers = prefs.GetItemI("maxplayers");
	else
		serverInfo.maxPlayers = 32;

	serverInfo.token = -1;

	if (args.Exists("listupdatetime"))
		listServerUpdateTime = args.GetDataF("listupdatetime");
	else if (prefs.ItemExists("listupdatetime"))
		listServerUpdateTime = prefs.GetItemF("listupdatetime");

	return true;
}

int errorOut ( const char * error, const char* place, int ret )
{
	printf("Fatal Error: %s at %s",error,place);
	return ret;
}

bool updateListServ ( void )
{
	if (!registerAsPublic)
		return false;
	
	if (lastListServerUpdate + listServerUpdateTime < CTimer::instance().GetTime())
	{
		if (!serverListServerConnection.update(serverInfo))
			return errorOut("list server connection error","serverListServerConnection.Update(serverInfo)") != 0;

		lastListServerUpdate = (float)CTimer::instance().GetTime();
	}
	return false;
}

int main (int argc, char **argv)
{
	CTimer::instance().Init();

	CCommandLineArgs	&args = CCommandLineArgs::instance();
	args.Set(argc,argv);

	if (args.Exists("prefs"))
		CPrefsManager::instance().Init((char*)args.GetDataS("prefs"));
	else
		CPrefsManager::instance().Init("firestarterd");

	registerAsPublic = args.GetDataB("public");

	if (!registerAsPublic)
		registerAsPublic = CPrefsManager::instance().ItemExists("public");

	lastListServerUpdate = (float)CTimer::instance().GetTime();

	getServerInfo();

	if (registerAsPublic)
	{
		if (!serverListServerConnection.add(serverInfo))
			return errorOut("list server connection error","serverListServerConnection.Add(serverInfo)");
	}

	bool	done = false;
	while (!done)
	{
		CTimer::instance().Update();
		//done = eventCycle();
		if (done)
			done = updateListServ();
		OSSleep(0.001f);
	}

	if (registerAsPublic)
	{
		if (!serverListServerConnection.remove(serverInfo))
			return errorOut("list server connection error","serverListServerConnection.Add(serverInfo)");
	}
	return 0;
}