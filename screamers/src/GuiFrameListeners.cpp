/*
 * GuiFrameListeners.cpp - various GUI screens
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

#include "GuiFrameListeners.h"

namespace Screamers
{
namespace Client
{

GuiFrameListener::GuiFrameListener (Ogre::RenderWindow *window, CEGUI::Renderer *renderer) : FrameListener (window)
{
	event_processor = new Ogre::EventProcessor ();
	event_processor->initialise (window);
	event_processor->startProcessingEvents ();
	event_processor->addKeyListener (this);
	event_processor->addMouseMotionListener (this);
	event_processor->addMouseListener (this);

	gui_renderer = renderer;
}

bool GuiFrameListener::frameEnded (const Ogre::FrameEvent &event)
{
	// FIXME - we should return false on some key here, like ESC
	return FrameListener::frameEnded (event);
}

void GuiFrameListener::mouseMoved (Ogre::MouseEvent *event)
{
	CEGUI::System::getSingleton ().injectMouseMove (event->getRelX () * gui_renderer->getWidth (),
							event->getRelY () * gui_renderer->getHeight ());
}

void GuiFrameListener::mouseDragged (Ogre::MouseEvent *event)
{
}

void GuiFrameListener::mousePressed (Ogre::MouseEvent *event)
{
}

void GuiFrameListener::mouseReleased (Ogre::MouseEvent *event)
{
}

void GuiFrameListener::keyPressed (Ogre::KeyEvent *event)
{
}

void GuiFrameListener::keyReleased (Ogre::KeyEvent *event)
{
}

void GuiFrameListener::keyClicked (Ogre::KeyEvent *event)
{
}

};
};
