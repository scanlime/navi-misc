/* Firestarter
* serverListener.cpp :
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

#include "serverListener.h"
#include "firestarterd.h"
#include "commandargs.h"

CServerListener::CServerListener()
{
	InitNetwork();
	lastID = 0;
	game = NULL;;
}

CServerListener::~CServerListener()
{
	FreeNetwork();
	kill();
}

bool CServerListener::init ( const char* gameType, int port, int maxListens )
{
	game = CServerGameManger::instance().get(gameType);

	if (!game)
		return false;

	game->init();

	server.SetMesageHandaler(this);
	server.Start(maxListens,-1,-1,port);

	// add some bots
	CCommandLineArgs	&args = CCommandLineArgs::instance();
	if (args.Exists("robots"))
	{
		int bots = args.GetDataI("robots");
		for (int i = 0; i < bots; i++ )
		{
			char temp[512];
			sprintf(temp,"robot-%d",i);
			
			game->addBot(lastID++,temp,"default");
		}
	}
	return game != NULL;
}

int CServerListener::getCurrentPlayers ( void )
{
	return (int)users.size();
}

bool CServerListener::update ( void )
{
	server.ProcessMessages();
	if (!game)
		return true;

	if (game->think())
		return true;
	return false;
}

void CServerListener::kill ( void )
{
	if (game)
		game->kill();
	server.Stop();
}

void CServerListener::OnConnect ( CNetworkPeer &peer )
{
	tmNetUserMap::iterator itr = users.find(&peer);
	if (itr != users.end())	// we all ready have them and they have and ID
		return;

	users[&peer] = lastID++;
	if (!game)
		return;

	game->add(lastID-1,peer);

	logOut("client connect","CServerListener::OnConnect",eLogLevel5);
}

void CServerListener::OnDisconnect ( CNetworkPeer &peer )
{
	tmNetUserMap::iterator itr = users.find(&peer);
	if (itr == users.end())	// we don't know who he is
		return;

	if (!game)
		return;

	game->remove(itr->second,peer);

	logOut("client disconnect","CServerListener::OnDisconnect",eLogLevel5);
}

void CServerListener::OnMessage ( CNetworkPeer &peer, CNetworkMessage &message )
{
	tmNetUserMap::iterator itr = users.find(&peer);
	if (itr == users.end())	// we don't know who he is
		return;

	if (!game)
		return;

	game->message(itr->second,peer,message);

	logOut("client message","CServerListener::OnMessage",eLogLevel5);

}