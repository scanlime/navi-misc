/* Firestarter
* osfile.h :
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

#ifndef _FIRESTARTERD_H_
#define _FIRESTARTERD_H_

#include "listserver.h"

// server globals
extern trServerInfo		serverInfo;
extern CListServerServerConnection	serverListServerConnection;
extern bool registerAsPublic;

// functions
int errorOut ( const char * error, const char* place = NULL, int ret = 1 );

typedef enum
{
	eLogLevel0 = 0,	// no loging, silent mode
	eLogLevel1,		// basic loging
	eLogLevel2,		// loging of major messages
	eLogLevel3,		// loging of minor messages
	eLogLevel4,		// loging of debug level 1 class messages
	eLogLevel5,		// loging of debug level 2 class messages, aka EVERYTHING
}eLogLevel;

void logOut ( const char * error, const char* place, eLogLevel level = eLogLevel1 );

extern eLogLevel	eCurrentLogLevel;

#endif //_FIRESTARTERD_H_