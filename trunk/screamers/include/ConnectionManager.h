/*
 * ConnectionManager.h - manages client connections in the server
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
#include <tnlEventConnection.h>
#include <tnlNetInterface.h>
#include <tnlRPC.h>

#include "MainLoop.h"
#include "Singleton.h"
#include "GameConnection.h"

#ifndef _CONNECTION_MANAGER_H_
#define _CONNECTION_MANAGER_H_

namespace Server
{

class ConnectionManager : public MainListener, public Singleton<ConnectionManager>
{
public:
					 ConnectionManager ();
					~ConnectionManager ();

	virtual void			 tick (void);

	void				 addConnection (GameConnection *connection);

private:
	TNL::RefPtr<TNL::NetInterface>	 network_interface;

	std::vector<GameConnection *>	 connections;

	friend class Singleton<ConnectionManager>;
};

};

#endif // _CONNECTION_MANAGER_H_
