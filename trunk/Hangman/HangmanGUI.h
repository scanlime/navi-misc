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

#include <SDL/SDL.h>

/************************ Drawing Functions *******************************/
/* Initialize SDL and return the primary SDL_Surface.
 */
SDL_Surface* GUI_Init(int width, int height);

/* Draw the background of the window.
 * Can also be used to clear the screen.
 */
void GUI_DrawBg(SDL_Surface *screen);

/* Draw the characters of missed guesses. Draws to *screen the
 * characters contained in *missed.
 */
void GUI_DrawMiss(SDL_Surface *screen, char *missed);

/* Draw the characters, in order, that are correct. Draws to *screen
 * the characters in *correct.  *correct is the size of the correct answer,
 * unguessed characters are stored as '_'. 
 */
void GUI_DrawCorr(SDL_Surface *screen, char *correct);
