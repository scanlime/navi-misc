#include "Fadey.h"
#include "IsoMapData.h"
#include "IsoMapView.h"
#include "MapObjectFactory.h"

#include <stdlib.h>

static void
handleEvents()
{
	SDL_Event event;

	while (SDL_WaitEvent(&event) != 0)
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return;
		}

		SDL_Delay(50);
	}
}

static SDL_Surface *
setupSDL()
{
	SDL_Surface *screen;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_HWSURFACE | SDL_DOUBLEBUF |
				 SDL_INIT_TIMER) < 0)
	{
		fprintf(stderr, "Video initialization failed: %s\n",
				SDL_GetError());
		exit(1);
	}

	atexit(SDL_Quit);


	screen = SDL_SetVideoMode(660, 465, 16, 0);

	if (screen == NULL)
	{
		fprintf(stderr, "Unable to set video mode: %s\n",
				SDL_GetError());
		exit(1);
	}

	SDL_WM_SetCaption("Sheep Sweeper", "Sheep Sweeper");

	return screen;
}

int
main(int argc, char **argv)
{
	MapView *mapView;
	MapData *mapData;
	MapObjectFactory *objFactory;
	SDL_Surface *screen;

	screen = setupSDL();

	Fadey fadey(screen, 3.0f);
	fadey.run();

	mapData = new IsoMapData(12, 32);
	mapView = new IsoMapView(screen);
	mapView->setMapData(mapData);

	objFactory = mapView->getObjectFactory();

	for (int y = 0; y < mapData->getHeight(); y++)
	{
		for (int x = 0; x < mapData->getWidth(); x++)
		{
			if ((x == 0 && y % 2 == 0) || y == 0 ||
				(x == mapData->getWidth() - 1 && y % 2 == 0) ||
				y == mapData->getHeight() - 1)
			{
				MapLocation loc(x, y);

				mapData->addMapObject(
					objFactory->makeTile(MAP_TILE_GRASS_BROWN), loc);
			}
			else
			{
				MapLocation loc(x, y);

				mapData->addMapObject(
					objFactory->makeTile(MAP_TILE_GRASS_GREEN), loc);
			}
		}
	}

	mapData->addMapObject( objFactory->makeSheep(), MapLocation(1, 4, 1));
	mapData->addMapObject(
		objFactory->makeTile(MAP_TILE_GRASS_BROWN), MapLocation(1, 5, 1));

	mapData->addMapObject(
		objFactory->makeTile(MAP_TILE_GRASS_BROWN), MapLocation(4, 10, 1));
	mapData->addMapObject(objFactory->makeSheep(), MapLocation(4, 10));
	mapData->addMapObject(objFactory->makeSheep(), MapLocation(4, 10));

	mapView->repaintMap();

	handleEvents();

	delete mapView;
	delete mapData;

	SDL_Quit();
}
