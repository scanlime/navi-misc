/*
 * HangmanGUI.h
 *
 * Evan Sheehan
 * evan@navi.picogui.org
 *
 * Contains necessary functions and data for layout of the game window for
 * Hangman.  
 *
 */
#ifndef HANGMANGUI
#define HANGMANGUI

#include <SDL/SDL.h>

#include "GUI.h"
#include "Hangman.h"

typedef struct HM_Window
{
	/* Screen to draw to. */
	SDL_Surface* screen;

	GUI_Node* root;

	/* nodes where actual drawing will occur. */
	
	/* text of incorrectly guessed letters. */
	GUI_Node* missedText;
	/* text of correctly guessed letters. */
	GUI_Node* correctText;

	/* drawing of gallows and hangman */
	GUI_Node* gallows;

	/* field to enter guesses */
	GUI_Node* guessField;

	/* button to submit guess in guessField */
	GUI_Node* guessButton;
} HM_Window;

/* Initialize */
void HM_Window_Init(HM_Window* window);

/* Create the layout tree */
int LayoutTree(HM_Window *window);

/* Update layout tree information */
void UpdateTree(HM_Window* window, Hangman data);

/* Draw the tree to the screen */
void DrawWindow(HM_Window* window);

#endif
