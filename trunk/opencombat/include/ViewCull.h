/* 3dScreamers
 * Copyright (c) 2002 - 2003 Jeffrey Myers
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named license.txt that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef _FCULL_H_
#define _FCULL_H_

#include "3dTypes.h"
#include <stdio.h>

// This is a horibly modified versiuon of the View Frustum tutorial by:
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com

class ViewPlane
{
public:
	ViewPlane();
	~ViewPlane();

	bool PointBehind ( float fX, float fY, float fZ );
	float PointDist ( float fX, float fY, float fZ );
	void Set ( float fA, float fB, float fC , float fD );

	float GetA ( void ){return m_rNorm.x;}
	float GetB ( void ){return m_rNorm.y;}
	float GetC ( void ){return m_rNorm.z;}
	float GetD ( void ){return m_fODist;}

	trVertex3D	m_rNorm;
	float		m_fODist;

	void Normalise();

private:
	bool	m_bSet;
};

typedef enum 
{
	eFrustRight	= 0,
	eFrustLeft,		
	eFrustBotom,	
	eFrustTop,		
	eFrustBack,		
	eFrustFront,
	eLastFrustSide
}eFrustumSides; 

class ViewFrustum 
{
public:
	ViewFrustum();
	~ViewFrustum();
	void Get();/// load frustum from GL
	bool PointIn(float fX, float fY, float fZ);
	bool SphereIn(float fX, float fY, float fZ, float fRad);
	bool CubeIn( float fX, float fY, float fZ, float fSize );
	bool BoxIn( float fX, float fY, float fZ ,float fXSize, float fYSize, float fZSize );

	bool QuadOut ( float *fp1, float *fp2, float *fp3, float *fp4 );

private:
	bool		m_bSet;
	ViewPlane m_aPlanes[eLastFrustSide];
};

#endif //_FCULL_H_
