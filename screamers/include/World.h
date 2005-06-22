/*
 * World.h - world data
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

#ifndef _WORLD_H_
#define _WORLD_H_

#include <string>
#include <vector>

namespace Screamers
{

class World
{
public:
	World ();
	~World ();

	// all the stuff you can add
	void	 addAttribute (std::string &attribute, std::string &value);
	int	 addPrim (std::string type);
	int	 addMesh (std::string model);

	void	 setObjectAttribute (int object, std::string &attribute, std::string &value);
	void	 setObjectPos (int object, float p[3]);
	void	 setObjectRot (int object, float r[3]);
	void	 setObjectScale (int object, float s[3]);
	void	 addObjectMaterial (int object, int materialID);

	int	 addMaterial ( const char* name );
	int	 addMaterial (float color[3], float alpha = 1.0f, const char* texture = NULL, const char* name = NULL);
	int	 findMaterialByName (const char* name);

protected:

	typedef struct 
	{
		std::string name;
		bool		ogreMat;
		std::string image;
		float		c[4];
	}Material;

	typedef std::vector<Material>	MaterialList;

	MaterialList		materials;
};

};
#endif //_WORLD_H_
