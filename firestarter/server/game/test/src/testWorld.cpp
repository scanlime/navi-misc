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
	name = &(mapFile.ReadLine()[5]);

	//sky: name
	skybox = &(mapFile.ReadLine()[4]);

	//ground: name
	groundMaterial = &(mapFile.ReadLine()[7]);

	//wall: name
	wallMaterial = &(mapFile.ReadLine()[5]);

	//wallZ: value
	wallHeight = (float)atof(&(mapFile.ReadLine()[6]));

	//size: x y
	sscanf(mapFile.ReadLine(),"size: %f %f",&size[0],&size[1]);

	// tuft groups
	int tufCount = 0;
	//tufts: n
	sscanf(mapFile.ReadLine(),"tufts: %d",tufts);
	
	trTuftDef	tuft;
	int			temp;
	for(int i = 0; i < tufCount; i++)
	{
		//tuft: n
		sscanf(mapFile.ReadLine(),"tuft: %d",&temp);

		//mesh: name
		tuft.mesh = &(mapFile.ReadLine()[5]);

		//count: n
		sscanf(mapFile.ReadLine(),"count: %d",&tuft.count);

		//center: x y
		sscanf(mapFile.ReadLine(),"center: %f %f",&tuft.center[0],&tuft.center[1]);

		//range: r
		sscanf(mapFile.ReadLine(),"range: %d",&tuft.range);

		tufts.push_back(tuft);
	}
	mapFile.Close();


	return true;
}

void CTestWorld::clear ( void )
{
	tufts.clear();
}

void CTestWorld::sendMapTo ( CNetworkPeer &peer )
{
	CNetworkMessage	message;
	
	message.SetType(_MESSAGE_WORLD_INFO);
	message.AddStr(name.c_str());
	message.AddStr(skybox.c_str());
	message.AddStr(groundMaterial.c_str());
	message.AddF(wallHeight);
	message.AddF(size[0]);
	message.AddF(size[1]);
	message.AddI((int)tufts.size());

	std::vector<trTuftDef>::iterator	itr = tufts.begin();
	while (itr != tufts.end())
	{
		message.AddStr(itr->mesh.c_str());
		message.AddI(itr->count);
		message.AddF(itr->center[0]);
		message.AddF(itr->center[1]);
		message.AddF(itr->range);
		itr++;
	}
	message.Send(peer,true);
}


