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
// baseObject.h


#ifndef _BASE_OBJECT_H_
#define _BASE_OBJECT_H_

class CBaseObject
{
public:
		CBaseObject(){return;}
		virtual ~CBaseObject(){return;}

		virtual bool GetPos ( float *pos ) = 0;
		virtual bool GetRot ( float *rot ) = 0;
		virtual bool GetScale ( float *scale ) = 0;
		virtual bool GetMatrix( float *matrix ) = 0;
		virtual bool GetType ( int *type ) = 0;
		virtual bool GetMaterial ( int item, char *material ) = 0;
protected:
};

#endif //_BASE_OBJECT_H_
