#include "Sheep.h"

Sheep::Sheep(SDL_Surface *screen)
	: IsoMapObject(screen)
{
	setObjectType(MAP_OBJECT_SHEEP);
}

Sheep::~Sheep()
{
}
