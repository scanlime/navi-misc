#include "IsoMapTile.h"

static const int heights[] =
{
	0, 1, 1
};

IsoMapTile::IsoMapTile(SDL_Surface *screen)
	: MapTile(), screen(screen), image(NULL)
{
}

IsoMapTile::~IsoMapTile()
{
}

int
IsoMapTile::getTileWidth() const
{
	if (image == NULL)
		return 0;

	return image->w;
}

int
IsoMapTile::getTileHeight() const
{
	if (image == NULL)
		return 0;

	return image->h;
}

int
IsoMapTile::getHeight() const
{
	return heights[getTileType()];
}

void
IsoMapTile::setImage(SDL_Surface *image)
{
	this->image = image;
}

SDL_Surface *
IsoMapTile::getImage() const
{
	return image;
}

SDL_Surface *
IsoMapTile::getScreen() const
{
	return screen;
}

void
IsoMapTile::render()
{
	SDL_Rect rect;

	if (getImage() == NULL || getScreen() == NULL)
		return;

	int xOff = 0, yOff = 0;

	yOff = -(TILE_HEIGHT / 2);

	if (getLocation().getY() % 2 == 0)
		xOff = -(TILE_WIDTH / 2);

	rect.x = getLocation().getX() * TILE_WIDTH + xOff;
	rect.y = getLocation().getY() * (TILE_HEIGHT / 2) +
	         (TILE_HEIGHT - getTileHeight()) + yOff;
	rect.w = getTileWidth();
	rect.h = getTileHeight();

	SDL_BlitSurface(getImage(), NULL, getScreen(), &rect);
}
