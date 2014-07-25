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
	float					serverOffset;
	float					lastPing;
	std::map<int,float>		syncPingMap;
	int						sentPings;
	CTimer					&timer;		
	*/

	sentPings = 0;
	serverOffset = 0;
	lastPing = 0;
}

CSyncedClock::~CSyncedClock()
{
	syncPingMap.clear();
}

void CSyncedClock::Init()
{
	timer.Init();
}

void CSyncedClock::Update()
{
	timer.Update();
}

float CSyncedClock::GetTime()
{
	return (float)(timer.GetTime()+serverOffset);
}

float CSyncedClock::GetFrameTime()
{
	return (float)timer.GetFrameTime();
}

int CSyncedClock::GetNewSyncPing ( void )
{
	int pingID = sentPings++;
	syncPingMap[pingID] = (float)(timer.GetTime());
	return pingID;
}

void CSyncedClock::ReturnSyncPing ( int ping, float value )
{
	if (value == 0)
		return;

	float now = (float)timer.GetTime();
	std::map<int,float>::iterator itr = syncPingMap.find(ping);
	if (itr == syncPingMap.end())
		return;

	float delta = (now - itr->second) * 0.5f;
	float serverNow = value + delta;

	serverOffset = serverNow - now;
	lastPing = delta;

	// clear the ping
	syncPingMap.erase(itr);
}

float CSyncedClock::GetServerPingLoss ( void )
{
	return ((float)(syncPingMap.size())-1.0f)/(float)sentPings;
}

float CSyncedClock::GetLastPingTime ( void )
{
	return lastPing;
}


