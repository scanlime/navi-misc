/*
 *  test_main.c
 *
 *
 *  Created by Wallace Sheehan on Fri Jun 20 2003.
 *  Copyright (c) 2003 Wallace Sheehan. All rights reserved.
 *
 *  A small main function to test the hangman controller.
 *
 */

#include <stdio.h>
#include <string.h>
#include "Hangman.h"
#include "HangmanGUI.h"

#define HOST "dictionary.com"
#define PATH "/home/evan/Hangman/"
const int MAX_Guess = 6;

int
main (int argc, char **argv)
{
  Hangman controller;	/* access to Hangman engine */
	HM_Window window;		/* access to Hangman gui */

  char guess[32];
  char filename[32];
  int count = 0;
  int win = 0;

	HM_Window_Init(&window);
	LayoutTree(&window);
	DrawWindow(&window);

  // contains debugging statements to indicate where in the program execution is

  // initialize the controller
  printf ("initializing...\n");
  Hangman_Init (&controller);

	// test the internet stuff
	controller.type = 'i';
	Hangman_ReadFile(&controller, HOST);

	controller.type = 'f';
  printf ("Enter file name: ");
  fgets (guess, sizeof (guess), stdin);

  guess[strlen (guess) - 1] = 0;
  strcpy (filename, PATH);
	printf ("%s\n",filename);
  strcat (filename, guess);
	printf("%s\n",filename);

  printf ("Reading file...\n");
  Hangman_ReadFile (&controller, filename);
  printf ("Done.\n");

  // loop for repeated tests, ctrl C to break
  while (1) {
      // start a new game
      printf ("Creating new game...\n");
      Hangman_NewGame (&controller, controller.type);
      win = 0;
      count = 0;

      // loop through current game until user guesses correctly, or until they
      // run out of guesses
      while (controller.guessedSize < MAX_Guess && win == 0) {
	  		printf ("Guess: ");
	  		fgets (guess, sizeof (guess), stdin);

	  		if (Hangman_CheckGuess (&controller, guess)) {
	      	printf ("Correct!\n");
	      	printf ("%s\n", controller.correctGuessed);
	    	}

	  		else 
	      	printf ("wrong\n");

	  	if (Hangman_AllCorrect (&controller))
	    	win = 1;
			}

			controller.guessedSize = 0;
    }
  return 0;
}
