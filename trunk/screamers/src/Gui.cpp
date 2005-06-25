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

#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUISchemeManager.h>
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

Gui::Gui ()
{
	Application &application = Application::getSingleton ();

	gui_renderer = new CEGUI::OgreCEGUIRenderer(application.getRenderWindow (), Ogre::RENDER_QUEUE_OVERLAY, false, 0, Ogre::ST_GENERIC);
	gui_system = new CEGUI::System (gui_renderer);

	// Most of the rest of the code here is copied from the ogre samples.
	// If you can't figure it out, check that code and the tutorial
	// on the ogre wiki, since this stuff is pretty ugly.

	// create render-to-texture setup
	Ogre::RenderTexture *rtt = application.getRoot ()->getRenderSystem ()->createRenderTexture ("RttTexture", 512, 512, Ogre::TEX_TYPE_2D, Ogre::PF_R8G8B8);
	{
		Ogre::SceneManager *scene_manager = application.getSceneManager ();
		Ogre::Camera *camera = scene_manager->createCamera ("RttCamera");
		Ogre::SceneNode *camera_node = scene_manager->getRootSceneNode ()->createChildSceneNode ("RttCameraNode");
		camera_node->attachObject (camera);
		camera->setPosition (0, 0, 200);

		Ogre::Viewport *v = rtt->addViewport (camera);
		v->setOverlaysEnabled (false);
		v->setClearEveryFrame (true);
		v->setBackgroundColour (Ogre::ColourValue::Black);
	}

	// Retrieve CEGUI texture for the RTT
	CEGUI::Texture *rtt_texture = gui_renderer->createTexture ((CEGUI::utf8*) "RttTexture");
	CEGUI::Imageset *rtt_imageset = CEGUI::ImagesetManager::getSingleton ().createImageset ((CEGUI::utf8*) "RttImageset", rtt_texture);

	rtt_imageset->defineImage ((CEGUI::utf8*) "RttImage",
				   CEGUI::Point (0.0f, 0.0f),
				   CEGUI::Size (rtt_texture->getWidth (), rtt_texture->getHeight ()),
				   CEGUI::Point (0.0f, 0.0f));

	// Load scheme and set defaults
	CEGUI::SchemeManager::getSingleton ().loadScheme ((CEGUI::utf8*) "TaharezLook.scheme");
	gui_system->setDefaultMouseCursor ((CEGUI::utf8*) "TaharezLook", (CEGUI::utf8*) "MouseArrow");
	gui_system->setDefaultFont ((CEGUI::utf8*) "Bitstream Vera Sans-12");
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
