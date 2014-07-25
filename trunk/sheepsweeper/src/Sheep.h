#ifndef _SHEEP_H_
#define _SHEEP_H_

#include "IsoMapObject.h"

class Sheep : public IsoMapObject
{
	public:
		Sheep(SDL_Surface *screen);
		virtual ~Sheep();

	protected:
		virtual void directionChanged(Direction newDir);
};

#endif /* _SHEEP_H_ */
