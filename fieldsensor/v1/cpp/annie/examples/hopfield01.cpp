/*
 *	annie - Neural Network Library
 *	http://annie.sourceforge.net/
 *
 *	EXAMPLE - Hopfield Network used for pattern retrieval
 *
 *	Last Modified On:
 *		January 12, 2003
 *
 *  Author(s):
 *		Asim Shankar
 *    
 *	This example shows how a Hopfield Network can store patterns
 *  and then, given a pattern with noise, retrieve the original
 *  pattern.
 *  Patterns stored are composed of -1s and 1s.
 */

#include <annie.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace annie;

#define NUM_DATA      5			//Total number of patterns stored
#define X             10		//The "width" of a pattern
#define Y             10		//The "height" of a pattern
/* The width and height are useful for display purposes only,
 * as far as the Hopfield Network is concerned, a pattern is
 * just a single dimension array of -1s and 1s
 */

#define N             (X * Y)

//Stores the pattern in an array containing +1 or -1 instead of characters
int	pattern[X*Y];

// The original patters to be stored
// O represents a 1 and a space represents a -1
char                  Pattern[NUM_DATA][Y][X+1]  = { { "O O O O O ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O"  },

                                                   { "OO  OO  OO",
                                                     "OO  OO  OO",
                                                     "  OO  OO  ",
                                                     "  OO  OO  ",
                                                     "OO  OO  OO",
                                                     "OO  OO  OO",
                                                     "  OO  OO  ",
                                                     "  OO  OO  ",
                                                     "OO  OO  OO",
                                                     "OO  OO  OO"  },

                                                   { "OOOOO     ",
                                                     "OOOOO     ",
                                                     "OOOOO     ",
                                                     "OOOOO     ",
                                                     "OOOOO     ",
                                                     "     OOOOO",
                                                     "     OOOOO",
                                                     "     OOOOO",
                                                     "     OOOOO",
                                                     "     OOOOO"  },

                                                   { "O  O  O  O",
                                                     " O  O  O  ",
                                                     "  O  O  O ",
                                                     "O  O  O  O",
                                                     " O  O  O  ",
                                                     "  O  O  O ",
                                                     "O  O  O  O",
                                                     " O  O  O  ",
                                                     "  O  O  O ",
                                                     "O  O  O  O"  },

                                                   { "OOOOOOOOOO",
                                                     "O        O",
                                                     "O OOOOOO O",
                                                     "O O    O O",
                                                     "O O OO O O",
                                                     "O O OO O O",
                                                     "O O    O O",
                                                     "O OOOOOO O",
                                                     "O        O",
                                                     "OOOOOOOOOO"  } };

/* Patterns with noise
 * Note that the patterns have been created such that
 * Pattern_[i] is a noisy version of the stored pattern Pattern[i]
 */
char                  Pattern_[NUM_DATA][Y][X+1] = { { "          ",
                                                     "          ",
                                                     "          ",
                                                     "          ",
                                                     "          ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O",
                                                     "O O O O O ",
                                                     " O O O O O"  },

                                                   { "OOO O    O",
                                                     " O  OOO OO",
                                                     "  O O OO O",
                                                     " OOO   O  ",
                                                     "OO  O  OOO",
                                                     " O OOO   O",
                                                     "O OO  O  O",
                                                     "   O OOO  ",
                                                     "OO OOO  O ",
                                                     " O  O  OOO"  },

                                                   { "OOOOO     ",
                                                     "O   O OOO ",
                                                     "O   O OOO ",
                                                     "O   O OOO ",
                                                     "OOOOO     ",
                                                     "     OOOOO",
                                                     " OOO O   O",
                                                     " OOO O   O",
                                                     " OOO O   O",
                                                     "     OOOOO"  },

                                                   { "O  OOOO  O",
                                                     "OO  OOOO  ",
                                                     "OOO  OOOO ",
                                                     "OOOO  OOOO",
                                                     " OOOO  OOO",
                                                     "  OOOO  OO",
                                                     "O  OOOO  O",
                                                     "OO  OOOO  ",
                                                     "OOO  OOOO ",
                                                     "OOOO  OOOO"  },

                                                   { "OOOOOOOOOO",
                                                     "O        O",
                                                     "O        O",
                                                     "O        O",
                                                     "O   OO   O",
                                                     "O   OO   O",
                                                     "O        O",
                                                     "O        O",
                                                     "O        O",
                                                     "OOOOOOOOOO"  } };


//Converts the 2-dimensional pattern of Os and spaces to a single dimension array
//of -1s and 1s
void putPatternIntoPattern(int p)
{
	for (int i=0;i<X*Y;i++)
	{
		char c=Pattern[p][i/X][i%Y];
		if(c=='O')
			pattern[i]=1;
		else
			pattern[i]=-1;
	}
}

/* Does the same thing as the above function, except that it takes an index into
 * the noisy patters
 */
void putPattern_IntoPattern(int p)
{
	for (int i=0;i<X*Y;i++)
	{
		char c=Pattern_[p][i/X][i%Y];
		if(c=='O')
			pattern[i]=1;
		else
			pattern[i]=-1;
	}
}

/* Displays the pattern received from the network (single dimension array of
 * -1s and 1, as a two dimensional pattern (X * Y) of Os and spaces
 */
void printPattern(vector<int> pattern)
{
	int i=0;
	for (int x=0;x<X;x++)
	{
		for (int y=0;y<Y;y++,i++)
		{
			if (pattern[i]==1)
				cout<<"O";
			else
				cout<<" ";
		}
		cout<<endl;
	}
	cout<<"-------------------------------------"<<endl;
}

void printPattern(int pattern[])
{
	int i=0;
	for (int x=0;x<X;x++)
	{
		for (int y=0;y<Y;y++,i++)
		{
			if (pattern[i]==1)
				cout<<"O";
			else
				cout<<" ";
		}
		cout<<endl;
	}
	cout<<"-------------------------------------"<<endl;
}

int main()
{
	//Instantiate a pattern of X*Y neurons
	HopfieldNetwork net(X*Y);
	
	//Store the patterns in the network
	for (int i=0;i<NUM_DATA;i++)
	{
		putPatternIntoPattern(i);
		net.addPattern(pattern);
	}


	//Display a menu
	int choice;
	do
	{
		cout<<"-----------------------------------------\n";
		cout<<"1. View a pattern stored in the network\n";
		cout<<"2. Feed a noisy pattern to the network\n";
		cout<<"3. Exit\n";
		cin>>choice;

		if (choice<1 || choice>3)
			continue;
	
		int p;
		switch(choice)
		{
		case 1:
			printf("%d patterns have been stored in the network. Which one to display? (1-%d) ",NUM_DATA,NUM_DATA);
			cin>>p;
			p--;
			if (p<0 || p>=NUM_DATA)
				break;
			putPatternIntoPattern(p);
			printPattern(pattern);
			break;
		case 2:
			printf("Select a noisy pattern to be fed to the network. (1-%d) : ",NUM_DATA);
			cin>>p;
			p--;
			if (p<0 || p>=NUM_DATA)
				break;
			//Load a noisy pattern into the pattern array and print it
			putPattern_IntoPattern(p);
			printf("Noisy pattern:\n");
			printPattern(pattern);
			//Feed the pattern to the network
			net.setInput(pattern);
			if (net.propagate(pattern,1000))
				cout<<"Converged after "<<net.getTime()<<" iterations to:"<<endl;
			else
				cout<<"Did NOT converge even after 1000 iterations\n";
			printPattern(net.getOutput());
		}
	}
	while (choice!=3);
	return 0;
}
