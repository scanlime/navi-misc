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

// events.cpp

#ifdef _WIN32
	#pragma warning( disable : 4786 )  // Disable warning message
#else
	#define stricmp strcasecmp
#endif

#include "events.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>

typedef struct
{
	tEventHandalerCallBack	callback;
	void *					param;
}trEventHandaler;

typedef std::vector<trEventHandaler> tvEventList;
typedef std::map<unsigned char,tvEventList> tvEventMap;

tvEventMap		theEventMap;

void RegisterEventHandaler ( unsigned char event, tEventHandalerCallBack callback, void *param )
{
	tvEventMap::iterator	itr = theEventMap.find(event);

	if (itr == theEventMap.end())
	{
		tvEventList		list;
		theEventMap[event] = list;
	}

	trEventHandaler handaler;
	
	handaler.callback = callback;
	handaler.param = param;

	theEventMap[event].push_back(handaler);
}

void RemoveEventHandaler ( unsigned char event, tEventHandalerCallBack callback, void *param )
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
void CallEventHandaler ( unsigned char event, void* message )
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
