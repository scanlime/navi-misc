/* X-Chat
 * Copyright (C) 1998 Peter Zelezny.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gnome.h>
#include "preferences.h"
#include "../common/xchat.h"
#include "../common/util.h"
#include "../common/cfgfiles.h"


GdkColor colors[] = {
	{0, 0xcf3c, 0xcf3c, 0xcf3c}, /* 0  white */
	{0, 0x0000, 0x0000, 0x0000}, /* 1  black */
	{0, 0x0000, 0x0000, 0xcccc}, /* 2  blue */
	{0, 0x0000, 0xcccc, 0x0000}, /* 3  green */
	{0, 0xdddd, 0x0000, 0x0000}, /* 4  red */
	{0, 0xaaaa, 0x0000, 0x0000}, /* 5  light red */
	{0, 0xbbbb, 0x0000, 0xbbbb}, /* 6  purple */
	{0, 0xffff, 0xaaaa, 0x0000}, /* 7  orange */
	{0, 0xeeee, 0xdddd, 0x2222}, /* 8  yellow */
	{0, 0x3333, 0xdede, 0x5555}, /* 9  green */
	{0, 0x0000, 0xcccc, 0xcccc}, /* 10 aqua */
	{0, 0x3333, 0xeeee, 0xffff}, /* 11 light aqua */
	{0, 0x0000, 0x0000, 0xffff}, /* 12 blue */
	{0, 0xeeee, 0x2222, 0xeeee}, /* 13 light purple */
	{0, 0x7777, 0x7777, 0x7777}, /* 14 grey */
	{0, 0x9999, 0x9999, 0x9999}, /* 15 light grey */
	{0, 0xa4a4, 0xdfdf, 0xffff}, /* 16 marktext Back (blue) */
	{0, 0x0000, 0x0000, 0x0000}, /* 17 marktext Fore (black) */
	{0, 0xdf3c, 0xdf3c, 0xdf3c}, /* 18 foreground (white) */
	{0, 0x0000, 0x0000, 0x0000}, /* 19 background (black) */
	{0, 0x8c8c, 0x1010, 0x1010}, /* 20 tab New Data (dark red) */
	{0, 0x0000, 0x0000, 0xffff}, /* 21 tab Nick Mentioned (blue) */
	{0, 0xf5f5, 0x0000, 0x0000}, /* 22 tab New Message (red) */
	{0, 0x9999, 0x9999, 0x9999}, /* 23 away user (grey) */
};

const GdkColor colors_white_on_black[] = {
	{0, 0xffff, 0xffff, 0xffff}, /* background (white) */
	{0, 0x0000, 0x0000, 0x0000}, /* foreground (black) */
	{0, 0xa4a4, 0xdfdf, 0xffff}, /* marktext back (blue) */
	{0, 0x0000, 0x0000, 0x0000}, /* marktext fore (black) */
	{0, 0x9999, 0x9999, 0x9999}, /* away user (grey) */
};

const GdkColor colors_black_on_white[] = {
	{0, 0x0000, 0x0000, 0x0000}, /* background (black) */
	{0, 0xdf3c, 0xdf3c, 0xdf3c}, /* foreground (white) */
	{0, 0xa4a4, 0xdfdf, 0xffff}, /* marktext back (blue) */
	{0, 0xdf3c, 0xdf3c, 0xdf3c}, /* marktext fore (white) */
	{0, 0x9999, 0x9999, 0x9999}, /* away user (grey) */
};

GdkColor custom_colors[5];

const GdkColor *color_schemes[] = {
	colors_white_on_black,
	colors_black_on_white,
	custom_colors,
};

const GdkColor default_palette[] = {
	{0, 0xcf3c, 0xcf3c, 0xcf3c}, /* 0  white */
	{0, 0x0000, 0x0000, 0x0000}, /* 1  black */
	{0, 0x0000, 0x0000, 0xcccc}, /* 2  blue */
	{0, 0x0000, 0xcccc, 0x0000}, /* 3  green */
	{0, 0xdddd, 0x0000, 0x0000}, /* 4  red */
	{0, 0xaaaa, 0x0000, 0x0000}, /* 5  light red */
	{0, 0xbbbb, 0x0000, 0xbbbb}, /* 6  purple */
	{0, 0xffff, 0xaaaa, 0x0000}, /* 7  orange */
	{0, 0xeeee, 0xdddd, 0x2222}, /* 8  yellow */
	{0, 0x3333, 0xdede, 0x5555}, /* 9  green */
	{0, 0x0000, 0xcccc, 0xcccc}, /* 10 aqua */
	{0, 0x3333, 0xeeee, 0xffff}, /* 11 light aqua */
	{0, 0x0000, 0x0000, 0xffff}, /* 12 blue */
	{0, 0xeeee, 0x2222, 0xeeee}, /* 13 light purple */
	{0, 0x7777, 0x7777, 0x7777}, /* 14 grey */
	{0, 0x9999, 0x9999, 0x9999}, /* 15 light grey */
};

GdkColor custom_palette[16];

const GdkColor *palette_schemes[] = {
	default_palette,
	custom_palette,
};

void load_colors(int selection) {
	colors[19] = color_schemes[selection][0];
	colors[18] = color_schemes[selection][1];
	colors[16] = color_schemes[selection][2];
	colors[17] = color_schemes[selection][3];
	colors[23] = color_schemes[selection][4];
}

void load_palette(int selection) {
	int i;

	for(i = 0; i < 16; i++) {
		colors[i] = palette_schemes[selection][i];
	}
}

void palette_init() {
	int i;

	for(i = 0; i < 16; i++) {
		custom_palette[i] = palette_schemes[0][i];
	}
	for(i = 0; i < 5; i++) {
		custom_colors[i] = color_schemes[0][i];
	}
}

void palette_alloc(GtkWidget *widget) {
	static gboolean done = FALSE;
	GdkColormap *cmap;
	int i;

	cmap = gtk_widget_get_colormap(widget);

	if(done)
		gdk_colormap_free_colors(cmap, colors, 24);

	for(i = 0; i < 24; i++)
		gdk_colormap_alloc_color(cmap, &colors[i], FALSE, TRUE);

	done = TRUE;
}

#define MAX_COL 23
