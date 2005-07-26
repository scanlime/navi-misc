#ifndef _WIN_VIEW_H_
#define _WIN_VIEW_H_

#ifdef _WIN32 // this file only has windows stuff
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Resource.h"

#include "mapedit.h"

class WinView : public EditView
{
public:
	WinView();
	virtual ~WinView();

	virtual void beginDraw ( void );
	virtual void endDraw ( void );

	virtual void setBGColor ( float r, float g, float b );

	virtual void size ( int x, int y );

	// windows stuff
	void init ( HWND _hWnd, HINSTANCE hInstance );
	void queryNewPallet ( void );
	void pallChanged ( void );
	void destroy ( void );
protected:

	HWND hWnd;
	HDC			hdc;
	HGLRC		hGLRC;
	HPALETTE	hPalette;

	void initGL ( long iX, long iY );
	void SetupPalette ( void );
	void SetupPixelFormat ( void );
	void RedoPalette ( void );
	void SetDepthRange ( float fNearZ, float xFarZ );


	int		viewportX;
	int		viewportY;
	float	fov;
	float	aspect;
	float	farZ;
	float	nearZ;

	// buffer info
	int			frameBuffer;
	int			depthBuffer;
	int			stencelBuffer;
	int			alphaBuffer;
	int			auxBuffer;
	int			bufferMode;

	float		bgColor[3];
};

#endif//_WIN32
#endif//_WIN_VIEW_H_