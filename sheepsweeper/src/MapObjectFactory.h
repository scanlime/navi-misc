#ifndef _MAP_OBJECT_FACTORY_H_
#define _MAP_OBJECT_FACTORY_H_

#include "MapObject.h"
#include "MapTile.h"

class MapObjectFactory
{
	public:
		virtual MapObject *makeObject(MapObjectType type) = 0;
		virtual MapObject *makeSheep() = 0;
		virtual MapTile *makeTile(MapTileType type) = 0;

	protected:
		bool isValidObjectType(MapObjectType type) const
		{
			return (type >= 0 && type < MAP_OBJECT_TYPE_NUM);
		}

		bool isValidTileType(MapTileType type) const
		{
			return (type >= 0 && type < MAP_TILE_TYPE_NUM);
		}
};

#endif /* _MAP_OBJECT_FACTORY_H_ */
