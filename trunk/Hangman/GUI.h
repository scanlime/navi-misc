/*
 * GUI.h
 *
 * W. Evan Sheehan
 * evan@navi.picogui.org
 *
 * A simple library of GUI tools using SDL.  Used to create a layout tree, each node
 * containing a rectangle.  Rectangles can be divided horizontally or vertically and can
 * contain buttons, text fields or drawing fields.
 *
 */

#ifndef GUI
#define GUI

#include <SDL/SDL.h>

typedef struct GUI_Node {
	/* x and y are the coordinates of the top left corner of the rectangle.
	 * h and w are the height and width of the rectangle.
	 */
	int x, y, h, w;

	/* Percentage to split the node by. */
	int percent; 

	/* Contains the text to be printed in the case of either a text field or a button.
	 * In case of a drawing field can contain the path to a .png file to blit to the
	 * screen.
	 */
	char* text;

	/* In case the node needs to control the color of whatever is being drawin inside
	 */
	SDL_Color *color;
	
	/* Pointer to the split function for the node. */
	int (*split)(struct GUI_Node *node);

	/* Pointer to the draw function for the node. */
	int (*draw)(struct GUI_Node* node, SDL_Surface* dest);

	/* Controls the behaviour of the node (e.g. text field, button, etc.) */
	void (*action)(struct GUI_Node *node);

	/* Child nodes. */
	struct GUI_Node *left;
	struct GUI_Node *right;

} GUI_Node;

/* Initialize the node. */
GUI_Node* GUI_Node_Init();


/************************ Rectangle Splitting *****************************/

/* Split the rectangle horizontally where the top rectangle is 'percent' the size of
 * the initial rectangle and the bottom is the remaining part.
 */
int split_horizontal(GUI_Node *node);

/* Split the rectangle vertically where the left rectangle is 'percent' the size of the
 * initial rectangle and the right rectangle is the remaining space.
 */
int split_vertical(GUI_Node *node);

/* Doesn't split the rectangle into two rectangles, but creates a rectangle smaller then
 * node by percent and puts it in the left child of node; the right child remains NULL.
 */
int margin(GUI_Node *node);


/*************************** Node combining *******************************/

/* Combine two nodes that are to be split horizontally
 */
GUI_Node* Combine_Horz(GUI_Node* leftNode, GUI_Node* rightNode, int prcnt);

/* combine two nodes that are to be split vertically
 */
GUI_Node* Combine_Vert(GUI_Node* leftNode, GUI_Node* rightNode, int prcnt);


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

/* Redraw the entire tree, starting at startNode
 */
void draw_tree(GUI_Node* startNode, SDL_Surface* dest);

/* Draw the image of the button to the rectangle.  node is the rectangle to draw the button
 * to, text is printed in the button.
 */
int draw_button(GUI_Node *node, SDL_Surface *dest);

/* Draw the image of the text field to the rectangle. Write text to the screen,
 * leave blank if text is NULL.
 */
int draw_textfield(GUI_Node *node, SDL_Surface *dest);

/* Draw the image of the drawing field to the rectangle. Draw the image to the 
 * rectangle.  Draw nothing if image is NULL.
 */
int draw_drawfield(GUI_Node *node, SDL_Surface *dest);

/********************************* Controls **********************************/

/* Control button behaviour.
 */
void button_control(GUI_Node *node);

/* Control textfield.
 */
void textfield_control(GUI_Node *node);

/* Control draw spaces.
 */
void drawfield_control(GUI_Node *node);

#endif
