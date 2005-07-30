/*
 * ConnectionManager.cpp - manages client connections in the server
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
#include "ConnectionManager.h"

template<> ConnectionManager *Ogre::Singleton<ConnectionManager>::ms_Singleton = 0;
ConnectionManager &ConnectionManager::getSingleton (void)
{
	assert (ms_Singleton);
	return (*ms_Singleton);
}

ConnectionManager *ConnectionManager::getSingletonPtr (void)
{
	return ms_Singleton;
}

ConnectionManager::ConnectionManager (void)
{
	TNL::Address address (TNL::IPProtocol, TNL::Address::Any, 27050);
	network_interface = new TNL::NetInterface (address);
	network_interface->setAllowsConnections (true);

	MainLoop::getSingleton ().addListener (this);
}

ConnectionManager::~ConnectionManager (void)
{
}

void ConnectionManager::tick (void)
{
	network_interface->checkIncomingPackets ();
	network_interface->processConnections ();
}

void ConnectionManager::addConnection (GameConnection *connection)
{
	std::cout << "adding a connection\n";
	connections.push_back (connection);
}

void ConnectionManager::removeConnection (GameConnection *connection)
{
	std::cout << "removing a connection\n";
	for (std::vector<GameConnection *>::iterator it = connections.begin (); it != connections.end (); it++) {
		if (*it == connection) {
			connections.erase (it);
			break;
		}
	}
}
