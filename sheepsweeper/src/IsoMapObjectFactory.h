#ifndef _ISO_MAP_OBJECT_FACTORY_H_
#define _ISO_MAP_OBJECT_FACTORY_H_

#include <SDL/SDL.h>

#include "MapObjectFactory.h"

class IsoMapObjectFactory : public MapObjectFactory
{
	public:
		IsoMapObjectFactory(SDL_Surface *screen = NULL);
		~IsoMapObjectFactory();

		virtual MapObject *makeObject(MapObjectType type);
		virtual MapObject *makeSheep();
		virtual MapTile   *makeTile(MapTileType type);

		void setScreen(SDL_Surface *screen);
		SDL_Surface *getScreen() const;

	private:
		SDL_Surface *screen;
		SDL_Surface *objImageCache[MAP_OBJECT_TYPE_NUM];
		SDL_Surface *tileImageCache[MAP_TILE_TYPE_NUM];
};

#endif /* _ISO_MAP_OBJECT_FACTORY_H_ */
