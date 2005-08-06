/*
 * Application.cpp - application classes which generally run everything
 *
 * Copyright (C) 2005 David Trowbridge
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

#include "Application.h"

Application::Application (void)
{
	main_loop = new MainLoop ();

	configured = false;
}

Application::~Application (void)
{
	if (main_loop)
		delete main_loop;
}

void Application::main (void)
{
	if ((!configured) && (!setup ())) {
		std::cerr << "Could not set up application, quitting\n";
		return;
	}
	this->go ();
	// FIXME - we may want to add something here to allow an Application to clean
	// up things before the destructor gets called (depends on how we work out
	// program lifecycle)
}

HeadlessApplication::HeadlessApplication (void)
{
}

HeadlessApplication::~HeadlessApplication (void)
{
}

template<> HeadlessApplication *Ogre::Singleton<HeadlessApplication>::ms_Singleton = 0;
HeadlessApplication &HeadlessApplication::getSingleton (void)
{
	assert (ms_Singleton);
	return (*ms_Singleton);
}

HeadlessApplication *HeadlessApplication::getSingletonPtr (void)
{
	return ms_Singleton;
}

bool HeadlessApplication::setup (void)
{
	configured = true;
	return true;
}

void HeadlessApplication::go (void)
{
	main_loop->go ();
}

WindowedApplication::WindowedApplication (void)
{
	root = NULL;
}

WindowedApplication::~WindowedApplication (void)
{
	if (root)
		delete root;
}

template<> WindowedApplication *Ogre::Singleton<WindowedApplication>::ms_Singleton = 0;
WindowedApplication &WindowedApplication::getSingleton (void)
{
	assert (ms_Singleton);
	return (*ms_Singleton);
}

WindowedApplication *WindowedApplication::getSingletonPtr (void)
{
	return ms_Singleton;
}

Ogre::Root *WindowedApplication::getRoot (void)
{
	return root;
}

Ogre::Camera *WindowedApplication::getCamera (void)
{
	return camera;
}

Ogre::SceneManager *WindowedApplication::getSceneManager (void)
{
	return sceneManager;
}

Ogre::RenderWindow *WindowedApplication::getRenderWindow (void)
{
	return renderWindow;
}

bool WindowedApplication::setup (void)
{
	if (configured)
		return true;
	root = new Ogre::Root ();
	loadResourcePaths ();

	if (!configure ())
		return false;

	// FIXME - we'll probably need something more complicated than all this
	sceneManager = root->getSceneManager (Ogre::ST_GENERIC);
	camera = sceneManager->createCamera ("PlayerCam");

	// position and aim camera - right now, this is just copied over from ExampleApplication
	camera->setPosition (Ogre::Vector3 (0, 0, 500));
	camera->lookAt (Ogre::Vector3 (0, 0, -300));
	camera->setNearClipDistance (5);

	// create a viewport
	Ogre::Viewport *viewport = renderWindow->addViewport (camera);

	viewport->setBackgroundColour (Ogre::ColourValue (0, 0, 0));
	camera->setAspectRatio (Ogre::Real (viewport->getActualWidth ()) / Ogre::Real (viewport->getActualHeight ()));

	Ogre::TextureManager::getSingleton ().setDefaultNumMipmaps (5);
	loadResources ();

	// create the various frame listeners that handle important, always-on stuff
	infoListener = new InfoListener (renderWindow);
	mainListener = new MainLoopTrigger ();
	root->addFrameListener (mainListener);

	configured = true;
	return true;
}

void WindowedApplication::go (void)
{
	root->startRendering ();
}

bool WindowedApplication::configure (void)
{
	if ((root->restoreConfig ()) || (root->showConfigDialog ())) {
		renderWindow = root->initialise (true, "Screamers");
		return true;
	}
	return false;
}

void WindowedApplication::loadResourcePaths (void)
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

void WindowedApplication::loadResources (void)
{
	Ogre::ResourceGroupManager::getSingleton ().initialiseAllResourceGroups ();
}
