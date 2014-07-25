/*
 * WorldParser.cpp - parses worlds, dummy
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

#include <vector>
#include "tinyxml.h"
#include "WorldParser.h"
#include "TextUtils.h"

typedef struct
{
	std::string name;
	std::string value;
} WorldAttribute;

WorldParser::WorldParser (World &w): world (w)
{
}

WorldParser::~WorldParser ()
{
}

bool WorldParser::load (const char* mapFile)
{
	TiXmlDocument doc (mapFile);

	if (!doc.LoadFile ())
		return false;

	TiXmlNode *node = 0;
	TiXmlNode *rootNode = doc.FirstChild ("world");

	if (!rootNode)
		return false;

	TiXmlElement* itemElement = rootNode->ToElement ();

	if (!itemElement)
		return false;

	TiXmlAttribute *itemAttribute = itemElement->FirstAttribute ();

	while (itemAttribute) {
		WorldAttribute attrib;

		attrib.name = itemAttribute->Name ();
		attrib.value = itemAttribute->Value ();

		world.addAttribute (attrib.name,attrib.value);

		itemAttribute = itemAttribute->Next ();
	}

	node = rootNode->FirstChild ();
	std::string nodeName;

	while (node) {
		loadObject (node);
		node = node->NextSibling ();
	}

	return true;
}

void WorldParser::loadObject (void * _node)
{
	TiXmlNode* node = (TiXmlNode*) _node;
	if (!node)
		return;

	std::string nodeType = TextUtils::tolower (node->Value ());

	if (nodeType == "mesh")
		parseObject (node);
	if (nodeType == "texture")
		parseTexture (node);
}

int WorldParser::parseObject (void *_node)
{
	TiXmlNode* node = (TiXmlNode*) _node;
	if (!node)
		return -1;

	TiXmlElement* itemElement = node->ToElement ();

	TiXmlAttribute *itemAttribute = itemElement->FirstAttribute ();

	std::vector<WorldAttribute> attribs;
	std::string type;

	while (itemAttribute) {
		WorldAttribute	attrib;

		attrib.name = TextUtils::tolower (itemAttribute->Name ());
		attrib.value = itemAttribute->Value ();

		if (attrib.name == "model")
			type = attrib.value;
		else
			attribs.push_back (attrib);

		itemAttribute = itemAttribute->Next ();
	}

	if (!type.size())
		return -1;

	int objectID = world.addMesh (type);

	for (unsigned int i = 0; i < attribs.size (); i++) {
		float v[3] = {0};
		if (attribs[i].name == "position") {
			sscanf (attribs[i].value.c_str (), "%f,%f,%f", &v[0], &v[1], &v[2]);
			world.setObjectPos (objectID, v);
		} else if (attribs[i].name == "rotation") {
			sscanf (attribs[i].value.c_str (), "%f,%f,%f", &v[0], &v[1], &v[2]);
			world.setObjectRot (objectID, v);
		} else if (attribs[i].name == "scale") {
			sscanf (attribs[i].value.c_str (), "%f,%f,%f", &v[0], &v[1], &v[2]);
			world.setObjectScale (objectID, v);
		} else {
			world.setObjectAttribute (objectID, attribs[i].name, attribs[i].value);
		}
	}

	// check for any child objects
	TiXmlNode* childNode = node->FirstChild ();

	while (childNode) {
		std::string nodeType = TextUtils::tolower (childNode->Value ());

		if (nodeType == "material")
			world.addObjectMaterial (objectID, parseTexture (childNode));

		if (nodeType == "matref")
			world.addObjectMaterial (objectID, parseMatRef (childNode));

		childNode = childNode->NextSibling ();
	}
	return 1;
}

int WorldParser::parseTexture (void *_node)
{
	TiXmlNode* node = (TiXmlNode*) _node;

	if (!node)
		return -1;

	TiXmlElement* itemElement = node->ToElement ();
	TiXmlAttribute *itemAttribute = itemElement->FirstAttribute ();
	std::string name;
	std::string color;
	std::string alpha;
	std::string image;

	while (itemAttribute) {
		WorldAttribute attrib;

		attrib.name = TextUtils::tolower (itemAttribute->Name ());
		attrib.value = itemAttribute->Value ();

		if (attrib.name == "color")
			color = attrib.value;
		if (attrib.name == "alpha")
			alpha = attrib.value;
		if (attrib.name == "image")
			image = attrib.value;
		if (attrib.name == "name")
			name = attrib.value;

		itemAttribute = itemAttribute->Next ();
	}

	// we have to have something
	if ((color.size() == 0) && (image.size() == 0)) {
		if (name.size())	// it's a ogre material
			return world.addMaterial (name.c_str ());
		else
			return -1;
	}

	float c[3] = {1, 1, 1};
	float a = 1;

	if (color.size())
		sscanf (color.c_str (), "%f,%f,%f", &c[0], &c[1], &c[2]);
	if (alpha.size())
		a = (float) atof (alpha.c_str ());

	return world.addMaterial (c, a, image.size() ? image.c_str () : NULL, name.size () ? name.c_str () : NULL );
}

int WorldParser::parseMatRef (void *_node)
{
	TiXmlNode* node = (TiXmlNode*) _node;

	if (!node)
		return -1;

	TiXmlElement* itemElement = node->ToElement ();
	TiXmlAttribute *itemAttribute = itemElement->FirstAttribute ();
	std::string name;

	while (itemAttribute) {
		WorldAttribute attrib;

		attrib.name = TextUtils::tolower (itemAttribute->Name ());
		attrib.value = itemAttribute->Value ();

		if (attrib.name == "name")
			name = attrib.value;

		itemAttribute = itemAttribute->Next ();
	}

	// we have to have something
	if (name.size () == 0)
		return -1;

	return world.findMaterialByName (name.c_str ());
}
