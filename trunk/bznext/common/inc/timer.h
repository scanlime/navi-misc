/*
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


#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef _WIN32
#include <windows.h>
#endif

class CTimer
{
public:
	CTimer();
	~CTimer();

	void		Init();
	void		Update();
	double		GetTime() { return m_fThisTime; }
	double		GetFrameTime() { return m_fTimeDelta; }

private:
	unsigned int	m_liCurrentTime;
	unsigned int	m_liFirstTime;

	double		m_fThisTime;
	double		m_fLastTime;
	double		m_fTimeDelta;
};

void SetTimer ( CTimer *pTime );
CTimer* GetTimer ( void );

#endif
