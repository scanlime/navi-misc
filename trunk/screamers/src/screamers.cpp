/* screamers
 * Copyright (c) 2005 ScreamersGroup
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <string>
#include <iostream>
#include "screamers.h"
#include "version.h"

#include "Ogre.h"

int appMain ( std::string commandLine )
{
	std::cout << "version " << VERSION << std::endl;
	return 0;
}


// platform-specific main functions, 'cuz one of those platforms uses a silly one
#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	std::string cli  = lpCmdLine;
	return appMain(cli);
}
#else
int main( int argc, char *argv[])
{
	// FIXME - this is bad!
	std::string	cli;
	for ( int i = 1; i < argc; i++)
		cli += argv[i] + std::string(" ");

	return appMain(cli);
}
#endif


void ApplicationFrameListener::updateStats(void)
{
	static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";

	// update stats when necessary
	try {
		OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats& stats = mWindow->getStatistics();

		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

		OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

		OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		guiDbg->setCaption(mWindow->getDebugText());
	}
	catch(...)
	{
		// ignore
	}
}

ApplicationFrameListener::ApplicationFrameListener(RenderWindow* win, Camera* cam, bool useBufferedInputKeys, bool useBufferedInputMouse)
{
	mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	mUseBufferedInputKeys = useBufferedInputKeys;
	mUseBufferedInputMouse = useBufferedInputMouse;
	mInputTypeSwitchingOn = mUseBufferedInputKeys || mUseBufferedInputMouse;
	mRotateSpeed = 36;
	mMoveSpeed = 100;

	if (mInputTypeSwitchingOn)
	{
		mEventProcessor = new EventProcessor();
		mEventProcessor->initialise(win);
		mEventProcessor->startProcessingEvents();
		mEventProcessor->addKeyListener(this);
		mInputDevice = mEventProcessor->getInputReader();

	}
	else
	{
		mInputDevice = PlatformManager::getSingleton().createInputReader();
		mInputDevice->initialise(win,true, true);
	}

	mCamera = cam;
	mWindow = win;
	mStatsOn = true;
	mNumScreenShots = 0;
	mTimeUntilNextToggle = 0;
	mSceneDetailIndex = 0;
	mMoveScale = 0.0f;
	mRotScale = 0.0f;
	mTranslateVector = Vector3::ZERO;
	mAniso = 1;
	mFiltering = TFO_BILINEAR;

	showDebugOverlay(true);
}

ApplicationFrameListener::~ApplicationFrameListener()
{
	if (mInputTypeSwitchingOn)
	{
		delete mEventProcessor;
	}
	else
	{
		PlatformManager::getSingleton().destroyInputReader( mInputDevice );
	}
}

bool ApplicationFrameListener::processUnbufferedKeyInput(const FrameEvent& evt)
{
	if (mInputDevice->isKeyDown(KC_A))
	{
		// Move camera left
		mTranslateVector.x = -mMoveScale;
	}

	if (mInputDevice->isKeyDown(KC_D))
	{
		// Move camera RIGHT
		mTranslateVector.x = mMoveScale;
	}

	/* Move camera forward by keypress. */
	if (mInputDevice->isKeyDown(KC_UP) || mInputDevice->isKeyDown(KC_W) )
	{
		mTranslateVector.z = -mMoveScale;
	}

	/* Move camera backward by keypress. */
	if (mInputDevice->isKeyDown(KC_DOWN) || mInputDevice->isKeyDown(KC_S) )
	{
		mTranslateVector.z = mMoveScale;
	}

	if (mInputDevice->isKeyDown(KC_PGUP))
	{
		// Move camera up
		mTranslateVector.y = mMoveScale;
	}

	if (mInputDevice->isKeyDown(KC_PGDOWN))
	{
		// Move camera down
		mTranslateVector.y = -mMoveScale;
	}

	if (mInputDevice->isKeyDown(KC_RIGHT))
	{
		mCamera->yaw(-mRotScale);
	}

	if (mInputDevice->isKeyDown(KC_LEFT))
	{
		mCamera->yaw(mRotScale);
	}

	if( mInputDevice->isKeyDown( KC_ESCAPE) )
	{            
		return false;
	}

	// see if switching is on, and you want to toggle 
	if (mInputTypeSwitchingOn && mInputDevice->isKeyDown(KC_M) && mTimeUntilNextToggle <= 0)
	{
		switchMouseMode();
		mTimeUntilNextToggle = 1;
	}

	if (mInputTypeSwitchingOn && mInputDevice->isKeyDown(KC_K) && mTimeUntilNextToggle <= 0)
	{
		// must be going from immediate keyboard to buffered keyboard
		switchKeyMode();
		mTimeUntilNextToggle = 1;
	}
	if (mInputDevice->isKeyDown(KC_F) && mTimeUntilNextToggle <= 0)
	{
		mStatsOn = !mStatsOn;
		showDebugOverlay(mStatsOn);

		mTimeUntilNextToggle = 1;
	}
	if (mInputDevice->isKeyDown(KC_T) && mTimeUntilNextToggle <= 0)
	{
		switch(mFiltering)
		{
		case TFO_BILINEAR:
			mFiltering = TFO_TRILINEAR;
			mAniso = 1;
			break;
		case TFO_TRILINEAR:
			mFiltering = TFO_ANISOTROPIC;
			mAniso = 8;
			break;
		case TFO_ANISOTROPIC:
			mFiltering = TFO_BILINEAR;
			mAniso = 1;
			break;
		default:
			break;
		}
		MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
		MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);


		showDebugOverlay(mStatsOn);

		mTimeUntilNextToggle = 1;
	}

	if (mInputDevice->isKeyDown(KC_SYSRQ) && mTimeUntilNextToggle <= 0)
	{
		char tmp[20];
		sprintf(tmp, "screenshot_%d.png", ++mNumScreenShots);
		mWindow->writeContentsToFile(tmp);
		mTimeUntilNextToggle = 0.5;
		mWindow->setDebugText(String("Wrote ") + tmp);
	}

	if (mInputDevice->isKeyDown(KC_R) && mTimeUntilNextToggle <=0)
	{
		mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
		switch(mSceneDetailIndex) {
case 0 : mCamera->setDetailLevel(SDL_SOLID) ; break ;
case 1 : mCamera->setDetailLevel(SDL_WIREFRAME) ; break ;
case 2 : mCamera->setDetailLevel(SDL_POINTS) ; break ;
		}
		mTimeUntilNextToggle = 0.5;
	}

	static bool displayCameraDetails = false;
	if (mInputDevice->isKeyDown(KC_P) && mTimeUntilNextToggle <= 0)
	{
		displayCameraDetails = !displayCameraDetails;
		mTimeUntilNextToggle = 0.5;
		if (!displayCameraDetails)
			mWindow->setDebugText("");
	}
	if (displayCameraDetails)
	{
		// Print camera details
		mWindow->setDebugText("P: " + StringConverter::toString(mCamera->getDerivedPosition()) + " " + 
			"O: " + StringConverter::toString(mCamera->getDerivedOrientation()));
	}

	// Return true to continue rendering
	return true;
}

bool ApplicationFrameListener::processUnbufferedMouseInput(const FrameEvent& evt)
{
	/* Rotation factors, may not be used if the second mouse button is pressed. */

	/* If the second mouse button is pressed, then the mouse movement results in 
	sliding the camera, otherwise we rotate. */
	if( mInputDevice->getMouseButton( 1 ) )
	{
		mTranslateVector.x += mInputDevice->getMouseRelativeX() * 0.13;
		mTranslateVector.y -= mInputDevice->getMouseRelativeY() * 0.13;
	}
	else
	{
		mRotX = Degree(-mInputDevice->getMouseRelativeX() * 0.13);
		mRotY = Degree(-mInputDevice->getMouseRelativeY() * 0.13);
	}


	return true;
}

void ApplicationFrameListener::moveCamera()
{

	// Make all the changes to the camera
	// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW (e.g. airplane)
	mCamera->yaw(mRotX);
	mCamera->pitch(mRotY);
	mCamera->moveRelative(mTranslateVector);


}

void ApplicationFrameListener::showDebugOverlay(bool show)
{
	if (mDebugOverlay)
	{
		if (show)
		{
			mDebugOverlay->show();
		}
		else
		{
			mDebugOverlay->hide();
		}
	}
}

// Override frameStarted event to process that (don't care about frameEnded)
bool ApplicationFrameListener::frameStarted(const FrameEvent& evt)
{
	if(mWindow->isClosed())
		return false;

	if (!mInputTypeSwitchingOn)
	{
		mInputDevice->capture();
	}


	if ( !mUseBufferedInputMouse || !mUseBufferedInputKeys)
	{
		// one of the input modes is immediate, so setup what is needed for immediate mouse/key movement
		if (mTimeUntilNextToggle >= 0) 
			mTimeUntilNextToggle -= evt.timeSinceLastFrame;

		// If this is the first frame, pick a speed
		if (evt.timeSinceLastFrame == 0)
		{
			mMoveScale = 1;
			mRotScale = 0.1;
		}
		// Otherwise scale movement units by time passed since last frame
		else
		{
			// Move about 100 units per second,
			mMoveScale = mMoveSpeed * evt.timeSinceLastFrame;
			// Take about 10 seconds for full rotation
			mRotScale = mRotateSpeed * evt.timeSinceLastFrame;
		}
		mRotX = 0;
		mRotY = 0;
		mTranslateVector = Vector3::ZERO;
	}

	if (mUseBufferedInputKeys)
	{
		// no need to do any processing here, it is handled by event processor and 
		// you get the results as KeyEvents
	}
	else
	{
		if (processUnbufferedKeyInput(evt) == false)
		{
			return false;
		}
	}
	if (mUseBufferedInputMouse)
	{
		// no need to do any processing here, it is handled by event processor and 
		// you get the results as MouseEvents
	}
	else
	{
		if (processUnbufferedMouseInput(evt) == false)
		{
			return false;
		}
	}

	if ( !mUseBufferedInputMouse || !mUseBufferedInputKeys)
	{
		// one of the input modes is immediate, so update the movement vector
		moveCamera();
	}

	return true;
}

bool ApplicationFrameListener::frameEnded(const FrameEvent& evt)
{
	updateStats();
	return true;
}

void ApplicationFrameListener::switchMouseMode() 
{
	mUseBufferedInputMouse = !mUseBufferedInputMouse;
	mInputDevice->setBufferedInput(mUseBufferedInputKeys, mUseBufferedInputMouse);
}

void ApplicationFrameListener::switchKeyMode() 
{
	mUseBufferedInputKeys = !mUseBufferedInputKeys;
	mInputDevice->setBufferedInput(mUseBufferedInputKeys, mUseBufferedInputMouse);
}

void ApplicationFrameListener::keyClicked(KeyEvent* e) 
{

}

void ApplicationFrameListener::keyPressed(KeyEvent* e)
{
}

void ApplicationFrameListener::keyReleased(KeyEvent* e)
{
}


