#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL_gfxPrimitives.h>

#include <stdio.h>
#include <stdlib.h>

#include "HangmanGUI.h"

/****************************** Initialize **********************************/
SDL_Surface* GUI_Init(int width, int height)
{
	SDL_Surface *window;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL_Init in GUI_Init(): %s\n",SDL_GetError());
		exit(1);
	}

	window = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE|SDL_ANYFORMAT|SDL_RESIZABLE);
	if (!window) {
		printf("SDL_SetVideoMode in GUI_Init(): %s\n", SDL_GetError());
		exit(1);
	}
	
	return window;
}

/****************************** GUI_DrawBg **********************************/
void GUI_DrawBg(SDL_Surface *screen)
{

}

/***************************** GUI_DrawMiss **********************************/
void GUI_DrawMiss(SDL_Surface *screen, char *missed)
{
	SDL_Surface *text;
	TTF_Font *font;
	SDL_Rect *dstrect;
	SDL_Color	color;

	color.r = 0;
	color.g = 0;
	color.b = 0;


	dstrect = malloc(sizeof(SDL_Rect));
	dstrect->x = 1024 - (1024/3);
	dstrect->y = (768/8);

	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(1);
	}

	font = TTF_OpenFont("/home/evan/bitstream/VeraMono.ttf", 24);
	if (!font) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		exit(1);
	}

	text = TTF_RenderText_Solid(font, missed, color);
	if (!text) {
		printf("TTF_RenderText_Solid: %s\n", TTF_GetError());
		exit(1);
	}

	if (SDL_BlitSurface(text, NULL, screen, dstrect) < 0) {
		printf("SDL_BlitSurface in GUI_DrawMiss: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_UpdateRect(screen, dstrect->x, dstrect->y, text->w, text->h);
}

/*****************************************************************************************/
void test()
{
	SDL_Surface *screen;
	SDL_Surface *image;
	SDL_Surface *char_print;
	TTF_Font *letters;
	SDL_Color	color;
	SDL_Rect *print_rect;
	SDL_Event event;
	int done = 0;

	print_rect = malloc(sizeof(SDL_Rect));

	if(SDL_Init(SDL_INIT_VIDEO)<0) {
		printf("Unable to init sdl, %s\n",SDL_GetError());
		exit(1);
	}

	atexit(SDL_Quit);

	if(TTF_Init() == -1) {
		printf("TTF Init: %s\n",TTF_GetError());
		exit(1);
	}

	color.r = 0;
	color.g = 0;
	color.b = 0;

	screen = SDL_SetVideoMode(1024,768,24,SDL_SWSURFACE);
	if(!screen) {
		printf("Couldn't set vid mode\n");
		exit(1);
	}

	image = IMG_Load("window.png");
	if(!image) {
		printf("Error loading bmp\n");
		exit(1);
	}

	letters = TTF_OpenFont("/home/evan/bitstream/VeraMono.ttf",24);
	if(!letters) {
		printf("Error loading chars.\n");
		exit(1);
	}

	char_print = TTF_RenderText_Solid(letters, "Test", color);

	print_rect->x = 0;
	print_rect->y = 0;

	if (image->format->palette && screen->format->palette) {
		SDL_SetColors(screen, image->format->palette->colors, 0, image->format->palette->ncolors);
	}
	
	if (SDL_BlitSurface(image, NULL, screen, NULL) < 0) {
		printf("blit surface error\n");
		exit(1);
	}

	if(SDL_BlitSurface(char_print, NULL, screen, print_rect) < 0) {
		printf("char blit error.\n");
		exit(1);
	}

	SDL_UpdateRect(screen, 0, 0, image->w, image->h);
	SDL_Flip(screen);

	while(!done) {
		while(SDL_PollEvent(&event)) {
				if(event.type == SDL_QUIT)
					done = 1;
				if(event.type == SDL_KEYDOWN)
					if(event.key.keysym.sym == SDLK_ESCAPE)
						done = 1;
		}
	}
	
	SDL_FreeSurface(image);

}
