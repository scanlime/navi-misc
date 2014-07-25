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

#include "common.h"
#include "bzfgl.h"
#include "ViewCull.h"

// view frustum class

ViewFrustumGL::ViewFrustumGL()
{
	m_bSet = false;
}

ViewFrustumGL::~ViewFrustumGL(){}

void ViewFrustumGL::Get()
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


