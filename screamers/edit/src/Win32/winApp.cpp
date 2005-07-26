// mapedit.cpp : Defines the entry point for the application.
//

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include "mapedit.h"
#include "Win32/winApp.h"
#include "TextUtils.h"

// Forward declarations of functions included in this code module:
LRESULT CALLBACK	_WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	prefs(HWND, UINT, WPARAM, LPARAM);


WinApp::WinApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
 	// TODO: Place code here.
	srand(GetTickCount());

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MAPEDIT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
		return;

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MAPEDIT);
}

WinApp::~WinApp()
{

}

bool WinApp::run ( void )
{
// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam == 0;
}

void  WinApp::redraw ( void )
{
	InvalidateRect(hWnd,NULL,false);
}

void WinApp::doAboutBox ( void )
{
	DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
}

bool  WinApp::doFilePrefs ( trFilePrefsData	&data )
{
	return DialogBoxParam(hInst, (LPCTSTR)IDD_FILE_PREFS_DLOG, hWnd, (DLGPROC)prefs,(LPARAM)&data) == IDOK;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM WinApp::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)_WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(WinApp*);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_MAPEDIT);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_MAPEDIT;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL WinApp::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, (int*)this);

   if (!hWnd)
      return FALSE;

   SetWindowLong(hWnd,GWLP_USERDATA,(LONG)this);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT WinApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message) 
	{
	case WM_CREATE:
		view.init(hWnd,hInst);
		mouseDown = false;
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			editor->helpAbout();
			break;

		case IDM_EXIT:
			editor->fileQuit();
			DestroyWindow(hWnd);
			break;

			// file menu
		case ID_FILE_NEW:
			editor->fileNew();
			break;

		case ID_FILE_OPEN:
			editor->fileOpen();
			break;

		case ID_FILE_SAVE:
			editor->fileSave();
			break;

		case ID_FILE_SAVEAS:
			editor->fileSaveAs();
			break;

		case ID_FILE_PREFRENCES:
			editor->filePrefrences();
			break;


			// edit menu
		case ID_EDIT_UNDO:
			editor->editUndo();
			break;

		case ID_EDIT_CUT:
			editor->editCut();
			break;
	
		case ID_EDIT_COPY:
			editor->editCopy();
			break;

		case ID_EDIT_PASTE:
			editor->editPaste();
			break;
		
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		view.beginDraw();
		if (editor)
			editor->draw();
		view.endDraw();
		break;

	case WM_ERASEBKGND:
		return 1;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PALETTECHANGED:
		view.pallChanged();
		break;

	case WM_QUERYNEWPALETTE:
		view.queryNewPallet();
		break;

	case WM_SIZE:
		view.size(LOWORD(lParam),HIWORD(lParam));
		break;

	case WM_RBUTTONDOWN:
		mouseDown = true;
		mousePos[0] = LOWORD(lParam); 
		mousePos[1] = HIWORD(lParam); 

		SetCapture(hWnd);
		break;

	case WM_RBUTTONUP:
		mouseDown = false;
		mousePos[0] = LOWORD(lParam); 
		mousePos[1] = HIWORD(lParam); 

		ReleaseCapture();
		break;

	case WM_MOUSEMOVE:
		if (mouseDown)
		{
			int dX = LOWORD(lParam)-mousePos[0];
			int dY = HIWORD(lParam)-mousePos[1];
			mousePos[0] = LOWORD(lParam); 
			mousePos[1] = HIWORD(lParam); 

			spinView(dY*0.5f,dX*-0.5f);

			redraw();
		// figure out a rotation
		}
		break;

	case WM_MOUSEWHEEL:
		{
			short	raw = HIWORD(wParam);
			float dist = raw/(float)WHEEL_DELTA;
			changePullback(dist);
			redraw();
		}
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_LEFT:
			spinView(0,10.0f);
			break;
		
		case VK_RIGHT:
			spinView(0,10.0f);
			break;

		case VK_UP:
			spinView(10,0);
			break;
		
		case VK_DOWN:
			spinView(-10,0);
			break;
		}
		redraw();
		break;


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WinApp* app = (WinApp*)GetWindowLong(hWnd,GWLP_USERDATA);

	if (message == WM_CREATE) 
		app = (WinApp*)(((CREATESTRUCT*)lParam)->lpCreateParams);

	if (app)
		return app->WndProc(hWnd, message, wParam, lParam);
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

// Message handler for prefs dialog.
LRESULT CALLBACK prefs(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	trFilePrefsData	*data = NULL;
	if ( message == WM_INITDIALOG)
		data = (trFilePrefsData*)lParam;
	else
		data = (trFilePrefsData*)GetWindowLong(hDlg,GWLP_USERDATA);

	if (data)
	{
		switch (message)
		{
		case WM_INITDIALOG:
			SetWindowLong(hDlg,GWLP_USERDATA,(LONG)data);

			SetDlgItemText(hDlg,IDC_BG_COLOR_RED,TextUtils::format("%4.3f",data->bgColor[0]).c_str());
			SetDlgItemText(hDlg,IDC_BG_COLOR_GREEN,TextUtils::format("%4.3f",data->bgColor[1]).c_str());
			SetDlgItemText(hDlg,IDC_BG_COLOR_BLUE,TextUtils::format("%4.3f",data->bgColor[2]).c_str()); 
			return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDOK:
				{
					char temp[128];

					GetDlgItemText(hDlg,IDC_BG_COLOR_RED,temp,128);
					data->bgColor[0] = (float)atof(temp);

					GetDlgItemText(hDlg,IDC_BG_COLOR_GREEN,temp,128);
					data->bgColor[1] = (float)atof(temp);

					GetDlgItemText(hDlg,IDC_BG_COLOR_BLUE,temp,128);
					data->bgColor[2] =(float) atof(temp);

					EndDialog(hDlg, IDOK);
				}
				return TRUE;

			case IDCANCEL:
				EndDialog(hDlg, IDCANCEL);
				return TRUE;
			}
			return false;
		}
	}
	return FALSE;
}
