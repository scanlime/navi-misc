/*
 * World.cpp - world data
 *
 * Copyright (C) 2005 Screamers Group (see AUTHORS)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "World.h"

World::World()
{
}

World::~World()
{
}

void World::addAttribute (std::string &attribute, std::string &value)
{
}

int World::addPrim (std::string type)
{
	return -1;
}

int World::addMesh (std::string model)
{
	return -1;
}

void World::setObjectAttribute (int object, std::string &attribute, std::string &value)
{
}

void World::setObjectPos (int object, float p[3])
{
}

void World::setObjectRot (int object, float r[3])
{
}

void World::setObjectScale (int object, float s[3])
{
}

void World::addObjectMaterial (int object, int materialID)
{
}

// todo: Dude do some checks for dupes
int World::addMaterial (const char* name)
{
	Material	mat;

	mat.name = name;
	mat.ogreMat = true;

	materials.push_back(mat);
	return (int)materials.size()-1;
}

int World::addMaterial (float color[3], float alpha, const char* texture, const char* name)
{
	Material	mat;

	if (name)
		mat.name = name;
	//	else
	//		mat.name << materials.size();

	mat.ogreMat = false;
	mat.c[0] = color[0];
	mat.c[1] = color[1];
	mat.c[2] = color[2];
	mat.c[3] = alpha;

	if (texture)
		mat.image = texture;

	materials.push_back(mat);
	return (int)materials.size()-1;
}

int World::findMaterialByName (const char* name)
{
	for (unsigned int i = 0; i < materials.size(); i++)
	{
		if (materials[i].name == std::string(name))
			return i;
	}
	return -1;
}
