/* Firestarter
* syncedClock.cpp :
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


#include "syncedClock.h"

template <>
CSyncedClock* Singleton<CSyncedClock>::_instance = (CSyncedClock*)0;


CSyncedClock::CSyncedClock():timer(CTimer::instance())
{
	/*
		float		serverOffset;
		std::map<int,float>	syncPingMap;
		int			sentPings;	*/
	
}

CSyncedClock::~CSyncedClock()
{

}

void CSyncedClock::Init()
{

}

void CSyncedClock::Update()
{

}

float CSyncedClock::GetTime()
{
	return 0;
}

float CSyncedClock::GetFrameTime()
{
	return 0;
}

int CSyncedClock::GetNewSyncPing ( void )
{
	return 0;
}

void CSyncedClock::ReturnSyncPing ( int ping, float value )
{

}

float CSyncedClock::GetServerPingLoss ( void )
{
	return 0;
}

