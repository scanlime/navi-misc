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

#include "3DView.h"
#include "common.h"
#include "bzfgl.h"

/*const int NO_VARIANT = (-1); */

// initialize the singleton
template <>
View3D* Singleton<View3D>::_instance = (View3D*)0;

View3D::View3D()
{	
	lastFOV = 0;
	lastH = 0;
	lastV = 0;
	lastHither = 0.00001f;
	lastYon = 10000.0f;
	pushed = false;
	inOrtho = false;

	glClearColor(0.25f, 0.5f, 0.25f, 1.0f);
	glClearDepth(1.0f);									// Depth Buffer Setup
}

View3D::~View3D()
{
	if (inOrtho)
		unsetOrtho();

	if (pushed)
		untransform();
}

void View3D::setViewPort ( int h, int v )
{
	if ( (h == lastH) && (v == lastV) )
		return;

	glViewport(0,0,h,v);
	lastH = h;
	lastV = v;
}

void View3D::setFOV ( float fov )
{
	if ( lastFOV == fov)
		return;
	
	float aspect = 1;
	if ( lastV >0)
		aspect = (float)lastH/(float)lastV;

	float realFOV = fov/aspect;
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(realFOV,aspect,lastHither, lastYon);
	glMatrixMode(GL_MODELVIEW);

	lastFOV = fov;
}

void View3D::setView ( C3DVertex pos, C3DVertex target )
{
	camera = pos;
	viewTarget = target;
}

void View3D::setViewDist ( float hither, float yon )
{
	if ( (lastHither == hither) && (lastYon == yon) )
		return;

	float aspect = 1;
	if ( lastV >0)
		aspect = (float)lastH/(float)lastV;

	float realFOV = lastFOV/aspect;

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(realFOV,aspect,hither, yon);
	glMatrixMode(GL_MODELVIEW);

	lastHither = hither;
	lastYon = yon;
}

void View3D::set ( C3DVertex pos, C3DVertex target, float fov )
{
	setFOV(fov);
	setView(pos,target);
}

void View3D::set ( float* pos, float* target, float fov )
{
	set(C3DVertex(pos),C3DVertex(target),fov);
}

void View3D::transform ( void )
{
	if (pushed)
		untransform();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	pushed = true;

	// set the view matrix to go from the eye point TO the target point;
	gluLookAt(camera.X(),camera.Y(),camera.Z(),viewTarget.X(),viewTarget.Y(),viewTarget.Z(),0,0,1);

	// save the curent view into the frustum;
	Get();
}

void View3D::untransform ( void )
{
	if (!pushed)
		return;

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glLoadIdentity();
	pushed = false;
}

void View3D::setOrtho ( void )
{
	if (!pushed)
		return;

	if (inOrtho)
		unsetOrtho();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0,lastH,0,lastV,0.1,100);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	inOrtho = true;
}

void View3D::unsetOrtho ( void )
{
	if (!inOrtho || !pushed)
		return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float aspect = 1;
	if ( lastV >0)
		aspect = (float)lastH/(float)lastV;

	float realFOV = lastFOV/aspect;
	gluPerspective(realFOV,aspect,lastHither, lastYon);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	inOrtho = false;
}

void View3D::clearFrameBuffer ( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}



