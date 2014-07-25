#include "IsoMapObject.h"

static const int heights[] =
{
	0, 20
};

IsoMapObject::IsoMapObject(SDL_Surface *screen)
	: MapObject(), screen(screen), image(NULL)
{
}

IsoMapObject::~IsoMapObject()
{
}

int
IsoMapObject::getTileWidth() const
{
	if (image == NULL)
		return 0;

	return image->w;
}

int
IsoMapObject::getTileHeight() const
{
	if (image == NULL)
		return 0;

	return image->h;
}

int
IsoMapObject::getHeight() const
{
	return heights[getObjectType()];
}

void
IsoMapObject::setImage(SDL_Surface *image)
{
	this->image = image;
}

SDL_Surface *
IsoMapObject::getImage() const
{
	return image;
}

SDL_Surface *
IsoMapObject::getScreen() const
{
	return screen;
}

void
IsoMapObject::render()
{
	SDL_Rect rect;

	if (getImage() == NULL || getScreen() == NULL)
		return;

	int xOff = 0, yOff = 0;

	yOff = -(TILE_HEIGHT / 2);

	if (getLocation().getY() % 2 == 0)
		xOff = -(TILE_WIDTH / 2);

	yOff -= getRaisedOffset();

	rect.x = getLocation().getX() * TILE_WIDTH + xOff;
	rect.y = getLocation().getY() * (TILE_HEIGHT / 2) +
	         (TILE_HEIGHT - getTileHeight()) + yOff;
	rect.w = getTileWidth();
	rect.h = getTileHeight();

	SDL_BlitSurface(getImage(), NULL, getScreen(), &rect);
}
