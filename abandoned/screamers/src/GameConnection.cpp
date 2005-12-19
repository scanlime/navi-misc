/*
 * GameConnection.cpp - manages a connection between client and server
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

#include <iostream>

#include "GameConnection.h"
#include "ConnectionManager.h"
#include "TextUtils.h"

TNL_IMPLEMENT_NETCONNECTION(GameConnection, TNL::NetClassGroupGame, true);

GameConnection::GameConnection () :
	TNL::GhostConnection (),
	TimeoutListener ()
{
	setIsAdaptive ();

}

GameConnection::~GameConnection ()
{
}

void GameConnection::connectToServer (const char *server, int port)
{
	setIsConnectionToServer ();

	// connect
	remote_address = new TNL::Address (TextUtils::format ("ip:%s:%d", server, port).c_str ());
	TNL::Address local_address (TNL::IPProtocol, TNL::Address::Any, 0);
	_interface = new TNL::NetInterface (local_address);
	connect (_interface, *remote_address);
}

bool GameConnection::isDataToTransmit ()
{
	return true;
}

void GameConnection::onConnectTerminated (TNL::NetConnection::TerminationReason reason, const char *rejectionString)
{
	TNL::logprintf ("%s - %s connect terminated: %d", getNetAddressString (), isConnectionToServer () ? "server" : "client", rejectionString);
}

void GameConnection::onConnectionTerminated (TNL::NetConnection::TerminationReason reason, const char *string)
{
	TNL::logprintf ("%s - %s connection terminated: %d", getNetAddressString (), isConnectionToServer () ? "server" : "client", reason);
	if (!isInitiator ())
		ConnectionManager::getSingleton ().removeConnection (this);
}

void GameConnection::onConnectionEstablished ()
{
	TNL::logprintf ("connected to server");
	if (isInitiator ()) {
		setGhostFrom (false);
		setGhostTo (true);
		activateGhosting ();
	} else {
		setGhostFrom (true);
		setGhostTo (false);
		activateGhosting ();
		ConnectionManager::getSingleton ().addConnection (this);
	}
}

bool GameConnection::timeout ()
{
	_interface->checkIncomingPackets ();
	_interface->processConnections ();
	return true;
}
