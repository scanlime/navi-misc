/* Firestarter
* messages.h :
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

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

// message types used in games

#define	_MESSAGE_SERVER_INFO 0x4953			// SI 
#define	_MESSAGE_CLIENT_INFO 0x4943			// CI 
#define	_MESSAGE_USER_PART 0x5055			// UP
#define	_MESSAGE_USER_ADD 0x4155			// UA
#define	_MESSAGE_KICK 0x4b4b				// KK
#define _MESSAGE_UPDATE 0x4455				//UD
#define _MESSAGE_ACKNOWLEDGE 0x4b41			//AK
#define _MESSAGE_SPAWN 0x5053				//SP
#define _MESSAGE_TIME_PING	 0x5054			//TP
#define _MESSAGE_WORLD_INFO	 0x4957			//WI
#define _MESSAGE_CREATE_SHOT	 0x5343			//CS

#endif //_MESSAGES_H_
