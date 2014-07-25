/*
 *  Hangman.c
 *
 *
 *  Created by Wallace Sheehan on Fri Jun 20 2003.
 *  Copyright (c) 2003 Wallace Sheehan. All rights reserved.
 *
 *  Implementation of Hangman game controller from Hangman.h.
 *
 */

#include "Hangman.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex.h>

/***********************************************************************/
void
Hangman_Init(Hangman * hman_object)
{
  //initialize sizes
  hman_object->wordsArraySize = 0;
  hman_object->correctIndex = 0;
  hman_object->guessedSize = 0;

  //initialize pointers
  hman_object->words = 0;
  hman_object->guessed = 0;
  hman_object->correctGuessed = 0;
}

/***********************************************************************/
void
Hangman_ReadFile(Hangman * hman_object, char *name)
{
  int    count, fileDescriptor;
  struct hostent *host;
  struct sockaddr_in host_addr;
  char   *sendMsg = "GET /\r\n\r\n";

  switch (hman_object->type) {
  case 'i':
    //handles internet data
    // get the host information
    if ((host = gethostbyname(name)) == NULL) {
      printf("\ngethostbyname\n");
      exit(1);
    }
    //more socket info
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr = *((struct in_addr *) host->h_addr);
    memset(&(host_addr.sin_zero), '\0', 8);

    //get socket descriptor
    if ((fileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      printf("Error getting socket descriptor.\n");
      exit(1);
    }
    //open a connection to the host
    if (connect(fileDescriptor, (struct sockaddr *) & host_addr,
		  sizeof(struct sockaddr))) {
      printf("\nconnect\n");
      exit(1);
    }
    //send the GET command to HOST
    if ((count = send(fileDescriptor, sendMsg, strlen(sendMsg), 0)) == -1) {
      printf("Error sending\n");
      exit(1);
    }
    break;

  case 'f':
    //handles file data
    // open file and get descriptor
    if ((fileDescriptor = open(name, O_RDONLY)) == -1) {
      printf("Error getting file descriptor.\n");
      exit(1);
    }
    break;

  default:
    break;
  };

  //parse the data from the file descriptor
	if(parse(fileDescriptor, hman_object) == 0)
		printf("\nError parsing.\n");

  //close the file descriptor
    close(fileDescriptor);
}

/************************************************************************/
int
Hangman_CheckGuess(Hangman * hman_object, char *guess)
{
  int             count;
  int             right = 0;
  guess[0] = tolower(guess[0]);

  //check to see if the guess exists in the answer
      for (count = 0;
	   				count < strlen(hman_object->words[hman_object->correctIndex]);
						count++)
      if (hman_object->words[hman_object->correctIndex][count] == guess[0]) {
	right = 1;
	hman_object->correctGuessed[count] = guess[0];
      }
  //if it exists in the answer, return true
      if (right)
      return 1;

  //if it doesn 't exist add the guess to the misses and return false
      else {
    hman_object->guessed =
      realloc(hman_object->guessed, hman_object->guessedSize + 1);

    if (!hman_object->guessed) {
      printf("\nError in character array.\n");
      exit(1);
    }
    hman_object->guessed[hman_object->guessedSize] = guess[0];
    hman_object->guessedSize++;
    return 0;
    }
}

/************************************************************************/
void
Hangman_NewGame(Hangman * hman_object, char type)
{
  int             count;

  //free the memory from last game
    free(hman_object->correctGuessed);
  free(hman_object->guessed);
  hman_object->correctGuessed = 0;
  hman_object->guessed = 0;

  switch (hman_object->type) {
  case 'i':
    //word source from the internet
      hman_object->correctIndex = 0;
    break;

  case 'f':
    //word source from a file
      // randomly select a new word
      srand(time(NULL));
    hman_object->correctIndex = (rand() % hman_object->wordsArraySize);

    hman_object->correctGuessed =
      malloc(strlen(hman_object->words[hman_object->correctIndex]) + 1);

    hman_object->
      correctGuessed[strlen(hman_object->words[hman_object->correctIndex])] =
      0;
    break;
  default:
    break;
  };

  //loop that initializes the correctGuessed array to all '_'
    for (count = 0; count <
	 strlen(hman_object->words[hman_object->correctIndex]); count++)
    hman_object->correctGuessed[count] = '_';
}

/************************************************************************/
int
Hangman_AllCorrect(Hangman * hman_object)
{
  int i;

  //if at least one letter doesn't match return false
  for (i = 0; i < strlen(hman_object->words[hman_object->correctIndex]); i++) {
      if (hman_object->words[hman_object->correctIndex][i] !=
	  			hman_object->correctGuessed[i])
				return 0;
    }

  return 1;
}

/************************************************************************/
int
parse(int fd, Hangman * hman_obj)
{
  int   count, count2, count3; 
  char  buf[20000];
  stack char_stack;

	// regular expression stuff for parsing the webpage
	char	*tag = "<a *href=\"/wordoftheday/\">.[^<]*</a>";
	regex_t regtag;
	regmatch_t	matches[1];

  init_stack(&char_stack);
  count2 = 0;

  memset(&buf, '\0', sizeof(buf));
  read(fd, buf, sizeof(buf) - 1);
  
	switch (hman_obj->type) {
    case 'i':		// parse a webpage
			// array size for this option will always be one
			hman_obj->wordsArraySize = 1;
			// create the regular expression to search for from tag
			if (regcomp(&regtag, tag, REG_ICASE) != 0) {
				printf("\nError creating regular expression.\n");
				exit(1);
			}
			// search for expression in buf, error if string isn't found
			if (regexec(&regtag, buf, sizeof(matches)/sizeof(regmatch_t), matches, REG_NOTBOL|REG_NOTEOL) 
					== REG_NOMATCH) {
				printf("\nError comparing regular expressions.\n");
				return 0;
			}
			// if string is found parse it for the word, strip the html
			else
				for(count=matches[0].rm_so; count<matches[0].rm_eo; count++) {
					// skip through tags
					if(buf[count] == '<')
						push(&char_stack, buf[count]);
					else if(buf[count] == '>')
						pop(&char_stack);
					// here you are not in a tag, so it's the word
					else if(char_stack.size == 0) {
						// push the word onto the stack
						do {
							push(&char_stack,buf[count]);
							count++;
							} while(buf[count] != '<');
						// allocate memory for word in the array of answers
						hman_obj->words = malloc(sizeof(char *));
						hman_obj->words[0] = malloc(char_stack.size+1);
						hman_obj->words[0][char_stack.size] = 0;
						// pull word from stack into array of answers
						for(count = char_stack.size-1; count >= 0; count--)
							hman_obj->words[0][count] = pop(&char_stack);
						// skip the rest of the matched expression, it's just tags
						count = matches[0].rm_eo;
						}
				}
			// done w/ regtag
			regfree(&regtag);
      break;

    case 'f':		// parse a file
      for (count = 0; count < strlen(buf); count++) {
				// here is the end of a word
				if (buf[count] == '\n') {
					// allocate memory for new word
	 				hman_obj->words = realloc(hman_obj->words, (count2 + 1) * sizeof(char *));
	  			hman_obj->words[count2] = malloc(char_stack.size + 1);

	  			count3 = char_stack.size - 1;
	  			hman_obj->words[count2][char_stack.size] = 0;

					// pull word from stack into array of possible answers
	  			while (char_stack.size != 0) {
	    			hman_obj->words[count2][count3] = pop(&char_stack);
	    			count3--;
	  			}

		  		count2++;
		  		hman_obj->words = realloc(hman_obj->words, (count2 + 1) * sizeof(char *));
				// if you're not at the end of a word, push the character on the stack
				} else
	  			push(&char_stack, buf[count]);
      }
			// set the size of the words array to the correct number
			hman_obj->wordsArraySize = count2;
      break;

    default:
      break;
    };

  return 1;
}


/********************************************************************************/
void
init_stack(stack * stk)
{
  stk->head = 0;
  stk->size = 0;
}

void
init_node(struct stack_node * node)
{
  node->data = 0;
  node->next = 0;
}

void
push(stack * stk, char add)
{
  struct stack_node *node;

  node = malloc(sizeof(struct stack_node));
  init_node(node);

  node->data = add;
  node->next = stk->head;
  stk->head = node;
  stk->size++;
}

char
pop(stack * stk)
{
  char            val;
  struct stack_node *stk2;

  stk2 = stk->head->next;
  val = stk->head->data;
  free(stk->head);
  stk->head = stk2;

  stk->size--;

  return val;
}
