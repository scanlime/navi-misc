/* 
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
// implementation for view cull classes


#include "OpenGL.h"
#include "ViewCull.h"

// plane class implementation;
ViewPlane::CViewPlane()
{
	m_rNorm.x = m_rNorm.y = m_rNorm.z = m_fODist = 0;
	m_bSet = false;
}

ViewPlane::~CViewPlane()
{
	m_rNorm.x = m_rNorm.y = m_rNorm.z = m_fODist = 0;
}

void ViewPlane::Set ( float fA, float fB, float fC , float fD )
{
	m_bSet = true;
	m_rNorm.x = fA;
	m_rNorm.y = fB;
	m_rNorm.z = fC;
	m_fODist = fD;
	Normalise();
}

void ViewPlane::Normalise ( void )
{
	m_fODist /= get_mag(m_rNorm);
	set_mag(m_rNorm,1.0f);
}

float ViewPlane::PointDist ( float fX, float fY, float fZ )
{
	if (!m_bSet)
		return 0.0f;

	return GetA() * fX + GetB() * fY + GetC() * fZ + GetD();
}

bool ViewPlane::PointBehind ( float fX, float fY, float fZ )
{
	if (!m_bSet)
		return false;

	return PointDist(fX,fY,fZ) <= 0.0f;
}

// view frustum class

ViewFrustum::CViewFrustum()
{
	m_bSet = false;
}

ViewFrustum::~CViewFrustum(){}

void ViewFrustum::Get()
{    
	float   proj[16];								// projection matrix
	float   modl[16];								// modelview matrix
	float   clip[16];								// clipping planes

	m_bSet = true;

	// get projection and modlel view matrixes and mull them
	glGetFloatv( GL_PROJECTION_MATRIX, proj );
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	// todo: Convert to trMatrix

	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];
	
	// extract planes from  gl matrix
	m_aPlanes[eFrustRight].Set(clip[3]-clip[0],clip[7]-clip[4],clip[11]-clip[8],clip[15]-clip[12]);
	m_aPlanes[eFrustLeft].Set(clip[3]+clip[0],clip[7]+clip[4],clip[11]+clip[8],clip[15]+clip[12]);
	m_aPlanes[eFrustBotom].Set(clip[3]+clip[1],clip[7]+clip[5],clip[11]+clip[9],clip[15]+clip[13]);
	m_aPlanes[eFrustTop].Set(clip[3]-clip[1],clip[7]-clip[5],clip[11]-clip[9],clip[15]-clip[13]);
	m_aPlanes[eFrustBack].Set(clip[3]-clip[2],clip[7]-clip[6],clip[11]-clip[10],clip[15]-clip[14]);
	m_aPlanes[eFrustTop].Set(clip[3]+clip[2],clip[7]+clip[6],clip[11]+clip[10],clip[15]+clip[14]);
}

bool ViewFrustum::PointIn( float fX, float fY, float fZ )
{
	if (!m_bSet)
		return false;

	for(int i = eFrustRight; i < eLastFrustSide; i++ )
	{
		if(m_aPlanes[i].PointBehind(fX,fY,fZ))
			return false;
	}
	return true;	// The point was inside of the frustum (In front of ALL the sides of the frustum)
}

bool ViewFrustum::SphereIn( float fX, float fY, float fZ, float fRad )
{
	if (!m_bSet)
		return false;

	for(int i = eFrustRight; i < eLastFrustSide; i++ )
	{
		// If the center of the sphere is farther away from the plane than the radius
		if( m_aPlanes[i].PointDist(fX,fY,fZ) <= -fRad )
			return false;
	}
	return true;// The sphere was inside of the frustum!
}

bool ViewFrustum::CubeIn( float fX, float fY, float fZ, float fSize )
{
	if (!m_bSet)
		return false;

	return BoxIn(fX,fY,fZ,fSize,fSize,fSize);
}

bool ViewFrustum::BoxIn( float fX, float fY, float fZ ,float fXSize, float fYSize, float fZSize )
{
	if (!m_bSet)
		return false;

	float fXHalfSize = fXSize *0.5f;
	float fYHalfSize = fYSize *0.5f;
	float fZHalfSize = fZSize *0.5f;

	for(int i = eFrustRight; i < eLastFrustSide; i++ )
	{
		if (!m_aPlanes[i].PointBehind(fX-fXHalfSize,fY-fYHalfSize,fZ-fZHalfSize))
		   continue;

		if (!m_aPlanes[i].PointBehind(fX+fXHalfSize,fY-fYHalfSize,fZ-fZHalfSize))
		   continue;

		if (!m_aPlanes[i].PointBehind(fX+fXHalfSize ,fY + fYHalfSize,fZ-fZHalfSize))
		   continue;

		if (!m_aPlanes[i].PointBehind(fX-fXHalfSize ,fY + fYHalfSize,fZ-fZHalfSize))
		   continue;

		if (!m_aPlanes[i].PointBehind(fX-fXHalfSize,fY-fYHalfSize,fZ+fZHalfSize))
		   continue;

		if (!m_aPlanes[i].PointBehind(fX+fXHalfSize,fY-fYHalfSize,fZ+fZHalfSize))
		   continue;

		if (!m_aPlanes[i].PointBehind(fX+fXHalfSize ,fY + fYHalfSize,fZ+fZHalfSize))
		   continue;

		if (!m_aPlanes[i].PointBehind(fX-fXHalfSize ,fY + fYHalfSize,fZ+fZHalfSize))
		   continue;

		return false;
	}
	return true;
}

bool ViewFrustum::QuadOut ( float *fp1, float *fp2, float *fp3, float *fp4 )
{
	if (!m_bSet)
		return false;

	if (!fp1 || !fp2 || !fp3 || !fp4)
		return false;

	bool	abPoints[4];

	for(int i = eFrustRight; i < eLastFrustSide; i++ )
	{
		abPoints[0] = m_aPlanes[i].PointBehind(fp1[0],fp1[1],fp1[2]);
		abPoints[1] = m_aPlanes[i].PointBehind(fp2[0],fp2[1],fp2[2]);
		abPoints[2] = m_aPlanes[i].PointBehind(fp3[0],fp3[1],fp3[2]);
		abPoints[3] = m_aPlanes[i].PointBehind(fp4[0],fp4[1],fp4[2]);

		if (abPoints[0] && abPoints[1] && abPoints[2] && abPoints[3])
			return true;
	}
	return false;
}


