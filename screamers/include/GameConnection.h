/*
 * GameConnection.h - manages a connection between client and server
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

#include <tnl.h>
#include <tnlGhostConnection.h>
#include <tnlNetConnection.h>
#include <tnlNetInterface.h>
#include "MainLoop.h"

#ifndef _GAME_CONNECTION_H_
#define _GAME_CONNECTION_H_

static char *connection_states[] = {
	// these map to the constants in TNL::NetConnectionState
	"Not connected.",
	"Sending challenge request...",
	"Punching through firewalls...",
	"Computing puzzle solution...",
	"Connection request sent.",
	"Connection timed out.",
	"Connected.",
	"Disconnected.",
	"Connection timed out.",
	"",
};

class GameConnection : public TNL::GhostConnection, public TimeoutListener
{
public:
	TNL_DECLARE_NETCONNECTION(GameConnection);

				 GameConnection ();
	virtual			~GameConnection ();

	void			 connectToServer (const char *server, int port);

	bool			 isDataToTransmit ();
	void			 onConnectTerminated (TNL::NetConnection::TerminationReason reason, const char *rejectionString);
	void			 onConnectionTerminated (TNL::NetConnection::TerminationReason reason, const char *string);
	void			 onConnectionEstablished ();

	bool			 timeout (void);

private:
	TNL::Address		*remote_address;
	TNL::NetInterface	*_interface;
};

#endif // _GAME_CONNECTION_H_
