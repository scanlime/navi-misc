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
#include "serverGameManager.h"
#include "serverlistener.h"

trServerInfo		serverInfo;
CListServerServerConnection	serverListServerConnection;

CServerListener		serverListener;

float	lastListServerUpdate;
float	listServerUpdateTime = 120;

bool registerAsPublic = true;
eLogLevel	eCurrentLogLevel = eLogLevel1;

bool eventCycle ( void );

//logings
int errorOut ( const char * error, const char* place, int ret )
{
	printf("Fatal Error: %s at %s\n",error,place);
	return ret;
}

void logOut ( const char * error, const char* place, eLogLevel level )
{
	if (level <= eCurrentLogLevel )
		printf("log: %s at %s\n",error,place);
}

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
		serverInfo.game = "susan";

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

bool updateListServ ( void )
{
	if (!registerAsPublic)
		return false;
	
	if (lastListServerUpdate + listServerUpdateTime < CTimer::instance().GetTime())
	{
		logOut("list server update sent","updateListServ",eLogLevel1);

		serverInfo.currentPlayers = serverListener.getCurrentPlayers();

		if (!serverListServerConnection.update(serverInfo))
			return errorOut("list server connection error","serverListServerConnection.Update(serverInfo)") != 0;

		logOut("list server update complete","updateListServ",eLogLevel2);

		lastListServerUpdate = (float)CTimer::instance().GetTime();
	}
	return false;
}

int main (int argc, char **argv)
{
	logOut("Firestarterd: starting fires","main",eLogLevel1);
	CTimer::instance().Init();
	CServerGameManger::instance().init();

	CCommandLineArgs	&args = CCommandLineArgs::instance();
	args.Set(argc,argv);

	if (args.Exists("prefs"))
		CPrefsManager::instance().Init((char*)args.GetDataS("prefs"));
	else
		CPrefsManager::instance().Init("firestarterd");

	logOut("Prefs loaded","main",eLogLevel2);

	// randdom generateor
	if (args.Exists("srand"))
		srand(args.GetDataI("srand"));
	else
	{
#ifdef _WIN32
		srand(GetTickCount());
#else
		srand(time(NULL));
#endif
	}

	if (args.Exists("loglevel"))
		eCurrentLogLevel = (eLogLevel)args.GetDataI("loglevel");

	registerAsPublic = args.GetDataB("public");

	if (!registerAsPublic)
		registerAsPublic = CPrefsManager::instance().ItemExists("public");

	lastListServerUpdate = (float)CTimer::instance().GetTime();

	getServerInfo();

	logOut("setings loaded","main",eLogLevel2);

	std::string temp1= "loaded game: " + serverInfo.game;
	logOut(temp1.c_str(),"main",eLogLevel3);

	if (!serverListener.init(serverInfo.game.c_str(),serverInfo.port,serverInfo.maxPlayers))
		return errorOut("server listener init error","serverListener.init(serverInfo.game.c_str(),serverInfo.port)");

	if (registerAsPublic)
	{
		logOut("Contacting list server","main",eLogLevel2);
		if (!serverListServerConnection.add(serverInfo))
			return errorOut("list server connection error","serverListServerConnection.Add(serverInfo)");
		logOut("List server add processed","main",eLogLevel3);
	}
	bool	done = false;
	while (!done)
	{
		CTimer::instance().Update();
		done = eventCycle();
		if (!done)
			updateListServ();
		OSSleep(0.001f);
	}
	logOut("Server shutdown comencing","main",eLogLevel1);

	if (registerAsPublic)
	{
		logOut("list server remove sent","main",eLogLevel2);
		if (!serverListServerConnection.remove(serverInfo))
			return errorOut("list server connection error","serverListServerConnection.Add(serverInfo)");
		logOut("list server remove completed","main",eLogLevel3);
	}

	serverListener.kill();
	return 0;
}

bool eventCycle ( void )
{
	return serverListener.update();
}