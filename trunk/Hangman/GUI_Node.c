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
void GUI_Node_Init(GUI_Node *node)
{
	node->x = 0;
	node->y = 0;
	node->h = 0;
	node->w = 0;

	node->percent = 0;
	
	node->split = 0;
	node->draw = 0;

	node->left = 0;
	node->right = 0;
}

/**************************** Rectangle Splitting ***************************/
void split_horizontal(GUI_Node *node)
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
}

/******************************************************************/
void split_vertical(GUI_Node *node)
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
}

/*********************************************************************/
void margin(GUI_Node *node)
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

	return newButton;
}

/*********************************************/
GUI_Node* make_textfield()
{
	GUI_Node* newField;

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

	/* Initialize */
	newField = malloc(sizeof(GUI_Node));
	GUI_Node_Init(newField);

	newField->draw = draw_drawfield;
	newField->action = drawfield_control;

	return newField;
}
/******************************** Draw Functions *******************************/
int draw_button(GUI_Node *node, SDL_Surface *dest)
{
	SDL_Surface *char_surface;
	TTF_Font *font;
	SDL_Color color;

	boxRGBA(dest, node->x, node->y, node->x+node->w, node->y+node->h,
			150,150,150, 255);
	rectangleRGBA(dest, node->x, node->y, node->x+node->w, node->y+node->h,
			255,255,255, 255);

	if (node->text) {
		if (TTF_Init() == -1) 
			return -1;
		font = TTF_OpenFont("/home/evan/bitstream/VeraMono.ttf", ((node->w * node->h) * 33)/100);
		if (!label)
			return -1;
		color.r = 0;
		color.g = 0;
		color.b = 0;
		char_print = TTF_RenderText_Solid(font, node->text, color);
	}

	return 0;
}

/************************************/
int draw_textfield(GUI_Node *node, SDL_Surface *dest)
{
	boxRGBA(dest, node->x, node->y, node->x+node->w, node->y+node->h,
			0,0,0, 255);

	return 0;
}

/************************************/
int draw_drawfield(GUI_Node *node, SDL_Surface *dest)
{
	boxRGBA(dest, node->x, node->y, node->x+node->w, node->y+node->h,
			0,0,0, 255);

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
