#ifndef _MAP_LOCATION_H_
#define _MAP_LOCATION_H_

class MapLocation
{
	public:
		MapLocation(int x = 0, int y = 0, int layer = -1);
		MapLocation(const MapLocation &loc);
		virtual ~MapLocation();

		void setX(int x);
		void setY(int y);
		void setLayer(int layer);

		int getX() const;
		int getY() const;
		int getLayer() const;

		bool isEvenRow() const;
		bool isOddRow() const;

	private:
		int x;
		int y;
		int layer;
};

#endif /* _MAP_LOCATION_H_ */
