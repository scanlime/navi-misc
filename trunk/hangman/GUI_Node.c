/*
 * GUI_Node.c
 *
 * W. Evan Sheehan
 * evan@navi.picogui.org
 *
 * Implementation of nodes for dividing window into rectangles for drawing.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL_gfxPrimitives.h>

#include "GUI.h"

/********************************* Initialize ******************************/
GUI_Node* GUI_Node_Init()
{
	GUI_Node* node;
	
	node = malloc(sizeof(GUI_Node));
	node->x = 0;
	node->y = 0;
	node->h = 0;
	node->w = 0;

	node->percent = 0;

	node->color = 0;
	
	node->split = 0;
	node->draw = 0;

	node->left = 0;
	node->right = 0;

	return node;
}

/**************************** Rectangle Splitting ***************************/
int split_horizontal(GUI_Node *node)
{
	/* Create children. */
	if(!node->left) {
		node->left = malloc(sizeof(GUI_Node));
		GUI_Node_Init(node->left);
	}
	if(!node->right) {
		node->right = malloc(sizeof(GUI_Node));
		GUI_Node_Init(node->right);
	}

	/* Left node:
	 * x,y coordinates.
	 */
	node->left->x = node->x;
	node->left->y = node->y;
	
	/* width and height. */
	node->left->w = node->w;
	node->left->h = (node->h * node->percent) / 100;

	/* Right node:
	 * x,y coordinates.
	 */
	node->right->x = node->x;
	node->right->y = node->y + node->left->h;

	/* width and height. */
	node->right->w = node->w;
	node->right->h = node->h - node->left->h;

	return 0;
}

/******************************************************************/
int split_vertical(GUI_Node *node)
{
	/* Create child nodes. */
	node->left = malloc(sizeof(GUI_Node));
	node->right = malloc(sizeof(GUI_Node));

	/* Initialize nodes. */
	GUI_Node_Init(node->left);
	GUI_Node_Init(node->right);

	/* Left node:
	 * x,y coordinates.
	 */
	node->left->x = node->x;
	node->left->y = node->y;

	/* width and height. */
	node->left->w = (node->w * node->percent) / 100;
	node->left->h = node->h;

	/* Right node:
	 * x,y coordinates.
	 */
	node->right->x = node->x + node->left->w;
	node->right->y = node->y;

	/* width and height. */
	node->right->w = node->w - node->left->w;
	node->right->h = node->h;

	return 0;
}

/*********************************************************************/
int margin(GUI_Node *node)
{
	/* Create childe node. */
	node->left = malloc(sizeof(GUI_Node));

	/* Initialize */
	GUI_Node_Init(node->left);

	/* scale down width and height */
	node->left->h = (node->h*node->percent)/100;
	node->left->w = (node->w*node->percent)/100;

	/* arithmetic is wrong here? */
	/* x and y of new node */
	node->left->x = node->x +((node->h - node->left->h)/2);
	node->left->y = node->y + ((node->w - node->left->w)/2);

	return 0;
}

/******************************* Node combining *****************************/
GUI_Node* Combine_Horz(GUI_Node* leftNode, GUI_Node* rightNode, int prcnt)
{
	GUI_Node* tmpNode;

	tmpNode = GUI_Node_Init();

	tmpNode->percent = prcnt;
	tmpNode->split = split_horizontal;

	tmpNode->left = leftNode;
	tmpNode->right = rightNode;

	return tmpNode;
}

/***********************************/
GUI_Node* Combine_Vert(GUI_Node* leftNode, GUI_Node* rightNode, int prcnt)
{
	GUI_Node* tmpNode;

	tmpNode = GUI_Node_Init();

	tmpNode->percent = prcnt;
	tmpNode->split = split_vertical;

	tmpNode->left = leftNode;
	tmpNode->right = rightNode;

	return tmpNode;
}

/******************************* Window Elements ****************************/
GUI_Node* make_button()
{
	GUI_Node* newButton;

	/* Initialize */
	newButton = malloc(sizeof(GUI_Node));
	GUI_Node_Init(newButton);

	newButton->draw = draw_button;
	newButton->action = button_control;

	newButton->color = malloc(sizeof(SDL_Color));
	newButton->color->r = 150;
	newButton->color->g = 150;
	newButton->color->b = 150;

	return newButton;
}

/*********************************************/
GUI_Node* make_textfield()
{
	GUI_Node* newField;

	newField = GUI_Node_Init();

	newField->color = malloc(sizeof(SDL_Color));
	newField->color->r = 0;
	newField->color->g = 0;
	newField->color->b = 0;

	/* Initialize */
	newField = malloc(sizeof(GUI_Node));
	GUI_Node_Init(newField);

	newField->draw = draw_textfield;
	newField->action = textfield_control;

	return newField;
}

/********************************************/
GUI_Node* make_drawfield()
{
	GUI_Node* newField;

	newField = GUI_Node_Init();

	newField->color = malloc(sizeof(SDL_Color));
	newField->color->r = 0;
	newField->color->g = 0;
	newField->color->b = 0;

	/* Initialize */
	newField = malloc(sizeof(GUI_Node));
	GUI_Node_Init(newField);

	newField->draw = draw_drawfield;
	newField->action = drawfield_control;

	return newField;
}
/******************************** Draw Functions *******************************/

void draw_tree(GUI_Node* startNode, SDL_Surface* dest)
{
	/* If node has a draw function call it */
	if (startNode->draw)
		startNode->draw(startNode, dest);
	
	/* If node has a split funciton, split it */
	if (startNode->split) 
		startNode->split(startNode);

	/* Recurse */
	if (startNode->left)
		draw_tree(startNode->left, dest);

	if (startNode->right)
		draw_tree(startNode->right, dest);
}

/*************************************/
int draw_button(GUI_Node *node, SDL_Surface *dest)
{
	SDL_Surface *char_surface;
	TTF_Font *font;
	SDL_Color textColor;

	boxRGBA(dest, node->x, node->y, node->x+node->w, node->y+node->h,
			node->color->r, node->color->g, node->color->b, 255);
	rectangleRGBA(dest, node->x, node->y, node->x+node->w, node->y+node->h,
			255,255,255, 255);

	if (node->text) {
		if (TTF_Init() == -1) 
			return -1;
		font = TTF_OpenFont("/home/evan/bitstream/VeraMono.ttf", ((node->w * node->h) * 33)/100);
		if (!font)
			return -1;

		textColor.r = 0;
		textColor.g = 0;
		textColor.b = 0;
		char_surface = TTF_RenderText_Solid(font, node->text, textColor);
	}

	return 0;
}

/************************************/
int draw_textfield(GUI_Node *node, SDL_Surface *dest)
{
	if(!node)
		return 1;
	if (node->color)
		boxRGBA(dest, node->x, node->y, node->x+node->w, node->y+node->h,
				node->color->r, node->color->g, node->color->b, 255);
	else
		boxRGBA(dest, node->x, node->y, node->x+node->w, node->y+node->h,
				150,150,150,255);
	return 0;
}

/************************************/
int draw_drawfield(GUI_Node *node, SDL_Surface *dest)
{
	boxRGBA(dest, node->x, node->y, node->x+node->w, node->y+node->h,
			node->color->r, node->color->g, node->color->b, 255);

	return 0;
}
/********************************** Controls ***********************************/
void button_control(GUI_Node *node)
{
}

/************************************/
void textfield_control(GUI_Node *node)
{
}

/************************************/
void drawfield_control(GUI_Node *node)
{
}
