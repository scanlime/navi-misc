/* Firestarter
* events.cpp : basic event handling
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
* email: jeffm@gibbscam.com
*/

#ifdef _WIN32
	#pragma warning( disable : 4786 )  // Disable warning message
	#include <Windows.h>
#else
	#define stricmp strcasecmp
#endif

#include "events.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>

void OSSleep ( float time )
{
#ifdef _WIN32
	Sleep((DWORD)(1000.0f * time));
#else
	usleep((unsigned int )(100000 * time));
#endif
}

typedef struct
{
	tEventHandlerCallBack	callback;
	void *					param;
}trEventHandler;

typedef std::vector<trEventHandler> tvEventList;
typedef std::map<unsigned char,tvEventList> tvEventMap;

tvEventMap		theEventMap;

void RegisterEventHandler ( unsigned char event, tEventHandlerCallBack callback, void *param )
{
	tvEventMap::iterator	itr = theEventMap.find(event);

	if (itr == theEventMap.end())
	{
		tvEventList		list;
		theEventMap[event] = list;
	}

	trEventHandler handler;
	
	handler.callback = callback;
	handler.param = param;

	theEventMap[event].push_back(handler);
}

void RemoveEventHandler ( unsigned char event, tEventHandlerCallBack callback, void *param )
{
	if (theEventMap.find(event) == theEventMap.end())
		return;

	tvEventList::iterator itr = theEventMap[event].begin();

	while (itr != theEventMap[event].end())
	{
		if (callback == itr->callback && param == itr->param)
		{
			theEventMap[event].erase(itr);
			return;
		}
		itr++;
	}
}

// only used by gameloop
void CallEventHandler ( unsigned char event, void* message )
{
	if (theEventMap.find(event) == theEventMap.end())
		return;

	tvEventList::iterator itr = theEventMap[event].begin();

	while (itr != theEventMap[event].end())
	{
		(*itr->callback)(event,message,itr->param);
		itr++;
	}
}
