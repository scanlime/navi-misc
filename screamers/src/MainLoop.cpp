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

#include "MainLoop.h"

template<> MainLoop *Ogre::Singleton<MainLoop>::ms_Singleton = 0;
MainLoop &MainLoop::getSingleton (void)
{
	assert (ms_Singleton);
	return (*ms_Singleton);
}

MainLoop *MainLoop::getSingletonPtr (void)
{
	return ms_Singleton;
}

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
		iteration ();
		TNL::Platform::sleep (delay);
	}
}

void MainLoop::iteration (void)
{
	// Call normal listeners
	for (std::vector<MainListener*>::iterator it = listeners.begin (); it != listeners.end (); it++)
		(*it)->tick ();

	// Call any timeouts that are registered.
	TNL::U32 current = TNL::Platform::getRealMilliseconds ();
	std::vector<std::vector<Timeout*>::iterator> remove_list;

	for (std::vector<Timeout*>::iterator it = timeouts.begin (); it != timeouts.end (); it++) {
		Timeout *to = *it;

		// getRealMilliseconds will wrap after about 7 days, so check for that
		if (to->last > current) {
			// Keep in mind that I haven't tested this, since I don't feel the need
			// to run useless programs for weeks at a time right now
			TNL::U32 total_time = current + (((TNL::U32)0) - to->last);
			if (total_time >= to->ms) {
				to->last = current;
				bool keep = to->listener->timeout ();
				if (!keep)
					remove_list.push_back (it);
			}
		} else {
			if ((current - to->last) >= to->ms) {
				to->last = current;
				bool keep = to->listener->timeout ();
				if (!keep)
					remove_list.push_back (it);
			}
		}
	}

	// Remove listeners which want themselves removed
	for (std::vector<std::vector<Timeout*>::iterator>::iterator it = remove_list.begin (); it != remove_list.end (); it++) {
		std::vector<Timeout*>::iterator it2 = *it;
		timeouts.erase (it2);
	}
}

void MainLoop::addListener (MainListener *listener)
{
	listeners.push_back (listener);
}

void MainLoop::addTimeout (TimeoutListener *listener, int ms)
{
	Timeout *timeout = new Timeout;
	timeout->listener = listener;
	timeout->ms = ms;
	timeout->last = TNL::Platform::getRealMilliseconds ();
	timeouts.push_back (timeout);
}

unsigned int MainLoop::getDelay (void)
{
	return delay;
}

void MainLoop::setDelay (unsigned int ms)
{
	delay = ms;
}
