#include "MapTile.h"

MapTile::MapTile()
	: MapObject(), tileType(MAP_TILE_BLANK)
{
}

MapTile::~MapTile()
{
}

int
MapTile::getHeight() const
{
	return 0;
}

void
MapTile::setTileType(MapTileType type)
{
	tileType = type;
}

MapTileType
MapTile::getTileType() const
{
	return tileType;
}
