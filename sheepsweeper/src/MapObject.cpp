#include "MapObject.h"

#include <stdlib.h>
#include <stdio.h>

MapObject::MapObject()
	: objType(MAP_OBJECT_BLANK), dir(NORTH), aboveObj(NULL), belowObj(NULL)
{
}

MapObject::~MapObject()
{
}

void
MapObject::setObjectType(MapObjectType type)
{
	objType = type;
}

MapObjectType
MapObject::getObjectType() const
{
	return objType;
}

void
MapObject::setDirection(Direction dir)
{
	if (dir == getDirection())
		return;

	this->dir = dir;

	directionChanged(dir);
}

Direction
MapObject::getDirection() const
{
	return dir;
}

int
MapObject::getRaisedOffset() const
{
	MapObject *obj;
	int offset = 0;

	for (obj = getObjectBelow(); obj != NULL; obj = obj->getObjectBelow())
		offset += obj->getHeight();

	return offset;
}

void
MapObject::setLocation(const MapLocation &location)
{
	this->location = location;
}

const MapLocation &
MapObject::getLocation() const
{
	return location;
}

void
MapObject::setObjectAbove(MapObject *obj)
{
	if (getObjectAbove() == obj)
		return;

	aboveObj = obj;

	if (aboveObj != NULL)
		aboveObj->setObjectBelow(this);
}

void
MapObject::setObjectBelow(MapObject *obj)
{
	if (getObjectBelow() == obj)
		return;

	belowObj = obj;

	if (belowObj != NULL)
		belowObj->setObjectAbove(this);
}

MapObject *
MapObject::getObjectAbove() const
{
	return aboveObj;
}

MapObject *
MapObject::getObjectBelow() const
{
	return belowObj;
}
