#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL_gfxPrimitives.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HangmanGUI.h"

void HM_Window_Init(HM_Window* window)
{
	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL_Init in HM_Window_Init: %s\n", SDL_GetError());
		exit(1);
	}
	window->screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE|SDL_ANYFORMAT);
	if (!window->screen) {
		printf("SDL_SetVideoMode in HM_Window_Init: %s\n", SDL_GetError());
		exit(1);
	}

	/* Initialize root node */
	window->root = GUI_Node_Init();
	window->root->w = 640;
	window->root->h = 480;
	window->root->draw = draw_drawfield;

	/* root window colors */
	window->root->color = malloc(sizeof(SDL_Color));
	window->root->color->r = 255;
	window->root->color->g = 255;
	window->root->color->b = 255;

	/* Initialize remaining variables to 0 */
	window->missedText = 0;
	window->correctText = 0;
	window->gallows = 0;
	window->guessField = 0;
	window->guessButton = 0;
}

/***************************************/
int LayoutTree(HM_Window* window)
{
	GUI_Node* tmp;

	window->missedText = make_textfield();
	window->correctText = make_textfield();
	window->gallows = make_drawfield();
	window->guessField = make_textfield();

	tmp = Combine_Vert(window->gallows, window->missedText, 66);
	window->root->left = tmp;

	tmp = Combine_Horz(window->correctText, window->guessField, 50);
	window->root->right = tmp;

	window->root->percent = 60;
	window->root->split = split_horizontal;

	return 0;
}

/***************************************/
void UpdateTree(HM_Window* window, Hangman data)
{
	strcpy(window->missedText->text, data.guessed);
	strcpy(window->correctText->text, data.correctGuessed);
}

/****************************************/
void DrawWindow(HM_Window *window)
{
	draw_tree(window->root, window->screen);
}
