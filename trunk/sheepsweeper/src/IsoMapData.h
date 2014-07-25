#ifndef _ISO_MAP_DATA_H_
#define _ISO_MAP_DATA_H_

#include "MapData.h"

class IsoMapData : public MapData
{
	public:
		IsoMapData(int width, int height);
		virtual ~IsoMapData();

		MapObject *getNeighbor(int x, int y, Direction dir);

		virtual bool areNeighbors(const MapLocation &loc1,
								  const MapLocation &loc2) const;

		virtual MapLocation getRandomLocation() const;
};

#endif /* _ISO_MAP_DATA_H_ */
