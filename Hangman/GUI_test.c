#include "HangmanGUI.h"

#include <SDL/SDL.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	SDL_Surface *screen;
	SDL_Event event;
	int done = 0;

	screen = GUI_Init(640, 480);
	GUI_DrawBg(screen);
	system("sleep 2");
	//GUI_DrawMiss(screen, "test");
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				done = 1;
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE)
					done = 1;
			}
			if (event.type == SDL_VIDEORESIZE) {
				screen = GUI_Init(event.resize.w, event.resize.h);
				GUI_DrawBg(screen);
			}
		}
	}

	SDL_FreeSurface(screen);
	return 0;
}
