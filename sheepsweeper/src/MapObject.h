#ifndef _MAP_OBJECT_H_
#define _MAP_OBJECT_H_

#include "Direction.h"
#include "MapLocation.h"

typedef enum
{
	MAP_OBJECT_BLANK = 0,
	MAP_OBJECT_SHEEP,
	MAP_OBJECT_TYPE_NUM

} MapObjectType;

class MapObject
{
	public:
		MapObject();
		virtual ~MapObject();

		void setObjectType(MapObjectType type);
		MapObjectType getObjectType() const;

		void setDirection(Direction dir);
		Direction getDirection() const;

		virtual int getTileWidth() const  = 0;
		virtual int getTileHeight() const = 0;
		virtual int getHeight() const = 0;

		int getRaisedOffset() const;

		void setLocation(const MapLocation &location);
		const MapLocation &getLocation() const;

		void setObjectAbove(MapObject *obj);
		void setObjectBelow(MapObject *obj);
		MapObject *getObjectAbove() const;
		MapObject *getObjectBelow() const;

		virtual void render() = 0;

	protected:
		virtual void directionChanged(Direction newDir) {}

	private:
		Direction dir;
		MapObjectType objType;
		MapLocation location;
		MapObject *aboveObj;
		MapObject *belowObj;
};

#endif /* _MAP_OBJECT_H_ */
