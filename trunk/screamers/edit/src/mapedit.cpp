// mapedit.cpp : Defines the entry point for the application.
//

#include "mapedit.h"

#include "WorldParser.h"

#ifdef _WIN32
#include "Win32/winApp.h"
#include "Win32/winView.h"
#endif

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "OSX/osxapp.h"
#endif


EditApp *app;

// call the correct main, and make the right app
#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	app = new WinApp(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
#else
int main ( int argc, char *argv[] )
{
	#ifdef __APPLE__
		app = new OSXApp(argc,argv);	// somone should implement this one
	#else
		app = new LinuxApp(argc,argv);	// somone should implement this one
	#endif //__APPLE__
#endif //_WIN32

	Editor	editor;
	app->setEditor(&editor);

	editor.init();
	app->redraw();

	return app->run()? 0 : 1;
}

void EditApp::changePullback ( float pullback )
{
	float newView = getView()->getViewPull()+pullback;
	if (newView > 0)
		getView()->setViewPull(newView);
}

void EditApp::spinView ( float xRot, float zRot )
{
	float rots[2];
	getView()->getViewRots(rots);

	rots[0]+= xRot;
	rots[1]+= zRot;

	getView()->setViewRot(rots[0],rots[1]);
}

void EditApp::setView ( float xRot, float zRot, float pullback )
{
	getView()->setViewPull(pullback);
	getView()->setViewRot(xRot,zRot);
}

Editor::Editor()
{
}

Editor::~Editor()
{
}

void Editor::init ( void )
{

/*	app->getView()->setBGColor(prefs.GetItemF("display:background:color:r"),
								prefs.GetItemF("display:background:color:g"),
								prefs.GetItemF("display:background:color:b")); */
}

void Editor::draw ( void )
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
	
	glColor4f(1,0,0,1);
	glVertex3f(10,0,0);
	glVertex3f(-10,0,0);

	glColor4f(0,1,0,1);
	glVertex3f(0,10,0);
	glVertex3f(0,-10,0);

	glColor4f(0,0,1,1);
	glVertex3f(0,0,10);
	glVertex3f(0,0,-10);

	glEnd();
}

void Editor::fileNew ( void )
{
}

void Editor::fileOpen ( void )
{
	trStandardFileOpen	fileOpenData;
	fileOpenData.filename = "*.xml";
	fileOpenData.title = "World File To Open";

	fileOpenData.extenstions.push_back(std::string("XML"));
	fileOpenData.extenstions.push_back(std::string("WLD"));
	fileOpenData.extenstion = 0;
	
	if (app->getStdFileOpen(fileOpenData))
	{
		world.clear();
		WorldParser	parser(world);
		parser.load(fileOpenData.filename.c_str());
	}
}

void Editor::fileSave ( void )
{
}

void Editor::fileSaveAs ( void )
{
}

void Editor::fileClose ( void )
{
}

void Editor::filePrefrences ( void )
{
	trFilePrefsData data;

/*	data.bgColor[0] = prefs.GetItemF("display:background:color:r");
	data.bgColor[1] = prefs.GetItemF("display:background:color:g");
	data.bgColor[2] = prefs.GetItemF("display:background:color:b"); */

	if ( app->doFilePrefs(data) )
	{
	/*	prefs.SetItem("display:background:color:r",data.bgColor[0]);
		prefs.SetItem("display:background:color:g",data.bgColor[1]);
		prefs.SetItem("display:background:color:b",data.bgColor[2]); */

		app->getView()->setBGColor(data.bgColor[0],data.bgColor[1],data.bgColor[2]);
		app->redraw();
	}
}

void Editor::fileQuit ( void )
{
}

void Editor::editUndo ( void )
{
}

void Editor::editCut ( void )
{
}

void Editor::editCopy ( void )
{
}

void Editor::editPaste( void )
{
}

void Editor::helpAbout ( void )
{
	app->doAboutBox();
}

void Editor::checkPrefsDefaults ( void )
{
/*	switch(prefs.GetItemI("version"))
	{
	case 0:
		prefs.SetItem("display:background:color:r",0.0f);
		prefs.SetItem("display:background:color:g",0.0f);
		prefs.SetItem("display:background:color:b",0.0f);

		prefs.SetItem("display:grid:size:major",10.0f);
		prefs.SetItem("display:grid:color:major:r",0.5f);
		prefs.SetItem("display:grid:color:major:g",0.5f);
		prefs.SetItem("display:grid:color:major:b",0.5f);

		prefs.SetItem("display:grid:size:minor",1.0f);
		prefs.SetItem("display:grid:color:minor:r",0.75f);
		prefs.SetItem("display:grid:color:minor:g",0.75f);
		prefs.SetItem("display:grid:color:minor:b",0.75f);

		prefs.SetItem("data:path","./");

	case 1:
		break;
	}

	prefs.SetItem("version",1);

	prefs.Update(); */
}



