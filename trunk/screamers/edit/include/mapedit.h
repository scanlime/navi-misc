#ifndef _MAPEDIT_H_
#define _MAPEDIT_H_

#include <stdio.h>

#include "dialogData.h"

/*
 *	Editor class handles all logical edit operations. It does everything the app needs to do
 *  so the view and app can be OS independent
 */
class Editor
{
public:
	Editor();
	~Editor();

	void init ( void );
	void draw ( void );

	void fileNew ( void );
	void fileOpen ( void );
	void fileSave ( void );
	void fileSaveAs ( void );
	void fileClose ( void );
	void filePrefrences ( void );
	void fileQuit ( void );

	void editUndo ( void );
	void editCut ( void );
	void editCopy ( void );
	void editPaste( void );

	void helpAbout ( void );

protected:
	void checkPrefsDefaults ( void );
};

/*
 *	EditView is the base view class. There is no implemenation for this class
 *  each OS/API type should implement it's own version of it.
 */
class EditView
{
public:
	virtual ~EditView(){};

	virtual void beginDraw ( void ) = 0;
	virtual void endDraw ( void ) = 0;

	virtual void setBGColor ( float r, float g, float b ) = 0;

	virtual void setViewPull ( float pull )
	{
		pullback = pull;
	}

	virtual float getViewPull ( void )
	{
		return pullback;
	}

	virtual void setViewRot ( float xRot, float zRot )
	{
		rot[0] = xRot;
		rot[1] = zRot;
	}

	virtual void getViewRots ( float rots[2] )
	{
		rots[0] = rot[0];
		rots[1] = rot[1];
	}

	virtual void size ( int x, int y ) = 0;
protected:

	float pullback;
	float rot[2];
};

/*
*	EditApp is the base application class. There is no implemenation for this class
*  each OS/API type should implement it's own version of it.
*  this classes job is to build the OS specific application environement and handle message maping.
*/
class EditApp
{
public:
	EditApp(){editor = NULL;}
	virtual ~EditApp(){};

	virtual bool run ( void ) = 0;

	void setEditor ( Editor *ed ){editor = ed;}

	// view methods
	virtual EditView* getView ( void ) = 0;
	virtual void changePullback ( float pullback );
	virtual void spinView ( float xRot, float zRot ); // rotate incrementaly
	virtual void setView ( float xRot, float zRot, float pullback ); // rotate absolute

	virtual void redraw ( void ) = 0;

	// dialog boxes
	virtual void doAboutBox ( void ) = 0;
	virtual bool doFilePrefs ( trFilePrefsData	&data ) = 0;

protected:
	Editor	*editor;
};

#endif//_MAPEDIT_H_