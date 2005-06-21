/*
* worldParser.h - main entry point for the server
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

#include "tinyxml.h"
#include "worldParser.h"

WorldParser::WorldParser( World &w ): world(w)
{
}

WorldParser::~WorldParser()
{
}

bool WorldParser::load ( const char* mapFile )
{
	TiXmlDocument	doc(mapFile);

	if (!doc.LoadFile())
		return false;

	TiXmlNode* node = 0;
	TiXmlNode	*rootNode = doc.FirstChild("World");
	if (!rootNode)
		return false;

	TiXmlElement* itemElement = rootNode->ToElement();

	while (itemElement)
	{
		// see what we have here
		itemElement = itemElement->NextSiblingElement();
	}

	return true;
}
