#ifndef _OPENGL_H_
#define _OPENGL_H_

#ifdef _WIN32 // this file only has windows stuff
	#include <windows.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#else
	#ifdef __APPLE__
		#include <Carbon/Carbon.h>
	#endif	// apple
#endif // _WIN32

inline float getFloatColor ( int val )
{
	return val/255.0f;
}

inline void setColor ( float c[3], int r, int g, int b )
{
	c[0] = getFloatColor(r);
	c[1] = getFloatColor(g);
	c[2] = getFloatColor(b);
}

inline void glSetColor ( float c[3], float alpha = 1.0f )
{
	glColor4f(c[0],c[1],c[2],alpha);
}

inline void glTranslatefv ( float v[3] )
{
	glTranslatef(v[0],v[1],v[2]);
}

void glSetTextureScale ( float du, float dv );
void glResetTextureScale ( void );
void glDrawSolidBox ( float size[3] );
void glDrawSolidBox ( float x, float y, float z );
void glDrawOutlineBox ( float size[3], float lineWidth );

void glSetPolygonOffSet ( float offset );
void glResetPolygonOffset ( void );

void glCapedCylinder ( float topRadius, float bottomRadius, int segments, float height );
void glCylinder ( float topRadius, float bottomRadius, int segments, float height );

void glSphere ( float radius, int segments );

#endif //_OPENGL_H_