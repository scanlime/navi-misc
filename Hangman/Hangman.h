/*
 *  Hangman.h
 *
 *
 *  Created by Wallace Sheehan on Fri Jun 20 2003.
 *  Copyright (c) 2003 Wallace Sheehan. All rights reserved.
 *
 *  Header file for a Hangman game controller.
 * 
 */
#ifndef HANGMAN
#define HANGMAN

#include <stdlib.h>

// port to connect to host on
#define PORT 80

typedef struct hangman
{
  char **words;					// array containing all possible answers 
  char *guessed;				// array containing all guessed letters 
  char *correctGuessed;	// array containing correct guesses 
												// in the positions they occupy in the answer

  char type;						// 'f' indicates data from a file, 'i' indicates
												// internet

	int wordsArraySize;		// size of words array 
  int correctIndex;			// array index of the correct answer 
  int guessedSize;			// size of the guessed array 
}
Hangman;

// initialize the data in hman_object
void Hangman_Init (Hangman * hman_object);

// takes a file name and reads the contents of the file into hman_object,
// if game type is 'i' (internet) pass NULL as file_name
void Hangman_ReadFile (Hangman * hman_object, char *file_name);

// creates a new game using the data in hman_object, type indicates where to
// read from, internet or file
void Hangman_NewGame (Hangman * hman_object, char type);

// checks guess to see if it is in the answer in hman_object, returns 1
// if guess exists in hman_object->words[hman_object->correctIndex]
int Hangman_CheckGuess (Hangman * hman_object, char *guess);

// checks to see if all the letters in the answer have been guessed, returns
// 1 if all are correct
int Hangman_AllCorrect (Hangman * hman_object);

// parse the data recieved from a webpage, returns a 1 when the desired data
// is found
int parse(int fd, Hangman *hman_obj);

/******************************************************************************/
struct stack_node
{
	char data;
	struct stack_node *next;
};

typedef struct stack
{
	struct stack_node *head;
	size_t size;
} stack;

void init_stack(stack *stk);
void init_node(struct stack_node *node);			// initialize stack
void push(stack *stk, char add); 	// add character in 'add' to stk
char pop(stack *stk);							// remove top item on stack and return the
																	// character it contained

#endif
