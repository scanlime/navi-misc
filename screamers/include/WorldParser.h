/*
 * WorldParser.h - parses worlds, dummy
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

#ifndef _WORLD_PARSER_H_
#define _WORLD_PARSER_H_

#include <string>
#include "World.h"

class WorldParser
{
public:
		 WorldParser (World &w);
		~WorldParser ();

	bool	 load (const char* mapFile);

protected:
	World	&world;

	void	 loadObject (void *_node);

	int	 parseObject (void *_node);
	int	 parseTexture (void *_node);
	int	 parseMatRef (void *_node);
};

#endif //_WORLD_PARSER_H_
