/*
 * Log.h - logging classes
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
#include <tnlLog.h>
#include <fstream>
#include <Ogre.h>

#ifndef _LOG_H_
#define _LOG_H_

class Log : public TNL::LogConsumer
{
public:
			 Log (const char *filename);
			~Log ();

	virtual void	 logString (const char *string);

protected:
	std::ofstream	 log_file;
};

class ClientLog : public ::Log, public Ogre::LogListener
{
public:
			 ClientLog ();
			~ClientLog ();

	virtual void	 write (const Ogre::String &name,
				const Ogre::String &message,
				Ogre::LogMessageLevel lml = Ogre::LML_NORMAL,
				bool maskDebug = false);
};


#endif // _LOG_H_
