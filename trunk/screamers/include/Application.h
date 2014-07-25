/*
 * Application.h - application classes which generally run everything
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
#include "Listeners.h"

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

class Application
{
public:
					 Application (void);
	virtual				~Application (void);


	virtual bool			 setup (void) = 0;
	void				 main (void);

protected:
	virtual void			 go (void) = 0;

	MainLoop			*main_loop;
	bool				 configured;
};

class HeadlessApplication : public Application, public Ogre::Singleton<HeadlessApplication>
{
public:
					 HeadlessApplication (void);
	virtual				~HeadlessApplication (void);

	static HeadlessApplication	&getSingleton (void);
	static HeadlessApplication	*getSingletonPtr (void);

	virtual bool			 setup (void);
protected:
	virtual void			 go (void);
};

class WindowedApplication : public Application, public Ogre::Singleton<WindowedApplication>
{
public:
					 WindowedApplication (void);
	virtual				~WindowedApplication (void);

	Ogre::Root			*getRoot (void);
	Ogre::Camera			*getCamera (void);
	Ogre::SceneManager		*getSceneManager (void);
	Ogre::RenderWindow		*getRenderWindow (void);

	static WindowedApplication	&getSingleton (void);
	static WindowedApplication	*getSingletonPtr (void);

	virtual bool			 setup (void);
protected:
	virtual void			 go (void);
	bool				 configure (void);

	Ogre::Root			*root;
	Ogre::Camera			*camera;
	Ogre::SceneManager		*sceneManager;
	Ogre::RenderWindow		*renderWindow;
	InfoListener			*infoListener;
	MainLoopTrigger			*mainListener;

private:
	void				 loadResourcePaths (void);
	void				 loadResources (void);
};

#endif
