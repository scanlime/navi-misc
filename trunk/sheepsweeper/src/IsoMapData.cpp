#include "IsoMapData.h"

IsoMapData::IsoMapData(int width, int height)
	: MapData(width, height)
{
}

IsoMapData::~IsoMapData()
{
}

MapObject *
IsoMapData::getNeighbor(int x, int y, Direction dir)
{
	int tx = x, ty = y;

	if (dir == NORTH)
		ty -= 2;
	else if (dir == EAST)
		tx++;
	else if (dir == SOUTH)
		ty += 2;
	else if (dir == WEST)
		tx--;
	else
	{
		if (y % 2 == 0)
		{
			if (dir == NORTH_EAST)
				ty--;
			else if (dir == SOUTH_EAST)
				ty++;
			else if (dir == SOUTH_WEST)
			{
				tx--;
				ty++;
			}
			else if (dir == NORTH_WEST)
			{
				tx--;
				ty--;
			}
		}
		else
		{
			if (dir == NORTH_EAST)
			{
				tx++;
				ty--;
			}
			else if (dir == SOUTH_EAST)
			{
				tx++;
				ty++;
			}
			else if (dir == SOUTH_WEST)
				ty++;
			else if (dir == NORTH_WEST)
				ty--;
		}
	}

	return getMapObject(tx, ty);
}

bool
IsoMapData::areNeighbors(const MapLocation &loc1,
						 const MapLocation &loc2) const
{
	return false;
}

MapLocation
IsoMapData::getRandomLocation() const
{
	return MapLocation();
}
