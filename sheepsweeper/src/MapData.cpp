#include "MapData.h"

#include <stdlib.h>
#include <string.h>

MapData::MapData(int width, int height)
	: width(width), height(height), mapData(NULL)
{
	mapData = new MapObject**[height];

	for (int y = 0; y < height; y++)
	{
		mapData[y] = new MapObject*[width];

		memset(mapData[y], 0, width * sizeof(MapObject *));
	}
}

MapData::~MapData()
{
	for (int y = 0; y < getArrayHeight(); y++)
	{
		for (int x = 0; x < getArrayWidth(); x++)
			delete mapData[y][x];

		delete [] mapData[y];
	}

	delete [] mapData;
}

int
MapData::getWidth() const
{
	return width;
}

int
MapData::getHeight() const
{
	return height;
}

void
MapData::addMapObject(MapObject *obj, MapLocation location)
{
	MapObject *currentObj;
	int i = 0;
	int x, y, layer;

	x     = location.getX();
	y     = location.getY();
	layer = location.getLayer();

	if (x >= getWidth() || y >= getHeight())
		return;

	if (layer == 0)
	{
		currentObj = mapData[y][x];

		if (currentObj != NULL)
			currentObj->setObjectBelow(obj);

		mapData[y][x] = obj;
	}
	else
	{
		for (i = 0, currentObj = mapData[y][x];
			 (layer == -1 || i < layer) &&
			 currentObj != NULL && currentObj->getObjectAbove() != NULL;
			 currentObj = currentObj->getObjectAbove(), i++)
			;

		if (currentObj == NULL)
			mapData[y][x] = obj;
		else
		{
			MapObject *aboveObj = currentObj->getObjectAbove();

			currentObj->setObjectAbove(obj);

			if (aboveObj != NULL)
				aboveObj->setObjectBelow(obj);
		}
	}

	location.setLayer(i);
	obj->setLocation(location);
}

MapObject *
MapData::getMapObject(const MapLocation &location)
{
	return getMapObject(location.getX(), location.getY());
}

MapObject *
MapData::getMapObject(int x, int y)
{
	if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight())
		return NULL;

	return mapData[y][x];
}

MapObject *
MapData::getNeighbor(const MapLocation &location, Direction dir)
{
	return getNeighbor(location.getX(), location.getY(), dir);
}

MapLocation
MapData::getRandomLocation() const
{
	return MapLocation();
}

int
MapData::getArrayWidth() const
{
	return width;
}

int
MapData::getArrayHeight() const
{
	return height;
}
