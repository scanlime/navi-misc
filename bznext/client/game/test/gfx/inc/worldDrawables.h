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
// worldDrawables.h

#ifndef _WORLD_DRAWABLES_H_
#define _WORLD_DRAWABLES_H_

#include "baseObject.h"

class CSkyObject : public CBaseDrawable
{
public:
	CSkyObject();
	virtual ~CSkyObject();
	virtual void Init ( void ) {return;}
	virtual void Think ( void ){return;}

protected:
};

#endif //_WORLD_DRAWABLES_H_
