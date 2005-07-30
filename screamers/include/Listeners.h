/*
 * Listeners.h - screamers-specific FrameListeners
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

#include <Ogre.h>
#include "MainLoop.h"

#ifndef _LISTENERS_H_
#define _LISTENERS_H_

class FrameListener : public Ogre::FrameListener
{
public:
				 FrameListener (Ogre::RenderWindow *window);
	virtual			~FrameListener ();

protected:
	Ogre::RenderWindow	*window;
};

class InfoListener : public TimeoutListener
{
public:
				 InfoListener (Ogre::RenderWindow *window);
	virtual			~InfoListener ();

	virtual bool		 timeout ();

private:
	Ogre::Overlay		*debugOverlay;
	Ogre::OverlayElement	*gui_current_fhz;
	Ogre::OverlayElement	*gui_average_fhz;
	Ogre::OverlayElement	*gui_best_fhz;
	Ogre::OverlayElement	*gui_worst_fhz;
	Ogre::OverlayElement	*gui_triangle_count;
	Ogre::RenderWindow	*window;

	void			 updateStats (void);
};

class MainLoopTrigger : public FrameListener
{
public:
				 MainLoopTrigger ();
				~MainLoopTrigger ();

	virtual bool		 frameEnded (const Ogre::FrameEvent &event);
};

#endif /* _LISTENERS_H_ */
