#include "Fadey.h"

#include <SDL/SDL_image.h>

#include <stdlib.h>

Fadey::Fadey(SDL_Surface *screen, float time)
{
	this->time = time / 3.0f;
	done = false;
	this->screen = screen;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 rmask = 0xff000000;
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00;
	Uint32 amask = 0x000000ff;
#else
	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;
#endif
	black = SDL_CreateRGBSurface(SDL_SRCALPHA, screen->w, screen->h,
								 32, rmask, gmask, bmask, amask);

	SDL_Surface *tempImage = IMG_Load("images/poweredby.png");

	if (tempImage == NULL)
	{
		fprintf(stderr, "Unable to locate images directory.\n");
		exit(1);
	}

	image = SDL_DisplayFormat(tempImage);

	SDL_FreeSurface(tempImage);
}

Fadey::~Fadey()
{
	SDL_FreeSurface(black);
	SDL_FreeSurface(image);
}

void
Fadey::run()
{
	float lasttime = SDL_GetTicks();
	float curtime = 0;
	SDL_Event e;
	int alpha;

	while (curtime < time * 3)
	{
		if (curtime < time)
		{
			alpha = static_cast<int>((1 - (curtime / time)) * 255);
			SDL_FillRect(black, NULL, SDL_MapRGBA(black->format,
												  0, 0, 0, alpha));
		}
		else if (curtime > time * 2)
		{
			alpha = static_cast<int>(((curtime - time * 2) / time) * 255);
			SDL_FillRect(black, NULL, SDL_MapRGBA(black->format,
												  0, 0, 0, alpha));
		}
		else
		{
			SDL_FillRect(black, NULL, SDL_MapRGBA(black->format,
												  0, 0, 0, 0));
		}

		SDL_BlitSurface(image, NULL, screen, NULL);
		SDL_BlitSurface(black, NULL, screen, NULL);
		SDL_Flip(screen);

		curtime += (SDL_GetTicks() - lasttime) / 1000.0f;
		lasttime = SDL_GetTicks();

		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				SDL_FreeSurface(black);
				SDL_FreeSurface(image);
				SDL_Quit();
			}

			if (e.type == SDL_KEYDOWN)
				return;
		}
	}
}
