#include "MapLocation.h"

MapLocation::MapLocation(int x, int y, int layer)
	: x(x), y(y), layer(layer)
{
}

MapLocation::MapLocation(const MapLocation &loc)
{
	setX(loc.getX());
	setY(loc.getY());
	setLayer(loc.getLayer());
}

MapLocation::~MapLocation()
{
}

void
MapLocation::setX(int x)
{
	this->x = x;
}

void
MapLocation::setY(int y)
{
	this->y = y;
}

void
MapLocation::setLayer(int layer)
{
	this->layer = layer;
}

int
MapLocation::getX() const
{
	return x;
}

int
MapLocation::getY() const
{
	return y;
}

int
MapLocation::getLayer() const
{
	return layer;
}

bool
MapLocation::isEvenRow() const
{
	return (getY() % 2 == 0);
}

bool
MapLocation::isOddRow() const
{
	return !isEvenRow();
}
