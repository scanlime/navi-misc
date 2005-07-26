//
//  main.c
//  mapedit
//
//  Created by Jeff Myers on 3/15/05.
//  Copyright __MyCompanyName__ 2005. All rights reserved.
//

#include "OSX/osxapp.h"

OSXApp::OSXApp(int argc, char* argv[])
{
	err = CreateNibReference(CFSTR("main"), &nibRef);
	err = SetMenuBarFromNib(nibRef, CFSTR("MenuBar"));
	err = CreateWindowFromNib(nibRef, CFSTR("MainWindow"), &window);
	DisposeNibReference(nibRef);

	ShowWindow( window );
}

OSXApp::~OSXApp()
{
}

bool OSXApp::run ( void )
{
	RunApplicationEventLoop();
	return err;
}

EditView* OSXApp::getView ( void )
{
	return &view;
}

void OSXApp::redraw ( void )
{

}

void OSXApp::doAboutBox ( void )
{

}


