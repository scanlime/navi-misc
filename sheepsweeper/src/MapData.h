#ifndef _MAP_DATA_H_
#define _MAP_DATA_H_

#include "MapObject.h"

class MapObject;

class MapData
{
	public:
		MapData(int width, int height);
		virtual ~MapData();

		int getWidth() const;
		int getHeight() const;

		void addMapObject(MapObject *obj, MapLocation location);

		MapObject *getMapObject(const MapLocation &location);
		MapObject *getMapObject(int x, int y);

		MapObject *getNeighbor(const MapLocation &location, Direction dir);
		virtual MapObject *getNeighbor(int x, int y, Direction dir) = 0;

		virtual bool areNeighbors(const MapLocation &loc1,
								  const MapLocation &loc2) const = 0;

		virtual MapLocation getRandomLocation() const;

	protected:
		int getArrayWidth() const;
		int getArrayHeight() const;

	private:
		int width;
		int height;

		MapObject ***mapData;
};

#endif /* _MAP_DATA_H_ */
