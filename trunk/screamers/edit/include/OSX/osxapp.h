
#ifndef _OSX_APP_H_
#define _OSX_APP_H_

#ifdef __APPLE__
#include <Carbon/Carbon.h>

#include "mapedit.h"

class OSXView : public EditView
{
public:
	virtual ~OSXView(){};

	virtual void beginDraw ( void ){};
	virtual void endDraw ( void ){};

	virtual void setBGColor ( float r, float g, float b ){};

	virtual void size ( int x, int y ){};
};

class OSXApp: public EditApp
{
public:
	OSXApp(int argc, char* argv[]);
	virtual ~OSXApp();


	virtual bool run ( void );
	// view methods
	virtual EditView* getView ( void );
	virtual void redraw ( void );

	// generics
	virtual void doAboutBox ( void );

protected:
	IBNibRef 		nibRef;
	WindowRef 		window;

	OSStatus		err;

	OSXView			view;
};

#endif // __APPLE__
#endif // _OSX_APP_H_

