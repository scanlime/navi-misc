/*
 * frame-listener.cpp - screamers-specific FrameListeners
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

#include "frame-listener.h"

namespace Screamers
{

FrameListener::FrameListener (Ogre::RenderWindow *window)
{
	this->window = window;
}

FrameListener::~FrameListener ()
{
}

InfoListener::InfoListener (Ogre::RenderWindow *window) : FrameListener (window)
{
	debugOverlay = Ogre::OverlayManager::getSingleton ().getByName ("Core/DebugOverlay");
	debugOverlay->show ();
}

InfoListener::~InfoListener ()
{
}

bool
InfoListener::frameEnded (const Ogre::FrameEvent &event)
{
	updateStats ();
	return true;
}

void
InfoListener::updateStats ()
{
	// FIXME - it really doesn't make any sense to update this every frame
	try {
		// FIXME - should these be cached?
		Ogre::OverlayManager &overlay_manager = Ogre::OverlayManager::getSingleton ();
		Ogre::OverlayElement *gui_current_fhz    = overlay_manager.getOverlayElement ("Core/CurrFps");
		Ogre::OverlayElement *gui_average_fhz    = overlay_manager.getOverlayElement ("Core/AverageFps");
		Ogre::OverlayElement *gui_best_fhz       = overlay_manager.getOverlayElement ("Core/BestFps");
		Ogre::OverlayElement *gui_worst_fhz      = overlay_manager.getOverlayElement ("Core/WorstFps");
		Ogre::OverlayElement *gui_triangle_count = overlay_manager.getOverlayElement ("Core/NumTris");

		const Ogre::RenderTarget::FrameStats &stats = window->getStatistics ();

		// FIXME - do we want to show frame times as well?
		gui_current_fhz->setCaption    (Ogre::StringConverter::toString (stats.lastFPS));
		gui_average_fhz->setCaption    (Ogre::StringConverter::toString (stats.avgFPS));
		gui_best_fhz->setCaption       (Ogre::StringConverter::toString (stats.bestFPS));
		gui_worst_fhz->setCaption      (Ogre::StringConverter::toString (stats.worstFPS));
		gui_triangle_count->setCaption (Ogre::StringConverter::toString (stats.triangleCount));
	} catch (...) {
		// ignore
	}
}

};
