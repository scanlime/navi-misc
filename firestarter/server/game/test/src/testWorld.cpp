/* Firestarter
* testWorld.cpp :
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

#include "testWorld.h"
#include "osfile.h"
#include "messages.h"

CTestWorld::CTestWorld()
{
	name = "nomap";
	skybox = "default";
	groundMaterial = "default";
	wallMaterial = "default";

	wallHeight = 10.0f;
	size[0] = size[1] = 300.0f;
	textureRepeat = 80.0f;
}

bool CTestWorld::initDefaultWorld ( void )
{
	clear();

	name = "defaultWorld";
	skybox = "grassland_skybox";
	groundMaterial = "ground_mat";
	wallMaterial = "wall_mat";

	wallHeight = 5.0f;
	size[0] = size[1] = 400.0f;

	ambientColor[0] = ambientColor[1] = ambientColor[2] = 0.5f;
	sunPos[0] = sunPos[1] = sunPos[2] = 100;
	sunColor[0] = sunColor[1] = sunColor[2] = 0.75f;
	textureRepeat = 40;

	return true;
}

CTestWorld::~CTestWorld()
{	
	clear();
}

bool CTestWorld::init ( const char* mapFileName )
{
	COSFile	mapFile(mapFileName,"rt");

	if (!mapFile.IsOpen())
		return false;

	std::string line;

	//name: name
	name = &(mapFile.ReadLine()[6]);

	//sky: name
	skybox = &(mapFile.ReadLine()[5]);

	//ground: name
	groundMaterial = &(mapFile.ReadLine()[8]);

	//wall: name
	wallMaterial = &(mapFile.ReadLine()[6]);

	//wallZ: value
	wallHeight = (float)atof(&(mapFile.ReadLine()[6]));

	//size: x y
	sscanf(mapFile.ReadLine(),"size: %f %f",&size[0],&size[1]);

	//repeat: r
	textureRepeat = (float)atof(&(mapFile.ReadLine()[7]));

	//ambient: r g b
	sscanf(mapFile.ReadLine(),"ambient: %f %f %f",&ambientColor[0],&ambientColor[1],&ambientColor[2]);

	//sun: x y z r g b
	sscanf(mapFile.ReadLine(),"sun: %f %f %f %f %f %f",&sunPos[0],&sunPos[1],&sunPos[2],&sunColor[0],&sunColor[1],&sunColor[2]);

	// tuft groups
	int flairCount = 0;
	//tufts: n
	sscanf(mapFile.ReadLine(),"flairgroups: %d",&flairCount);
	
	trFlairDef	flair;
	int			temp;
	for(int i = 0; i < flairCount; i++)
	{
		//tuft: n
		sscanf(mapFile.ReadLine(),"flair: %d",&temp);

		//mesh: name
		flair.mesh = &(mapFile.ReadLine()[6]);

		//count: n
		sscanf(mapFile.ReadLine(),"count: %d",&flair.count);

		//center: x y
		sscanf(mapFile.ReadLine(),"center: %f %f",&flair.center[0],&flair.center[1]);

		//range: r
		sscanf(mapFile.ReadLine(),"range: %f",&flair.range);

		//scale: min max
		sscanf(mapFile.ReadLine(),"scale: %f %f",&flair.scale[0],&flair.scale[1]);

		flairGroups.push_back(flair);
	}
	mapFile.Close();

	return true;
}

void CTestWorld::clear ( void )
{
	flairGroups.clear();
}

void CTestWorld::sendMapTo ( CNetworkPeer &peer )
{
	CNetworkMessage	message;
	
	message.SetType(_MESSAGE_WORLD_INFO);
	message.AddStr(name.c_str());
	message.AddStr(skybox.c_str());
	message.AddStr(groundMaterial.c_str());
	message.AddStr(wallMaterial.c_str());
	message.AddF(wallHeight);
	message.AddF(textureRepeat);
	message.AddF(size[0]);
	message.AddF(size[1]);
	message.AddV(sunPos);
	message.AddV(sunColor);
	message.AddV(ambientColor);
	message.AddI((int)flairGroups.size());

	std::vector<trFlairDef>::iterator	itr = flairGroups.begin();
	while (itr != flairGroups.end())
	{
		message.AddStr(itr->mesh.c_str());
		message.AddI(itr->count);
		message.AddF(itr->center[0]);
		message.AddF(itr->center[1]);
		message.AddF(itr->range);
		message.AddF(itr->scale[0]);
		message.AddF(itr->scale[1]);
		itr++;
	}
	message.Send(peer,true);
}


