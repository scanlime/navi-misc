#ifndef _MAP_VIEW_H_
#define _MAP_VIEW_H_

#include "MapData.h"

class MapObjectFactory;

class MapView
{
	public:
		MapView();
		virtual ~MapView();

		void setMapData(MapData *data);
		MapData *getMapData() const;

		MapObjectFactory *getObjectFactory() const;

		virtual void repaintMap() = 0;

	protected:
		void setObjectFactory(MapObjectFactory *factory);

	private:
		MapData *mapData;
		MapObjectFactory *objFactory;
};

#endif /* _MAP_VIEW_H_ */
