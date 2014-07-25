/* Firestarter
* testWorld.h :
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

#ifndef _TEST_WORLD_H_
#define _TEST_WORLD_H_

#include "networking.h"
#include <string>
#include <vector>

class CTestWorld
{
public:
	CTestWorld();
	virtual ~CTestWorld();

	virtual bool init ( const char* map );
	virtual void clear ( void );
	virtual void sendMapTo ( CNetworkPeer &peer );

	virtual	bool initDefaultWorld ( void );

	virtual float getScaleX ( void ) {return size[0];}
	virtual float getScaleY ( void ) {return size[1];}
protected:
	std::string		name;
	std::string		skybox;
	std::string		groundMaterial;
	std::string		wallMaterial;

	float			textureRepeat;
	float			wallHeight;
	float			size[2];

	float sunPos[3];
	float	sunColor[3];
	float ambientColor[3];

	typedef struct 
	{
		std::string mesh;
		int			count;
		float		center[2];
		float		range;
		float		scale[2];
	}trFlairDef;

	std::vector<trFlairDef> flairGroups;
};

#endif// _TEST_WORLD_H_