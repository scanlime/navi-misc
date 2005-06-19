/*
 * application.cpp - application class which generally runs everything
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

#include "application.h"

namespace Screamers
{

Application::Application ()
{
	root          = NULL;
}

Application::~Application ()
{
	if (root)
		delete root;
}

void
Application::go ()
{
	if (!setup ())
		return;

	root->startRendering ();

	// FIXME - clean up
}

void
Application::loadResourcePaths ()
{
	Ogre::ConfigFile resources_config;
	resources_config.load ("resources.cfg");

	Ogre::ConfigFile::SectionIterator it = resources_config.getSectionIterator ();

	Ogre::String section_name, type_name, arch_name;

	while (it.hasMoreElements ()) {
		section_name = it.peekNextKey ();
		Ogre::ConfigFile::SettingsMultiMap *settings = it.getNext ();
		Ogre::ConfigFile::SettingsMultiMap::iterator sec_it;
		for (sec_it = settings->begin (); sec_it != settings->end (); ++sec_it) {
			type_name = sec_it->first;
			arch_name = sec_it->second;
			Ogre::ResourceGroupManager::getSingleton ().addResourceLocation (arch_name, type_name, section_name);
		}
	}
}

void
Application::loadResources ()
{
	Ogre::ResourceGroupManager::getSingleton ().initialiseAllResourceGroups ();
}

bool
Application::setup ()
{
	root = new Ogre::Root ();
	loadResourcePaths ();

	if (!configure ())
		return false;

	// FIXME - we'll probably need something more complicated than all this
	sceneManager = root->getSceneManager (Ogre::ST_GENERIC);
	camera = sceneManager->createCamera ("PlayerCam");

	// position and aim camera - right now, this is just copied over from
	// ExampleApplication
	camera->setPosition (Ogre::Vector3 (0, 0, 500));
	camera->lookAt (Ogre::Vector3 (0, 0, -300));
	camera->setNearClipDistance (5);

	// create a viewport
	Ogre::Viewport *viewport = renderWindow->addViewport (camera);
	viewport->setBackgroundColour (Ogre::ColourValue (0, 0, 0));
	camera->setAspectRatio (Ogre::Real (viewport->getActualWidth ()) / Ogre::Real (viewport->getActualHeight ()));

	Ogre::TextureManager::getSingleton ().setDefaultNumMipmaps (5);

	loadResources ();

	// create the info listener - this displays things like FHz
	infoListener = new InfoListener (renderWindow);
	root->addFrameListener (infoListener);
}

bool
Application::configure ()
{
	// FIXME - we'll want to store most of this stuff in whatever configuration
	// system we end up having, rather than running this dialog every time
	if (root->showConfigDialog ()) {
		renderWindow = root->initialise (true, "Screamers");
		return true;
	} else {
		return false;
	}
}

};
