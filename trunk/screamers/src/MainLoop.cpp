/*
 * MainLoop.cpp - manages a main loop
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
#include <tnlPlatform.h>

#include "MainLoop.h"

template <>
MainLoop* Singleton<MainLoop>::_instance = NULL;

MainLoop::MainLoop ()
{
	delay = 10;
}

MainLoop::~MainLoop ()
{
}

void MainLoop::go (void)
{
	while (1) {
		for (std::vector<MainListener*>::iterator it = listeners.begin (); it != listeners.end (); it++)
			(*it)->tick ();
		TNL::Platform::sleep (delay);
	}
}

void MainLoop::iteration (void)
{
	for (std::vector<MainListener*>::iterator it = listeners.begin (); it != listeners.end (); it++)
		(*it)->tick ();
}

void MainLoop::addListener (MainListener *listener)
{
	listeners.push_back (listener);
}

unsigned int MainLoop::getDelay (void)
{
	return delay;
}

void MainLoop::setDelay (unsigned int ms)
{
	delay = ms;
}
