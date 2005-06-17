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

#ifndef _SCREAMERS_H_
#define _SCREAMERS_H_

// windows-specific includes
#ifdef _WIN32
#include "resource.h"
#include <windows.h>
#endif

#include "Ogre.h"
#include "OgreKeyEvent.h"
#include "OgreEventListeners.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

using namespace Ogre;

class ApplicationFrameListener: public FrameListener, public KeyListener
{
public:
	// Constructor takes a RenderWindow because it uses that to determine input context
	ApplicationFrameListener(RenderWindow* win, Camera* cam, bool useBufferedInputKeys = false, bool useBufferedInputMouse = false);
	virtual ~ApplicationFrameListener();

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt);

	bool processUnbufferedMouseInput(const FrameEvent& evt);

	void moveCamera();

	void showDebugOverlay(bool show);

	// Override frameStarted event to process that (don't care about frameEnded)
	bool frameStarted(const FrameEvent& evt);

	bool frameEnded(const FrameEvent& evt);

	void switchMouseMode();
	void switchKeyMode();

	void keyClicked(KeyEvent* e);
	void keyPressed(KeyEvent* e);
	void keyReleased(KeyEvent* e);

protected:
	EventProcessor* mEventProcessor;
	InputReader* mInputDevice;
	Camera* mCamera;

	Vector3 mTranslateVector;
	RenderWindow* mWindow;
	bool mStatsOn;
	bool mUseBufferedInputKeys, mUseBufferedInputMouse, mInputTypeSwitchingOn;
	unsigned int mNumScreenShots;
	float mMoveScale;
	Degree mRotScale;
	// just to stop toggles flipping too fast
	Real mTimeUntilNextToggle ;
	Radian mRotX, mRotY;
	TextureFilterOptions mFiltering;
	int mAniso;

	int mSceneDetailIndex ;
	Real mMoveSpeed;
	Degree mRotateSpeed;
	Overlay* mDebugOverlay;

	void updateStats(void);

};

#endif //_SCREAMERS_H_
