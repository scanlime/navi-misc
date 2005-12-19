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
	params.size[0] = 1000;
	params.size[1] = 1000;
}

World::~World()
{
}

void World::addAttribute (std::string &attribute, std::string &value)
{
	std::string name = TextUtils::toupper(attribute);

	if (name == "size") {
		std::vector<std::string> l = TextUtils::tokenize (value, std::string (","));
		if (l.size ()) {
			params.size[0] = (float) atof (l[0].c_str ());
			if (l.size() > 1)
				params.size[1] = (float) atof (l[1].c_str ());
			else
				params.size[1] = params.size[0];
		}
	} else if (name == "wallheight") {
		params.wallHeight = (float) atof (value.c_str ());
	} else if (name == "name") {
		params.name = value;
	}
}

int World::addMesh (std::string model)
{
	WorldObject object;

	object.type = model;

	objects.push_back(object);

	return (int) objects.size () - 1;
}

// todo: Dude do some checks for dupes
int World::addMaterial (const char* name)
{
	WorldMaterial	mat;

	mat.name = name;
	mat.ogreMat = true;

	materials.push_back (mat);
	return (int) materials.size () - 1;
}

int World::addMaterial (float color[3], float alpha, const char* texture, const char* name)
{
	WorldMaterial	mat;

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

	materials.push_back (mat);
	return (int) materials.size () - 1;
}

int World::findMaterialByName (const char* name)
{
	for (unsigned int i = 0; i < materials.size(); i++) {
		if (materials[i].name == std::string(name))
			return i;
	}
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

void World::clear (void)
{
	materials.clear ();
	objects.clear ();
}

