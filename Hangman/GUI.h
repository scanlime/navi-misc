/*
 * GUI.h
 *
 * W. Evan Sheehan
 * evan@navi.picogui.org
 *
 * A simple library of GUI tools using SDL.
 *
 */

#include <SDL/SDL.h>
/* Division type of the rectangle. */
enum div_t{UNSET, HORIZONTAL, VERTICAL, MARGIN};

typedef struct GUI_Node {
	/* x and y are the coordinates of the top left corner of the rectangle.
	 * h and w are the height and width of the rectangle.
	 */
	int x, y, h, w;

	/* Percentage to split the node by. */
	int percent; 
	
	/* Division of the rectangle: HORIZONTAL, VERTICAL, MARGIN or UNSET. */
	enum div_t division;

	/* Pointer to the split function for the node. */
	void (*split)(void);

	/* Pointer to the draw function for the node. */
	struct GUI_Node* (*draw)(void);

	/* Controls the behaviour of the node (e.g. text field, button, etc.) */
	void (*action)(void);

	/* Child nodes. */
	struct GUI_Node *left;
	struct GUI_Node *right;
} GUI_Node;

/* Initialize the node. */
void GUI_Node_Init(GUI_Node *node);

/************************ Rectangle Splitting *****************************/
/* Split the rectangle horizontally where the top rectangle is 'percent' the size of
 * the initial rectangle and the bottom is the remaining part.
 */
void split_horizontal(GUI_Node *node);

/* Split the rectangle vertically where the left rectangle is 'percent' the size of the
 * initial rectangle and the right rectangle is the remaining space.
 */
void split_vertical(GUI_Node *node);

/* Doesn't split the rectangle into two rectangles, but creates a rectangle smaller then
 * node by percent and puts it in the left child of node; the right child remains NULL.
 */
void margin(GUI_Node *node);

/**************************** Window Elements *****************************/
/* Produces a button node.
 */
GUI_Node* make_button();

/* Produces a text field node. 
 */
GUI_Node* make_textfield();

/* Produces a drawing field node.
 */
GUI_Node* make_drawfield();

/**************************** Drawing Functions *****************************/

/* Draw the image of the button to the rectangle.  node is the rectangle to draw the button
 * to, text is printed in the button.
 */
void draw_button(GUI_Node *node, SDL_Surface *dest, char* text);

/* Draw the image of the text field to the rectangle. Write text to the screen,
 * leave blank if text is NULL.
 */
void draw_textfield(GUI_Node *node, SDL_Surface *dest, char* text);

/* Draw the image of the drawing field to the rectangle. Draw the image to the 
 * rectangle.  Draw nothing if image is NULL.
 */
void draw_drawfield(GUI_Node *node, SDL_Surface* image);

/* Resize the entire layout tree and re-blit everything.
 */
void resize(GUI_Node* node);

/********************************* Controls **********************************/
void button_control();

void textfield_control();

void drawfield_control();
