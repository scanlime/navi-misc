#include "IsoMapView.h"
#include "IsoMapObjectFactory.h"

IsoMapView::IsoMapView(SDL_Surface *screen)
	: MapView(), screen(screen)
{
	setObjectFactory(new IsoMapObjectFactory(getScreen()));
}

IsoMapView::~IsoMapView()
{
}

SDL_Surface *
IsoMapView::getScreen() const
{
	return screen;
}

void
IsoMapView::repaintMap()
{
	MapObject *obj;

	if (getMapData() == NULL)
		return;

	for (int y = 0; y < getMapData()->getHeight(); y++)
	{
		for (int x = 0; x < getMapData()->getWidth(); x++)
		{
			for (obj = getMapData()->getMapObject(x, y);
				 obj != NULL;
				 obj = obj->getObjectAbove())
			{
				obj->render();
			}
		}
	}

	SDL_UpdateRect(getScreen(), 0, 0, getScreen()->w, getScreen()->h);
}
