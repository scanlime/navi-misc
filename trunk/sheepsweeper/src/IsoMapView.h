#ifndef _ISO_MAP_VIEW_H_
#define _ISO_MAP_VIEW_H_

#include <SDL/SDL.h>

#include "MapView.h"

class IsoMapView : public MapView
{
	public:
		IsoMapView(SDL_Surface *screen);
		~IsoMapView();

		SDL_Surface *getScreen() const;

		virtual void repaintMap();

	private:
		SDL_Surface *screen;
};

#endif /* _ISO_MAP_VIEW_H_ */
