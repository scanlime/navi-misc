#include "IsoMapObjectFactory.h"
#include "IsoMapObject.h"
#include "IsoMapTile.h"
#include "Sheep.h"

#include <SDL/SDL_image.h>

#include <stdlib.h>
#include <string.h>

static const char *objImageFiles[] =
{
	NULL,
	"images/sheep-45.png",
	NULL
};

static const char *tileImageFiles[] =
{
	NULL,
	"images/tiles/grass_green_1.png",
	"images/tiles/grass_brown_1.png",
	NULL
};

IsoMapObjectFactory::IsoMapObjectFactory(SDL_Surface *screen)
	: screen(screen)
{
	memset(objImageCache,  0, sizeof(SDL_Surface *) * MAP_OBJECT_TYPE_NUM);
	memset(tileImageCache, 0, sizeof(SDL_Surface *) * MAP_TILE_TYPE_NUM);
}

IsoMapObjectFactory::~IsoMapObjectFactory()
{
}

void
IsoMapObjectFactory::setScreen(SDL_Surface *screen)
{
	this->screen = screen;
}

SDL_Surface *
IsoMapObjectFactory::getScreen() const
{
	return screen;
}

MapObject *
IsoMapObjectFactory::makeObject(MapObjectType type)
{
	IsoMapObject *obj;
	SDL_Surface *image;

	if (!isValidObjectType(type))
		return NULL;

	if (type == MAP_OBJECT_SHEEP)
		obj = new Sheep(getScreen());
	else
	{
		obj = new IsoMapObject(getScreen());
		obj->setObjectType(type);
	}

	if (objImageCache[type] != NULL)
		image = objImageCache[type];
	else
	{
		SDL_Surface *tempImage = IMG_Load(objImageFiles[(int)type]);

		if (tempImage == NULL)
		{
			fprintf(stderr, "Unable to load graphics file `%s': %s\n",
					objImageFiles[(int)type], SDL_GetError());
			exit(1);
		}

		image = SDL_DisplayFormatAlpha(tempImage);

		if (image == NULL)
		{
			fprintf(stderr,
					"Unable to convert file `%s' to display format: %s\n",
					objImageFiles[type], SDL_GetError());
			exit(1);
		}

		SDL_FreeSurface(tempImage);

		objImageCache[type] = image;
	}

	obj->setImage(image);

	return obj;
}

MapObject *
IsoMapObjectFactory::makeSheep()
{
	return new Sheep(getScreen());
}

MapTile *
IsoMapObjectFactory::makeTile(MapTileType type)
{
	IsoMapTile *tile;
	SDL_Surface *image;

	if (!isValidTileType(type))
		return NULL;

	tile = new IsoMapTile(getScreen());
	tile->setTileType(type);

	if (tileImageCache[type] != NULL)
		image = tileImageCache[type];
	else
	{
		SDL_Surface *tempImage = IMG_Load(tileImageFiles[(int)type]);

		if (tempImage == NULL)
		{
			fprintf(stderr, "Unable to load graphics file `%s': %s\n",
					tileImageFiles[(int)type], SDL_GetError());
			exit(1);
		}

		image = SDL_DisplayFormatAlpha(tempImage);

		if (image == NULL)
		{
			fprintf(stderr,
					"Unable to convert file `%s' to display format: %s\n",
					tileImageFiles[type], SDL_GetError());
			exit(1);
		}

		SDL_FreeSurface(tempImage);

		tileImageCache[type] = image;
	}

	tile->setImage(image);

	return tile;
}
