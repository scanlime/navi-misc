#include "MapView.h"

#include <stdlib.h>

MapView::MapView()
	: mapData(NULL), objFactory(NULL)
{
}

MapView::~MapView()
{
}

void
MapView::setMapData(MapData *data)
{
	mapData = data;
}

MapData *
MapView::getMapData() const
{
	return mapData;
}

MapObjectFactory *
MapView::getObjectFactory() const
{
	return objFactory;
}

void
MapView::setObjectFactory(MapObjectFactory *factory)
{
	objFactory = factory;
}
