/* Firestarter
* baseObject.h :
*
* Copyright (C) 2004 Jeffrey Myers
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* email: jeffm2501@sbcglobal.net
*/


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
		virtual const char* GetValueS ( const char *item ){return 0;}

		virtual	bool Visible ( void ) { return true; }
protected:
};

#endif //_BASE_OBJECT_H_
