/*
 * application.h - application class which generally runs everything
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
#include "frame-listener.h"

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

namespace Screamers
{

class Application
{
	// This is mostly taken from the "ExampleApplication" class in the
	// Ogre samples, since it's a pretty nice way to set up everything
	// we need.

public:
				 Application ();
	virtual			~Application ();

	virtual void		 go ();

protected:
	Ogre::Root		*root;
	Ogre::Camera		*camera;
	Ogre::SceneManager	*sceneManager;
	Ogre::RenderWindow	*renderWindow;
	InfoListener		*infoListener;

	virtual bool		 setup ();
	virtual bool		 configure ();

private:
	void			 loadResourcePaths ();
	void			 loadResources ();
};

};

#endif
