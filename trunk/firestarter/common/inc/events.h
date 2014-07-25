/* Firestarter
* utils.h :
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

#ifndef _EVENTS_H_
#define _EVENTS_H_

// registration for handling of raw SDL Events
// param is some pointer passed by the handler that is passed back when the event is called
typedef int (*tEventHandlerCallBack)(unsigned char event, void* message, void *param);

void  RegisterEventHandler ( unsigned char event, tEventHandlerCallBack callback, void* param );
void  RemoveEventHandler ( unsigned char event, tEventHandlerCallBack callback, void* param ); 

// only used by gameloop
void CallEventHandler ( unsigned char event, void* message );


/** basic sleep method 
@note
equvelent to sleep on unix
@param time
time in seconds to sleep for
*/
void OSSleep ( float time );

#endif //_EVENTS_H_

