#ifndef _ISO_MAP_OBJECT_H_
#define _ISO_MAP_OBJECT_H_

#include <SDL/SDL.h>

#include "MapObject.h"

class IsoMapObject : public MapObject
{
	public:
		static const int TILE_WIDTH  = 60;
		static const int TILE_HEIGHT = 30;

	public:
		IsoMapObject(SDL_Surface *screen);
		virtual ~IsoMapObject();

		virtual int getTileWidth() const;
		virtual int getTileHeight() const;
		virtual int getHeight() const;

		void setImage(SDL_Surface *image);
		SDL_Surface *getImage() const;
		SDL_Surface *getScreen() const;

		virtual void render();

	private:
		SDL_Surface *screen;
		SDL_Surface *image;
};

#endif /* _ISO_MAP_OBJECT_H_ */
