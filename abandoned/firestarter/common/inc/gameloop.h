/* Firestarter
* gameloop.h :
*
* Copyright (C) 2004 Jeffrey Myers
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* email: jeffm2501@sbcglobal.net
*/

#ifndef _GAME_LOOP_H_
#define _GAME_LOOP_H_

#include "Ogre.h"
#include "OgreConfigFile.h"

#include <string>

using namespace Ogre;

/** BaseGameLoop to provide interface and initalisation to ogre
	@note
	the app needs to provide it's own derived CBaseGameLoop implementation
	if this base class is used the game will simply quit
*/

class  CBaseGameLoop : public FrameListener
{
public:
	/** default constructor
		@note
		should be called by derived class to init ogre vars
	*/
  CBaseGameLoop();

	/** default destructor
	@note
	dosn't do anything
	*/
  virtual ~CBaseGameLoop();

	/** Main entry point into game loop ( windows style )
		@remarks
		This is the main entrypoint into the app. It is called by main
		and will not return untill the app is ready to quit.
		@note
		This is the windows version of main, it expects all command args in one string, space deliminated
		@param args
		character string with the command line arguments passed in from main.
		@returns
		If the application terminitated with an error the method will return true
	*/
  bool Main ( const char* args );

	/** Main entry point into game loop ( unix style )
	@remarks
		This is the main entrypoint into the app. It is called by main
		and will not return untill the app is ready to quit.
		@note
		This is the unix version of main, it expects all command args in standard arg format
		@param argc
		number of command arguments in argv
		@param argv
		array of command arguments
		@returns
		If the application terminitated with an error the method will return true
	*/
  bool Main ( int argc, char *argv[] );

	/** Inintalisaion Callback
		@remarks
		Method to be called after engine, resources and plugins are loaded.
		This is where the derived class will call its scene initalisation and startup code
		@returns
		If the application needs to terminate the method will return true
	*/
  virtual bool OnInit ( void );

	/** Destruction Callback
		@remarks
		Method to be called just before the application exits, and the engine quits
		This is where the derived class will clean up it's mess
		@returns
		If the application needs to terminate the method will return true
	*/
  virtual bool OnKill( void );

	/** Activation Callback
	@remarks
	Method to be called just before the application is restored from being minimised,
	@returns
	If the application needs to terminate the method will return true
	*/
  virtual bool OnActivate ( void );

	/** Deactivation Callback
	@remarks
	Method to be called just before the application is minimised,
	@returns
	If the application needs to terminate the method will return true
	*/
  virtual bool OnDeactivate ( void );

	/** Main Loop Callback
	@remarks
	Method to be called at the start of each draing fame
	This is where the derived class will do it's per frame code
	@returns
	If the application needs to terminate the method will return true
	*/
  virtual bool GameLoop ( void );

	/** FrameEnd Callback
	@remarks
	Method to be called at the end of each draing fame
	Generaly not used by most apps. A good place to get per frame statistics from ogre if needed.
	*/
  virtual void OnFrameEnd ( void );

	/** Window Name Callback
	@remarks
	Method to be called when the base class needs the name of the window.
	The derived class should overide this and return what ever name it wishes
	@returns
	Name to put in the catption of the main rendering OS window
	*/
	virtual const char* GetWindowName ( void );

	/** SceneType Callback
	@remarks
	Method to be called when the base class needs an ogre scene type
	The derived class should overide this and return the ogre scene type it requires
	@returns
	SceneType to use at ogre init time
	*/
  virtual SceneType GetSceneType ( void );

	/** camera Name Callback
	@remarks
	Method to be called when the base class needs the name of the camera.
	The derived class should overide this and return what ever name it wishes
	@returns
	Name of the main camera atached to the rendering window
	*/
  virtual const char* GetCameraName ( void );

	/** Root Reource Dir Name Callback
	@remarks
	Method to be called when the base class needs the name base dir for resource scaning
	The derived class should overide this and return what ever name it wishes
	@returns
	Dir to start a resource search in, allways in unix format
	*/
  virtual const char* GetRootResDir ( void );

	/** Plugin Dir Callback
	@remarks
	Method to be called when the base class needs the dircetory for plugins
	The derived class should overide this and return what ever name it wishes
	@returns
	Dir to start a plugin search in, allways in unix format
	*/
	virtual const char* GetPluginsDir ( void );

	/** Prefrences File Name Callback
	@remarks
	Method to be called when the base class needs the filename for the prefs file
	The derived class should overide this and return what ever name it wishes
	@returns
	filename to of the prefs file, allways in unix format
	*/
	virtual const char* GetPrefsName ( void );

 // info methods.

  Camera* GetCamera ( void ) {return mCamera;}
  Root* GetRoot ( void ) {return mRoot;}
  SceneManager* GetSceneManager ( void ) {return mSceneMgr;}
  RenderWindow* GetRenderWindow ( void ) {return mWindow;}

	// common operations
	void ClearScene ( void );

  // OGRE frame listener
  bool frameStarted(const FrameEvent& evt);
  bool frameEnded(const FrameEvent& evt);

	// global quit handler
	void SetQuit ( bool set ) {quit = set;}

	void SetGameName ( const char* name );
	void SetGameStartString ( const char *startString );

	const char* GetGameName ( void );
	const char* GetGameStartString ( void ){ return gameStartString.c_str();}

protected:
	bool LoadPlugins ( void );
  bool SetupConfigure ( void );
  bool SetupResources ( void );
  bool Process( void );
  bool Run ( void );

  void ChooseSceneManager ( void );
  void CreateCamera ( void );
  void CreateViewports ( void );

	void updateStats ( void );
	void showDebugOverlay(bool show);

  Root *mRoot;
  Camera* mCamera;
  SceneManager* mSceneMgr;
  RenderWindow* mWindow;

	bool				quit;
	std::string	gameName;
	std::string gameStartString;

	bool				showDebug;
};

#endif //_GAME_LOOP_H_

