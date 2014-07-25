/* bzflag
* Copyright (c) 1993 - 2004 Tim Riker
*
* This package is free software;  you can redistribute it and/or
* modify it under the terms of the license found in the file
* named COPYING that should have accompanied this file.
*
* THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

// display manager

#ifndef _DISPLAY_MANANGER_H_
#define _DISPLAY_MANANGER_H_

#include "Singleton.h"

class DisplayManager : public Singleton<DisplayManager>
{
public:
	DisplayManager();
	~DisplayManager();

	void init ( void );

	// resoultion enumeration
	int getResCount ( void );
	std::string	getResName ( int res );
	
	int getBPPCount ( void );
	std::string	getBPPName ( int BPP );

	// resolution switching
	bool setRes ( int res, int BPP, bool fullScreen );
	bool setRes ( int h, int v, int BPP, bool fullScreen );
	bool setRes ( std::string res, std::string BPP, bool fullScreen );

	bool setFullScreen ( bool fullScreen );

	// info methods
	int getWidth ( void );
	int getHeight ( void );
	int getBPP( void );

	int getResMode ( void );
	int getBPPMode ( void );

	// state stuff
	void activate ( void );
	void deactivate ( void );

	void enable ( void );
	void yeld ( void );
	void release ( void );

	void flip ( void );

	// gamma
	void	setGamma ( float gamma );
	float	getGamma ( void );
protected:
	friend class Singleton<DisplayManager>;
};

#endif //_DISPLAY_MANANGER_H_