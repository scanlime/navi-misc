/*
 * Gui.cpp - various GUI screens
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

#include "Gui.h"

namespace Screamers
{
namespace Client
{

static CEGUI::MouseButton convertOgreButtonToCegui (int button)
{
	switch (button) {
	case Ogre::MouseEvent::BUTTON0_MASK:	return CEGUI::LeftButton;
	case Ogre::MouseEvent::BUTTON1_MASK:	return CEGUI::RightButton;
	case Ogre::MouseEvent::BUTTON2_MASK:	return CEGUI::MiddleButton;
	case Ogre::MouseEvent::BUTTON3_MASK:	return CEGUI::X1Button;
	default:				return CEGUI::LeftButton;
	}
}

GuiFrameListener::GuiFrameListener (Ogre::RenderWindow *window, CEGUI::Renderer *renderer) : FrameListener (window)
{
	event_processor = new Ogre::EventProcessor ();
	event_processor->initialise (window);
	event_processor->startProcessingEvents ();
	event_processor->addKeyListener (this);
	event_processor->addMouseMotionListener (this);
	event_processor->addMouseListener (this);

	gui_renderer = renderer;

	quit = false;
}

bool GuiFrameListener::frameEnded (const Ogre::FrameEvent &event)
{
	if (quit)
		return false;
	return FrameListener::frameEnded (event);
}

void GuiFrameListener::mouseMoved (Ogre::MouseEvent *event)
{
	CEGUI::System::getSingleton ().injectMouseMove (event->getRelX () * gui_renderer->getWidth (),
							event->getRelY () * gui_renderer->getHeight ());
}

void GuiFrameListener::mouseDragged (Ogre::MouseEvent *event)
{
	mouseMoved (event);
}

void GuiFrameListener::mousePressed (Ogre::MouseEvent *event)
{
	CEGUI::System::getSingleton ().injectMouseButtonDown (convertOgreButtonToCegui (event->getButtonID ()));
}

void GuiFrameListener::mouseReleased (Ogre::MouseEvent *event)
{
	CEGUI::System::getSingleton ().injectMouseButtonUp (convertOgreButtonToCegui (event->getButtonID ()));
}

void GuiFrameListener::keyPressed (Ogre::KeyEvent *event)
{
	// if the user hit escape, quit. if not, punt back into CEGUI
	if (event->getKey () == Ogre::KC_ESCAPE) {
		quit = true;
	} else {
		CEGUI::System::getSingleton ().injectKeyDown (event->getKey ());
		CEGUI::System::getSingleton ().injectChar (event->getKeyChar ());
	}
	event->consume ();
}

void GuiFrameListener::keyReleased (Ogre::KeyEvent *event)
{
	CEGUI::System::getSingleton ().injectKeyUp (event->getKey ());
	event->consume ();
}

void GuiFrameListener::keyClicked (Ogre::KeyEvent *event)
{
	event->consume ();
}

Gui::Gui (Ogre::RenderWindow *window)
{
	this->window = window;
	gui_renderer = new CEGUI::OgreCEGUIRenderer(window, Ogre::RENDER_QUEUE_OVERLAY, false, 0, Ogre::ST_GENERIC);
	gui_system = new CEGUI::System (gui_renderer);
}

Gui::~Gui ()
{
	if (gui_system) {
		delete gui_system;
		gui_system = NULL;
	}
	if (gui_renderer) {
		delete gui_renderer;
		gui_renderer = NULL;
	}
}

};
};
