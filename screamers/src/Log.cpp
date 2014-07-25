/*
 * Log.cpp - logging classes
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
#include "Log.h"

Log::Log (const char *filename) : TNL::LogConsumer ()
{
	log_file.open (filename);
	if (!log_file)
		std::cerr << "warning: could not open log file, printing log to stdout\n";

	// hmm. would be nice to have some kind of log scoping here
	// These are *far* too verbose
	// TNLLogEnable(LogConnectionProtocol, true);
	// TNLLogEnable(LogNetConnection,      true);
	TNLLogEnable(LogEventConnection,    true);
	TNLLogEnable(LogGhostConnection,    true);
	TNLLogEnable(LogNetInterface,       true);
	TNLLogEnable(LogPlatform,           true);
}

Log::~Log ()
{
	if (log_file)
		log_file.close ();
}

void Log::logString (const char *string)
{
	if (log_file)
		log_file << string << std::endl;
	else
		std::cout << string << std::endl;
}


ClientLog::ClientLog () : ::Log ("screamers.log"), Ogre::LogListener ()
{
	Ogre::LogManager *manager;
	if (Ogre::LogManager::getSingletonPtr () == NULL)
		manager = new Ogre::LogManager ();
	else
		manager = Ogre::LogManager::getSingletonPtr ();
	manager->addListener (this);
	manager->setLogDetail (Ogre::LL_BOREME);
}

ClientLog::~ClientLog ()
{
}

void ClientLog::write (const Ogre::String &name, const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug)
{
	if (log_file)
		log_file << message << std::endl;
	else
		std::cout << message << std::endl;
}

