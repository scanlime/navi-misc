/* bzflag
* Copyright (c) 1993 - 2004 Tim Riker
*
* This package is free software;  you can redistribute it and/or
* modify it under the terms of the license found in the file
* named COPYING that should have accompanied this file.
*
* THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/
// 3dtypes.h

#ifndef _3DTYPES_H_
#define _3DTYPES_H_

#include "GeoUtils.h"

typedef struct
{
	float	r,g,b;
}trColor;

// new color class
class CColor
{
	public:
		CColor();
		CColor(trColor &rColor);
		CColor(float r,float g, float b, float a = 1.0f);
		~CColor();

		void Set (trColor &rColor);
		void Set ( float r,float g, float b, float a = 1.0f);
		void SetAlpha ( float a = 1.0f) {m_fAlpha = a;}

		float Red (void);
		float Green (void);
		float Blue (void);
		float Alpha (void);

		void glColor (void);
		void glColor ( float alpha ); // same as glColor, just use this alpha not the one in the item

		operator trColor& (void); 
		operator const trColor& (void);

	protected:
		float	m_fRed;
		float	m_fGreen;
		float	m_fBlue;
		float	m_fAlpha;

		trColor	m_rColor;
};

// 3D Point class ( openGL enabled )
class CGLPoint :public C3DVertex
{
	public:
		CGLPoint();
		CGLPoint( const C3DVertex& r);
		CGLPoint( const CGLPoint &rVert );
		CGLPoint( float x , float y, float z = 0);
		CGLPoint( float *pos );
		virtual ~CGLPoint();

		// operators
		CGLPoint& operator = ( const C3DVertex& r);
		CGLPoint& operator = ( const CGLPoint& r);
		CGLPoint& operator = (const trVertex3D &p0);
		CGLPoint operator - (const CGLPoint &p0) const;
		CGLPoint operator + (const CGLPoint &p0) const;

		void operator -= (const CGLPoint &p0);
		void operator += (const CGLPoint &p0);
		void operator *= (const CGLPoint &p0);
		void operator /= (const CGLPoint &p0);

		// gl functions
		void glNormal (void) const;
		void glVertex (void) const;
		void glTranslate (void) const;
};

// matrix class that uses GL

class CGLMatrix : public CMatrix
{
	public:
		void LoadFromGL( void );
		void SetToGL ( void );
};

#endif//_3DTYPES_H_
