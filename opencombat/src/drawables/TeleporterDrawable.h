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

#include "DrawablesManager.h"
#include "VisualElementManager.h"

class TeleporterDrawable : public BaseVisualElement, BaseDrawable
{
	public:
	TeleporterDrawable();
	virtual ~TeleporterDrawable();

	// for the visual element
	virtual bool init ( void );
	virtual bool visible ( BaseFrustum *frustum );
	virtual void draw ( void );

	// or the drawables
	virtual bool draw ( int texture, int pass, int priority, void* param );
protected:
	int	baseTexture;
	int	fieldTexture;
	int baseListID;
	int fieldListID;

	C3DVertex	size;
	C3DVertex pos;
	C3DVertex rot;
	float			border;
};

class TeleporterDrawableFactory : public BaseVisualElementFactory
{
public:
		virtual BaseVisualElement* newElement ( void ){return new TeleporterDrawable;}
		virtual void deleteElement ( BaseVisualElement* element ){delete(element);}

};
