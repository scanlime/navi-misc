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

#include "TeleporterDrawable.h"
#include "DrawablesManager.h"
#include "3dTypes.h"

TeleporterDrawable::TeleporterDrawable()
{
//	int	baseTexture;
//	int	fieldTexture;
//	int baseListID;
//	int fieldListID;
}

TeleporterDrawable::~TeleporterDrawable()
{

}

// for the visual element
bool TeleporterDrawable::init ( void )
{
	parent->getPos(pos);
	parent->getPos(rot);
	parent->getPos(size);
	parent->getInfoF("BORDER",border);
	return true;
}

bool TeleporterDrawable::visible ( BaseFrustum *frustum )
{
	return true;
}

void TeleporterDrawable::draw ( void )
{
	DrawablesManager::instance().add(this);
}

// or the drawables
bool TeleporterDrawable::draw ( int texture, int pass, int priority, void* param )
{
	glPushMatrix();
	CGLPoint	glPos(pos);
	glPos.glTranslate();
	gluSphere(gluNewQuadric(),100,12,12);
	glPopMatrix();
	return true;
}

