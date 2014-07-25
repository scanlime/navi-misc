#ifndef _ISO_MAP_TILE_H_
#define _ISO_MAP_TILE_H_

#include <SDL/SDL.h>

#include "MapTile.h"

class IsoMapTile : public MapTile
{
	public:
		static const int TILE_WIDTH  = 60;
		static const int TILE_HEIGHT = 30;

	public:
		IsoMapTile(SDL_Surface *screen);
		virtual ~IsoMapTile();

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

#endif /* _ISO_MAP_TILE_H_ */
