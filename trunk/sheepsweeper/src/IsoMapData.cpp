#include "IsoMapData.h"

IsoMapData::IsoMapData(int width, int height)
	: MapData(width, height)
{
}

IsoMapData::~IsoMapData()
{
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
