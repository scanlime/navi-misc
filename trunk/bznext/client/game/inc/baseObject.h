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

		// basic
		virtual bool GetPos ( float *pos ){return false;}
		virtual bool GetRot ( float *rot ){return false;}
		virtual bool GetScale ( float *scale ){return false;}
		virtual bool GetMatrix( float *matrix ){return false;}
		virtual bool GetType ( int *type ){return false;}
		virtual bool GetMaterial ( int item, char *material ){return false;}
		virtual bool GetMaterial (  const char * item, char *material ){return false;}
		virtual float GetValueF ( const char *item ){return 0;}
		virtual int GetValueI ( const char *item ){return 0;}
		virtual const char* GetValueS ( const char *item ){return NULL;}
protected:
};

#endif //_BASE_OBJECT_H_
