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
// players.h

#ifndef _PLAYERS_H_
#define _PLAYERS_H_

#include "drawManager.h"
#include <vector>
#include <vector>
#include "baseObject.h"

class CPlayerObject : public CBaseObject
{
public:
	CPlayerObject();
	virtual ~CPlayerObject();

	void Init ( bool draw );
	bool Think ( void );
	void Kill ( void );

	// functions for the world drawable to call
	const char* GetValueS ( const char *item );
	float GetValueF ( const char *item );
	int GetValueI ( const char *item );

	bool GetPos ( float *pPos );
	bool GetRot ( float *pRot );
	bool GetScale ( float *scale );
	bool GetMatrix( float *matrix );

	bool GetType ( int *type );

	bool GetMaterial ( int item, char *szMaterial );
	bool GetMaterial (  const char * item, char *szMaterial );
	bool Visible ( void ) { return forceHidden ?  false :active; }

	int					idNumber;
	bool				active;
	std::string name;
	float				pos[3];
	float				rot[3];
	float				vec[3];	
	std::string	material;
	std::string mesh;

	bool				forceHidden;
	bool				dontUpdate;

	float				lastShotTime;
protected:
	int					drawable;
};

typedef std::map<int, CPlayerObject*> tmPlayerMap;

// shots
class CShotObject : public CBaseObject
{
public:
	CShotObject();
	virtual ~CShotObject();

	void Init ( bool draw );
	bool Think ( void );
	void Kill ( void );

	void Shoot ( int player, float *pos, float* rot, float speed, int shotClass, float startTime );

	// functions for the world drawable to call
	const char* GetValueS ( const char *item );
	float GetValueF ( const char *item );
	int GetValueI ( const char *item );

	bool GetPos ( float *pPos );
	bool GetRot ( float *pRot );

	float				pos[3];
	float				rot[3];
	float				vec[3];	

	std::string mesh;

protected:
	int					owner;
	int					drawable;
	float				lanchTime;
	float				timeToLive;
};

typedef std::vector<CShotObject*> tvShotList;


#endif //_PLAYERS_H_
