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
#ifndef _3DVIEW_H_
#define _3DVIEW_H_

#include "3dTypes.h"
#include "ViewCull.h"
#include "Singleton.h"

class View3D : public ViewFrustumGL, public Singleton<View3D>
{
public:
	View3D();
	virtual ~View3D();

	void setViewPort ( int h, int v );
	void setFOV ( float fov );
	void setView ( C3DVertex pos, C3DVertex target );
	void setViewDist ( float hither, float yon );

	void set ( C3DVertex pos, C3DVertex target, float fov );
	void set ( float* pos, float* target, float fov );

	void transform ( void );
	void untransform ( void );

	void setOrtho ( void );
	void unsetOrtho ( void );

	void clearFrameBuffer ( void );

	/*
	void setBillboard ( void );
	void unsetBillboard ( void );
	*/
protected:
	friend class Singleton<View3D>;

protected:
	float lastFOV;
	int		lastH;
	int		lastV;
	float lastHither;
	float	lastYon;
	bool	pushed;
	bool	inOrtho;
	C3DVertex camera;
	C3DVertex viewTarget;
};

#endif //_3DVIEW_H_
