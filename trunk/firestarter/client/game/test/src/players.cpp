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
// players.cpp

#include "players.h"
#include <vector>
#include "timer.h"

CPlayerObject::CPlayerObject()
{
	drawable = -1;
	idNumber = -1;
	active = false;
	pos[0] = pos[1] = pos[2] = 0;
	rot[0] = rot[1] = rot[2] = 0;
	vec[0] = vec[1] = vec[2] = 0;
	updateTime = 0;
	material = "RedkMK3";
}

CPlayerObject::~CPlayerObject()
{
	Kill();
	active = false;
}

void CPlayerObject::Init ( bool draw )
{
	Kill();

	if (material.size() ==0)
		material = "RedkMK3";

	if (draw)
		drawable = CDrawManager::instance().New("playerTank",this);
}

bool CPlayerObject::Think ( void )
{
	// do some dead reco if we didn't just get this update

	//if (updateTime != CTimer::instance().GetTime())
	{
		float updateTime = CTimer::instance().GetFrameTime();
		pos[0] += vec[0]*updateTime;
		pos[1] += vec[1]*updateTime;
		pos[2] += vec[2]*updateTime;

		if (pos[2] < 0)
			pos[2] = 0;
	}
	return true;
}

void CPlayerObject::Kill ( void )
{
	if (drawable != -1)
		CDrawManager::instance().Delete(drawable);

	drawable = -1;
}

const char* CPlayerObject::GetValueS ( const char *item )
{
	std::string label = item;

	static char temp[32];

	if (label == "name")
	{
		sprintf(temp,"nettank-%d",idNumber);
		return temp;
	}

	if (label == "mesh")
		return "MK3.mesh";

	return NULL;
}

float CPlayerObject::GetValueF ( const char *item )
{
	return 0;
}

int CPlayerObject::GetValueI ( const char *item )
{
	return 0;
}

bool CPlayerObject::GetPos ( float *pPos )
{
	if (!pPos)
		return false;

	memcpy(pPos,pos,sizeof(float)*3);
	return true;
}

bool CPlayerObject::GetRot ( float *pRot )
{
	if (!pRot)
		return false;

	memcpy(pRot,rot,sizeof(float)*3);
	return true;
}

bool CPlayerObject::GetScale ( float *scale )
{
	return false;
}

bool CPlayerObject::GetMatrix( float *matrix )
{
	return false;
}

bool CPlayerObject::GetType ( int *type )
{
	return false;
}

bool CPlayerObject::GetMaterial ( int item, char *szMaterial )
{
	strcpy(szMaterial,material.c_str());
	return true;

}

bool CPlayerObject::GetMaterial (  const char * item, char *szMaterial )
{
	strcpy(szMaterial,material.c_str());
	return true;
}



