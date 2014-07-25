/* Firestarter
* osfile.h :
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

#ifndef _SERVER_LISTENER_H_
#define _SERVER_LISTENER_H_

#include "networking.h"
#include "serverGameManager.h"
#include <map>

typedef std::map<CNetworkPeer*,int> tmNetUserMap; 

class CServerListener : public CNetworkMessageProcessor
{
public:
	CServerListener();
	virtual ~CServerListener();

	virtual bool init ( const char* gameType, int port,int maxListens  );
	virtual bool update ( void );
	virtual void kill ( void );

	virtual	int getCurrentPlayers ( void );

	virtual void OnConnect ( CNetworkPeer &peer );
	virtual void OnDisconnect ( CNetworkPeer &peer );
	virtual void OnMessage ( CNetworkPeer &peer, CNetworkMessage &message );

protected:
	CNetworkServer	server;
	tmNetUserMap		users;
	int							lastID;

	CBaseServerGame	*game;
};

#endif //_SERVER_LISTENER_H_