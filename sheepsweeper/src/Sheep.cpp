#include "Sheep.h"

#include <SDL/SDL_image.h>

#include <stdlib.h>

static const char *imageFiles[] =
{
	"images/sheep-180.png",
	"images/sheep-225.png",
	"images/sheep-90.png",
	"images/sheep-315.png",
	"images/sheep-0.png",
	"images/sheep-45.png",
	"images/sheep-270.png",
	"images/sheep-135.png"
};

static SDL_Surface **imageCache = NULL;

static void
initImageCache(void)
{
	imageCache = new SDL_Surface*[8];

	for (int i = 0; i < 8; i++)
	{
		SDL_Surface *tempImage, *image;

		if ((tempImage = IMG_Load(imageFiles[i])) == NULL)
		{
			fprintf(stderr, "Unable to load graphics file `%s': %s\n",
					imageFiles[i], SDL_GetError());
			exit(1);
		}

		if ((image = SDL_DisplayFormatAlpha(tempImage)) == NULL)
		{
			fprintf(stderr,
					"Unable to convert file `%s' to display format: %s\n",
					imageFiles[i], SDL_GetError());
			exit(1);
		}

		SDL_FreeSurface(tempImage);

		imageCache[i] = image;
	}
}

Sheep::Sheep(SDL_Surface *screen)
	: IsoMapObject(screen)
{
	setObjectType(MAP_OBJECT_SHEEP);

	if (imageCache == NULL)
		initImageCache();

	directionChanged(getDirection());
}

Sheep::~Sheep()
{
}

void
Sheep::directionChanged(Direction newDir)
{
	setImage(imageCache[(int)newDir]);
}
