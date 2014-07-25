#ifndef _FADEY_H_
#define _FADEY_H_

#include <SDL/SDL.h>

class Fadey
{
	public:
		Fadey(SDL_Surface *screen, float time);
		virtual ~Fadey();

		void run();

	protected:
		float time;

	private:
		bool done;
		SDL_Surface *image;
		SDL_Surface *black;
		SDL_Surface *screen;
};

#endif /* _FADEY_H_ */
