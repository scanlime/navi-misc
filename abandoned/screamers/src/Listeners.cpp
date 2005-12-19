/*
 * Listeners.cpp - screamers-specific FrameListeners
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

#include "Listeners.h"
#include "MainLoop.h"


FrameListener::FrameListener (Ogre::RenderWindow *window)
{
	this->window = window;
}

FrameListener::~FrameListener ()
{
}

InfoListener::InfoListener (Ogre::RenderWindow *window) : TimeoutListener ()
{
	Ogre::OverlayManager &overlay_manager = Ogre::OverlayManager::getSingleton ();
	this->window = window;

	debugOverlay = overlay_manager.getByName ("Core/DebugOverlay");

	debugOverlay->show ();

	gui_current_fhz    = overlay_manager.getOverlayElement ("Core/CurrFps");
	gui_average_fhz    = overlay_manager.getOverlayElement ("Core/AverageFps");
	gui_best_fhz       = overlay_manager.getOverlayElement ("Core/BestFps");
	gui_worst_fhz      = overlay_manager.getOverlayElement ("Core/WorstFps");
	gui_triangle_count = overlay_manager.getOverlayElement ("Core/NumTris");

	MainLoop::getSingleton ().addTimeout (this, 1000);
}

InfoListener::~InfoListener ()
{
}

bool InfoListener::timeout ()
{
	updateStats ();
	return true;
}

void InfoListener::updateStats (void)
{
	// if the overlay is hidden, don't update the labels
	if (!debugOverlay->isVisible ())
		return;

	// I'm not sure why this is in a try/catch block - maybe something with FrameStats
	try
	{
		const Ogre::RenderTarget::FrameStats &stats = window->getStatistics ();

		gui_current_fhz->setCaption    (Ogre::StringConverter::toString (stats.lastFPS));
		gui_average_fhz->setCaption    (Ogre::StringConverter::toString (stats.avgFPS));
		gui_best_fhz->setCaption       (Ogre::StringConverter::toString (stats.bestFPS));
		gui_worst_fhz->setCaption      (Ogre::StringConverter::toString (stats.worstFPS));
		gui_triangle_count->setCaption (Ogre::StringConverter::toString (stats.triangleCount));
	}
	catch (...)
	{
		// ignore
	}
}

MainLoopTrigger::MainLoopTrigger () : FrameListener (NULL)
{
}

MainLoopTrigger::~MainLoopTrigger ()
{
}

bool MainLoopTrigger::frameEnded (const Ogre::FrameEvent &event)
{
	MainLoop::getSingleton ().iteration ();
	return true;
}

