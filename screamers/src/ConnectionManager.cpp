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

#include "ConnectionManager.h"

namespace Screamers
{
namespace Server
{

ConnectionManager::ConnectionManager ()
{
	TNL::Address address (TNL::IPProtocol, TNL::Address::Broadcast, 27050);
	network_interface = new TNL::NetInterface (address);
	network_interface->setAllowsConnections (true);
}

ConnectionManager::~ConnectionManager ()
{
}

void ConnectionManager::tick (void)
{
	network_interface->checkIncomingPackets ();
	network_interface->processConnections ();
}

};
};
