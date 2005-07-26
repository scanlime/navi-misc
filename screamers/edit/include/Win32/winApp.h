#ifndef _WINAPP_H_
#define _WINAPP_H_

#ifdef _WIN32 // this file only has windows stuff

#define _WIN32_WINNT  0x0500

#include <windows.h>
#include "Win32/winView.h"
#include "Resource.h"

class WinApp: public EditApp
{
public:
	WinApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
	virtual ~WinApp();

	EditView* getView ( void ){return &view;}
	virtual void redraw ( void );

	bool run ( void );

	// dialog boxes
	virtual void doAboutBox ( void );
	virtual bool doFilePrefs ( trFilePrefsData	&data );

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
	ATOM MyRegisterClass(HINSTANCE hInstance);

	WinView	view;

#define MAX_LOADSTRING 100

	// Global Variables:
	HINSTANCE hInst;								// current instance
	HWND hWnd;
	MSG msg;
	HACCEL hAccelTable;
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

	// mouse drag stuff
	bool mouseDown;
	int mousePos[2];
};

#endif//_WIN32
#endif//_WINAPP_H_