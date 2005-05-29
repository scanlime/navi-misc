#ifndef _MAP_TILE_H_
#define _MAP_TILE_H_

#include "MapObject.h"

typedef enum
{
	MAP_TILE_BLANK = 0,
	MAP_TILE_GRASS_GREEN,
	MAP_TILE_GRASS_BROWN,
	MAP_TILE_TYPE_NUM

} MapTileType;

class MapTile : public MapObject
{
	public:
		MapTile();
		virtual ~MapTile();

		virtual int getHeight() const;

		void setTileType(MapTileType type);
		MapTileType getTileType() const;

	private:
		MapTileType tileType;
};

#endif /* _MAP_TILE_H_ */
