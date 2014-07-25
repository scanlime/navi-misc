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
// world.h

#ifndef _TEST_WORLD_H_
#define _TEST_WORLD_H_

#include "baseObject.h"
#include <string>
#include <vector>
#include <map>
#include "networking.h"

typedef struct 
{
	std::string	name;
	std::string skybox;
	std::string groundTexture;
	std::string wallTexture;
	float groundSize[2];
	float groundTextureRepeat;
	float wallSize;
	float sunPos[3];
	float	sunColor[3];
	float ambientColor[3];
}trWorldInfo;

typedef	 struct 
{
	float pos[3];
	float rot[3];
	float scale[3];
	std::string type;
	std::vector<std::string> textures;
}trObjectInfo;

typedef struct 
{
	std::string mesh;
	int					count;
	float				center[2];
	float				range;
	float				scale[2];
}trFlairDef;

class CTestWorld : public CBaseObject
{
	public:
		CTestWorld();
		~CTestWorld();
		
		void Load ( trWorldInfo &info, bool draw );	
		void Load ( CNetworkMessage &message, bool draw );	

		void AddWorldObject ( trObjectInfo &info );
		void AddTuft ( trFlairDef &tuft );

		bool Think ( void );

		// functions for the world drawable to call
		const char* GetValueS ( const char *item );
		float GetValueF ( const char *item );
		int GetValueI ( const char *item );

		float GetXSize ( void ) {return worldInfo.groundSize[0];}
		float GetYSize ( void ) {return worldInfo.groundSize[1];}
		// other stuff here for all the other world things that we need to do.

	protected:
		int											worldDrawables[3];
		trWorldInfo									worldInfo;
		std::vector<trFlairDef>						flairGroups;
};

#endif //_TEST_WORLD_H_