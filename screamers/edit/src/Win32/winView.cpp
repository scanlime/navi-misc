
// winView.cpp : Defines the entry point for the application.
//

#include "Win32/winView.h"

#define		_NearClip	1.0
#define		_FarClip	10000.0

#define piOver180 0.017453292519943295769236907684886f

WinView::WinView()
{
	depthBuffer = 32;
	depthBuffer = 32;
	stencelBuffer = 0;
	alphaBuffer = 0;
	auxBuffer = 0;
	bufferMode = PFD_TYPE_RGBA;

	fov = 60;

	pullback = 25;
	rot[0] = 25;
	rot[1] = 25;
	hGLRC = NULL;

	bgColor[0] = 0;
	bgColor[1] = 0;
	bgColor[1] = 0;
}

WinView::~WinView()
{
}

void WinView::beginDraw ( void )
{
	ValidateRect(hWnd,NULL);

	if (!hGLRC)
		return;

	wglMakeCurrent(hdc, hGLRC);

	// rest GL
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	// set up for a Z oriented matrix
	glTranslatef(0,0,-pullback);							// pull back on allong the zoom vector
	glRotatef(rot[0], 1.0f, 0.0f, 0.0f);					// pops us to the tilt
	glRotatef(-rot[1], 0.0f, 1.0f, 0.0f);					// gets us on our rot
	glRotatef(-90, 1.0f, 0.0f, 0.0f);						// gets us into XY

	glPushMatrix();

	// set the light in world space
	float pos[4] = {1,1,1,0};

	glLightfv (GL_LIGHT0, GL_POSITION,pos);
}

void WinView::endDraw ( void )
{
	if (!hGLRC)
		return;

	wglMakeCurrent(hdc, hGLRC);
	glPopMatrix();
	//	flip it
	SwapBuffers(hdc);
}

void WinView::size ( int x, int y )
{
	aspect = 1.33f;

	viewportX = x;
	viewportY = y;

	wglMakeCurrent(hdc, hGLRC);

	if (viewportY != 0)
		aspect = (float) viewportX / (float) viewportY;

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (fov,        // Field-of-view angle (def = 30)
		aspect,					// Aspect ratio of viewing volume
		_NearClip,				// Distance to near clipping plane
		_FarClip);				// Distance to far clipping plane

	glViewport (0, 0, viewportX, viewportY);
}

void WinView::init ( HWND _hWnd, HINSTANCE hInstance )
{
	hWnd = _hWnd;

	RECT	Rect;

	GetClientRect(hWnd,&Rect);
	initGL (Rect.right,Rect.top);
}

void WinView::setBGColor ( float r, float g, float b )
{
	float		bgColor[3];
	bgColor[0] = r;
	bgColor[1] = g;
	bgColor[2] = b;

	glClearColor (r,g,b, 1.0);
}

void WinView::initGL ( long iX, long iY )
{
	viewportX = iX;
	viewportY = iY;

	// create a new context
	hdc = GetDC(hWnd);

	SetupPixelFormat();
	SetupPalette();
	hGLRC = wglCreateContext (hdc);
	wglMakeCurrent(hdc, hGLRC);

	setBGColor(bgColor[0],bgColor[1],bgColor[2]);

	// make everything look it's best not fastest.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT ,GL_NICEST);

	// we want a z buffer
	glEnable (GL_DEPTH_TEST);

	// we want back face culling
	glEnable (GL_CULL_FACE);
	glCullFace(GL_BACK);

	glFrontFace(GL_CCW);

	// we want smooth filled polies
	glShadeModel (GL_SMOOTH);
	glPolygonMode (GL_FRONT, GL_FILL);

	float v[4] = {1.0f};

	v[0] = 0.25f; v[1] = 0.25f; v[2] = 0.25f; 
	glLightfv (GL_LIGHT0, GL_AMBIENT,v);

	v[0] = 0.75f; v[1] = 0.75f; v[2] = 0.75f; 
	glLightfv (GL_LIGHT0, GL_DIFFUSE,v);
	//glLightf (GL_LIGHT0, GL_SPECULAR,glLights[i].spec);

	v[0] = 100; v[1] = 100; v[2] = 100; 
	glLightfv (GL_LIGHT0, GL_POSITION,v);

	// but only enable one ?!?
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0); 

	// we want alpha based transperancy
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable (GL_COLOR_MATERIAL);

	// set the projection matrix and viewport info
	SetDepthRange(_NearClip,_FarClip);

	// clear out the model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();
}

void WinView::SetDepthRange ( float fNearZ, float xFarZ )
{
	farZ = xFarZ;
	nearZ = fNearZ;

	wglMakeCurrent(hdc, hGLRC);
	if (hGLRC)
	{
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		gluPerspective (fov,				// Field-of-view angle (def = 30)
			aspect,			// Aspect ratio of viewing volume
			nearZ,			// Distance to near clipping plane
			farZ);			// Distance to far clipping plane
		glViewport (0, 0, viewportX, viewportY);
	}
}

void WinView::queryNewPallet ( void )
{
	if (hGLRC && hPalette) 
		RedoPalette ();
}

void WinView::pallChanged ( void )
{
	RedoPalette ();
}

void WinView::RedoPalette ( void )
{
	UnrealizeObject (hPalette);
	SelectPalette (hdc, hPalette, false);
	RealizePalette (hdc);
}

void WinView::SetupPalette ( void )
{
	int pixelFormat = GetPixelFormat(hdc);
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE* pPal;
	int paletteSize;

	DescribePixelFormat(hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	if (pfd.dwFlags & PFD_NEED_PALETTE)
		paletteSize = 1 << pfd.cColorBits;
	else 
		return;

	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
	pPal->palVersion = 0x300;
	pPal->palNumEntries = paletteSize;

	// build a simple RGB color palette 
	{
		int redMask = (1 << pfd.cRedBits) - 1;
		int greenMask = (1 << pfd.cGreenBits) - 1;
		int blueMask = (1 << pfd.cBlueBits) - 1;
		int i;

		for (i=0; i<paletteSize; ++i)
		{
			pPal->palPalEntry[i].peRed =
				(((i >> pfd.cRedShift) & redMask) * 255) / redMask;
			pPal->palPalEntry[i].peGreen =
				(((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
			pPal->palPalEntry[i].peBlue =
				(((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
			pPal->palPalEntry[i].peFlags = 0;
		}
	}

	hPalette = (HPALETTE)CreatePalette(pPal);
	free(pPal);

	if (hPalette)
	{
		SelectPalette(hdc, hPalette, false);
		RealizePalette(hdc);
	}
}

void WinView::SetupPixelFormat ( void )
{
	HANDLE hHeap;
	int nColors, i;
	LPLOGPALETTE lpPalette;
	BYTE byRedMask, byGreenMask, byBlueMask;

	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof (PIXELFORMATDESCRIPTOR),             // Size of this structure
			1,                                          // Version number
			PFD_DRAW_TO_WINDOW |                        // Flags
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER ,
			bufferMode,                              // RGBA pixel values
			frameBuffer,                             // 24-bit color
			0, 0, 0, 0, 0, 0,                           // Don't care about these
			alphaBuffer, 0,                          // No alpha buffer
			0, 0, 0, 0, 0,                              // No accumulation buffer
			depthBuffer,                             // 32-bit depth buffer
			stencelBuffer,                           // No stencil buffer
			auxBuffer,                               // No auxiliary buffers
			PFD_MAIN_PLANE,                             // Layer type
			0,                                          // Reserved (must be 0)
			0, 0, 0                                     // No layer masks
	};

	int nPixelFormat;

	nPixelFormat = ChoosePixelFormat (hdc, &pfd);
	SetPixelFormat (hdc, nPixelFormat, &pfd);

	DescribePixelFormat (hdc, nPixelFormat, sizeof (PIXELFORMATDESCRIPTOR),&pfd);

	bufferMode = pfd.iPixelType;
	depthBuffer = pfd.cDepthBits;
	depthBuffer = pfd.cColorBits;
	stencelBuffer = pfd.cStencilBits;
	alphaBuffer = pfd.cAlphaBits;
	auxBuffer = pfd.cAuxBuffers; 

	if (pfd.dwFlags & PFD_NEED_PALETTE)
	{
		nColors = 1 << pfd.cColorBits;
		hHeap = GetProcessHeap ();

		(LPLOGPALETTE) lpPalette = (LPLOGPALETTE)HeapAlloc (hHeap, 0,
			sizeof (LOGPALETTE) + (nColors * sizeof (PALETTEENTRY)));

		lpPalette->palVersion = 0x300;
		lpPalette->palNumEntries = nColors;

		byRedMask = (1 << pfd.cRedBits) - 1;
		byGreenMask = (1 << pfd.cGreenBits) - 1;
		byBlueMask = (1 << pfd.cBlueBits) - 1;

		for (i=0; i<nColors; i++)
		{
			lpPalette->palPalEntry[i].peRed = (((i >> pfd.cRedShift) & byRedMask) * 255) / byRedMask;
			lpPalette->palPalEntry[i].peGreen = (((i >> pfd.cGreenShift) & byGreenMask) * 255) / byGreenMask;
			lpPalette->palPalEntry[i].peBlue = (((i >> pfd.cBlueShift) & byBlueMask) * 255) / byBlueMask;
			lpPalette->palPalEntry[i].peFlags = 0;
		}

		hPalette = CreatePalette (lpPalette);
		HeapFree (hHeap, 0, lpPalette);

		if (hPalette != NULL)
		{
			SelectPalette (hdc, hPalette, FALSE);
			RealizePalette (hdc);
		}
	}
}

void WinView::destroy ( void )
{

}



