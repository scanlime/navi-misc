/*
 * MainLoop.h - manages a main loop
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

#include <vector>

#ifndef _MAIN_LOOP_H_
#define _MAIN_LOOP_H_

namespace Screamers
{

class MainListener
{
public:
	virtual void tick (void) = 0;
};

// FIXME - should this be a singleton?
class MainLoop
{
public:
			 MainLoop ();
			~MainLoop ();

	void		 go (void);
	void		 addListener (MainListener *listener);

	unsigned int	 getDelay (void);
	void		 setDelay (unsigned int ms);

private:
	std::vector<MainListener*>	listeners;

	// FIXME - do we want some kind of priority/negotiation thing here?
	unsigned int			delay;
};

};

#endif // _MAIN_LOOP_H_
