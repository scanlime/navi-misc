/*
 * Gui.h - various GUI screens
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

#include <map>
#include <stack>
#include <Ogre.h>
#include <OgreKeyEvent.h>
#include <OgreEventListeners.h>
#include <OgreRenderWindow.h>
#include <OgreCEGUIRenderer.h>
#include <CEGUI/CEGUISystem.h>
#include "FrameListener.h"
#include "Application.h"

#ifndef _GUI_FRAME_LISTENER_H_
#define _GUI_FRAME_LISTENER_H_

namespace Screamers
{
namespace Client
{

class GuiFrameListener : public FrameListener,
			 public Ogre::KeyListener,
			 public Ogre::MouseMotionListener,
			 public Ogre::MouseListener
{
public:
				 GuiFrameListener (Ogre::RenderWindow *window,
						   CEGUI::Renderer    *renderer);

	bool			 frameEnded       (const Ogre::FrameEvent &event);
	void			 mouseMoved       (Ogre::MouseEvent *event);
	void			 mouseDragged     (Ogre::MouseEvent *event);
	void			 mousePressed     (Ogre::MouseEvent *event);
	void			 mouseReleased    (Ogre::MouseEvent *event);
	void			 mouseClicked     (Ogre::MouseEvent *event);
	void			 mouseEntered     (Ogre::MouseEvent *event);
	void			 mouseExited      (Ogre::MouseEvent *event);
	void			 keyPressed       (Ogre::KeyEvent *event);
	void			 keyReleased      (Ogre::KeyEvent *event);
	void			 keyClicked       (Ogre::KeyEvent *event);
private:
	CEGUI::Renderer		*gui_renderer;
	Ogre::EventProcessor	*event_processor;
	bool			 quit;
};

class GuiPage
{
public:
			 GuiPage ();
			~GuiPage ();
private:
	CEGUI::Window	*sheet;

	friend class Gui;
};

class Gui
{
public:
					 Gui      ();
					~Gui      ();

	void				 addPage  (std::string name, GuiPage *page);
	void				 pushPage (std::string name);
	void				 popPage  (void);

private:
	std::map<std::string, GuiPage*>	 pages;
	std::stack<GuiPage *>		 display;

	CEGUI::OgreCEGUIRenderer	*gui_renderer;
	CEGUI::System			*gui_system;
	GuiFrameListener		*frame_listener;
};

};
};

#endif
