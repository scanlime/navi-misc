/* Firestarter
* timer.cpp : 
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

#include "timer.h"
#include <SDL.h>

template <>
CTimer* Singleton<CTimer>::_instance = (CTimer*)0;

CTimer::CTimer()
{
	m_fThisTime = 0.0;
	m_fLastTime = 0.0;
	m_fTimeDelta = 0.0;

	m_liCurrentTime = 0;
	m_liFirstTime = 0;

	if(!(SDL_WasInit(SDL_INIT_EVERYTHING) & SDL_INIT_TIMER))
		SDL_InitSubSystem(SDL_INIT_TIMER);
}

CTimer::~CTimer()
{
}

void CTimer::Init()
{
	m_liCurrentTime = m_liFirstTime = SDL_GetTicks();
}

void CTimer::Update()
{
	m_fLastTime = m_fThisTime;
	m_liCurrentTime = SDL_GetTicks();
	m_fThisTime = static_cast<double>(m_liCurrentTime - m_liFirstTime) / 1000.0;
	m_fTimeDelta = m_fThisTime - m_fLastTime;
}

