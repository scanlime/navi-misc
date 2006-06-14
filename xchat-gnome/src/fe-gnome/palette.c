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

#include <config.h>
#include <glib/gi18n.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gnome.h>
#include <math.h>
#include "palette.h"
#include "preferences.h"
#include "../common/xchat.h"
#include "../common/util.h"
#include "../common/cfgfiles.h"


GdkColor colors[] =
{
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

	{0, 0xcccc, 0xcccc, 0xcccc}, /* 16 white */
	{0, 0x0000, 0x0000, 0x0000}, /* 17 black */
	{0, 0x0000, 0x0000, 0xcccc}, /* 18 blue */
	{0, 0x0000, 0x9999, 0x0000}, /* 19 green */
	{0, 0xcccc, 0x0000, 0x0000}, /* 20 red */
	{0, 0xaaaa, 0x0000, 0x0000}, /* 21 light red */
	{0, 0xaaaa, 0x0000, 0xaaaa}, /* 22 purple */
	{0, 0x9999, 0x3333, 0x0000}, /* 23 orange */
	{0, 0xffff, 0xaaaa, 0x0000}, /* 24 yellow */
	{0, 0x0000, 0xffff, 0x0000}, /* 25 green */
	{0, 0x0000, 0x5555, 0x5555}, /* 26 aqua */
	{0, 0x3333, 0x9999, 0x7f7f}, /* 27 light aqua */
	{0, 0x0000, 0x0000, 0xffff}, /* 28 blue */
	{0, 0xffff, 0x3333, 0xffff}, /* 29 light purple */
	{0, 0x7f7f, 0x7f7f, 0x7f7f}, /* 30 grey */
	{0, 0x9595, 0x9595, 0x9595}, /* 31 light grey */

	{0, 0x0000, 0x0000, 0x0000}, /* 32 marktext Fore (black)     - 256 */
	{0, 0xa4a4, 0xdfdf, 0xffff}, /* 33 marktext Back (blue)      - 257 */
	{0, 0xdf3c, 0xdf3c, 0xdf3c}, /* 34 foreground (white)        - 258 */
	{0, 0x0000, 0x0000, 0x0000}, /* 35 background (black)        - 259 */
	{0, 0xcccc, 0x0000, 0x0000}, /* 36 marker line (red)         - 260 */
	{0, 0x8c8c, 0x1010, 0x1010}, /* 37 tab New Data (dark red)   - 261 */
	{0, 0x0000, 0x0000, 0xffff}, /* 38 tab Nick Mentioned (blue) - 262 */
	{0, 0xf5f5, 0x0000, 0x0000}, /* 39 tab New Message (red)     - 263 */
	{0, 0x9999, 0x9999, 0x9999}, /* 40 away user (grey)          - 264 */
};

#define MAX_COL 40

const GdkColor colors_white_on_black[] =
{
	{0, 0xdf3c, 0xdf3c, 0xdf3c}, /* foreground (white) */
	{0, 0x0000, 0x0000, 0x0000}, /* background (black) */
	{0, 0xdf3c, 0xdf3c, 0xdf3c}, /* marktext fore (white) */
	{0, 0xa4a4, 0xdfdf, 0xffff}, /* marktext back (blue) */
	{0, 0x9999, 0x9999, 0x9999}, /* away user (grey) */
};

const GdkColor colors_black_on_white[] =
{
	{0, 0x0000, 0x0000, 0x0000}, /* foreground (black) */
	{0, 0xffff, 0xffff, 0xffff}, /* background (white) */
	{0, 0x0000, 0x0000, 0x0000}, /* marktext fore (black) */
	{0, 0xa4a4, 0xdfdf, 0xffff}, /* marktext back (blue) */
	{0, 0x9999, 0x9999, 0x9999}, /* away user (grey) */
};

GdkColor theme_colors[] =
{
	{0, 0xffff, 0x0000, 0x0000}, /* foreground (black) */
	{0, 0x0000, 0xffff, 0x0000}, /* background (white) */
	{0, 0x0000, 0x0000, 0xffff}, /* marktext fore (black) */
	{0, 0xffff, 0xffff, 0x0000}, /* marktext back (blue) */
	{0, 0xffff, 0x0000, 0xffff}, /* away user (grey) */
};

GdkColor custom_colors[9];

const GdkColor *color_schemes[] =
{
	colors_black_on_white,
	colors_white_on_black,
	custom_colors,
	theme_colors,
};

const GdkColor palette_black_on_white[] =
{
	{0, 0xcccc, 0xcccc, 0xcccc}, /* 1  white */
	{0, 0x0000, 0x0000, 0x0000}, /* 2  black */
	{0, 0x0000, 0x0000, 0xcccc}, /* 3  blue */
	{0, 0x0000, 0x9999, 0x0000}, /* 4  green */
	{0, 0xcccc, 0x0000, 0x0000}, /* 5  red */
	{0, 0xaaaa, 0x0000, 0x0000}, /* 6  light red */
	{0, 0xaaaa, 0x0000, 0xaaaa}, /* 7  purple */
	{0, 0x9999, 0x3333, 0x0000}, /* 8  orange */
	{0, 0xffff, 0xaaaa, 0x0000}, /* 9  yellow */
	{0, 0x0000, 0xffff, 0x0000}, /* 10 green */
	{0, 0x0000, 0x5555, 0x5555}, /* 11 aqua */
	{0, 0x3333, 0x9999, 0x7f7f}, /* 12 light aqua */
	{0, 0x0000, 0x0000, 0xffff}, /* 13 blue */
	{0, 0xffff, 0x3333, 0xffff}, /* 14 light purple */
	{0, 0x7f7f, 0x7f7f, 0x7f7f}, /* 15 grey */
	{0, 0x9595, 0x9595, 0x9595}, /* 16 light grey */

	{0, 0xcccc, 0xcccc, 0xcccc}, /* 16 white */
	{0, 0x0000, 0x0000, 0x0000}, /* 17 black */
	{0, 0x0000, 0x0000, 0xcccc}, /* 18 blue */
	{0, 0x0000, 0x9999, 0x0000}, /* 19 green */
	{0, 0xcccc, 0x0000, 0x0000}, /* 20 red */
	{0, 0xaaaa, 0x0000, 0x0000}, /* 21 light red */
	{0, 0xaaaa, 0x0000, 0xaaaa}, /* 22 purple */
	{0, 0x9999, 0x3333, 0x0000}, /* 23 orange */
	{0, 0xffff, 0xaaaa, 0x0000}, /* 24 yellow */
	{0, 0x0000, 0xffff, 0x0000}, /* 25 green */
	{0, 0x0000, 0x5555, 0x5555}, /* 26 aqua */
	{0, 0x3333, 0x9999, 0x7f7f}, /* 27 light aqua */
	{0, 0x0000, 0x0000, 0xffff}, /* 28 blue */
	{0, 0xffff, 0x3333, 0xffff}, /* 29 light purple */
	{0, 0x7f7f, 0x7f7f, 0x7f7f}, /* 30 grey */
	{0, 0x9595, 0x9595, 0x9595}, /* 31 light grey */
};

const GdkColor palette_white_on_black[] =
{
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

	{0, 0xcf3c, 0xcf3c, 0xcf3c}, /* 16 white */
	{0, 0x0000, 0x0000, 0x0000}, /* 17 black */
	{0, 0x0000, 0x0000, 0xcccc}, /* 18 blue */
	{0, 0x0000, 0xcccc, 0x0000}, /* 19 green */
	{0, 0xdddd, 0x0000, 0x0000}, /* 20 red */
	{0, 0xaaaa, 0x0000, 0x0000}, /* 21 light red */
	{0, 0xbbbb, 0x0000, 0xbbbb}, /* 22 purple */
	{0, 0xffff, 0xaaaa, 0x0000}, /* 23 orange */
	{0, 0xeeee, 0xdddd, 0x2222}, /* 24 yellow */
	{0, 0x3333, 0xdede, 0x5555}, /* 25 green */
	{0, 0x0000, 0xcccc, 0xcccc}, /* 26 aqua */
	{0, 0x3333, 0xeeee, 0xffff}, /* 27 light aqua */
	{0, 0x0000, 0x0000, 0xffff}, /* 28 blue */
	{0, 0xeeee, 0x2222, 0xeeee}, /* 29 light purple */
	{0, 0x7777, 0x7777, 0x7777}, /* 30 grey */
	{0, 0x9999, 0x9999, 0x9999}, /* 31 light grey */
};

GdkColor custom_palette[32];

const GdkColor *palette_schemes[] =
{
	palette_black_on_white,
	palette_white_on_black,
	custom_palette,
	NULL,
};

void
extract_theme_colors(void) {
		GtkWidget* w;
		w = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_widget_ensure_style (w);
		theme_colors[0] = w->style->text[GTK_STATE_NORMAL];
		theme_colors[1] = w->style->base[GTK_STATE_NORMAL];
		theme_colors[2] = w->style->text[GTK_STATE_SELECTED];
		theme_colors[3] = w->style->base[GTK_STATE_SELECTED];
		theme_colors[4] = w->style->text[GTK_STATE_INSENSITIVE];
		gtk_widget_destroy (w);
}

void
load_colors (int selection)
{
	if (selection == 3)
		extract_theme_colors();
	colors[34] = color_schemes[selection][0];
	colors[35] = color_schemes[selection][1];
	colors[32] = color_schemes[selection][2];
	colors[33] = color_schemes[selection][3];
	colors[40] = color_schemes[selection][4];
}

void
load_palette (int selection)
{
	int i;
	int background_magnitude;

	if (selection == 3) {
		extract_theme_colors();
		background_magnitude = sqrt(pow(theme_colors[0].red,   2) +
		                            pow(theme_colors[0].green, 2) +
					    pow(theme_colors[0].blue,  2));
		if (background_magnitude < 0x8000) {
			selection = 0;
		} else {
			selection = 1;
		}
	}

	for (i = 0; i < 32; i++)
		colors[i] = palette_schemes[selection][i];
}

void
palette_init (void)
{
	int i, f, l;
	int red, green, blue;
	struct stat st;
	char prefname[256];
	char *cfg;

	snprintf (prefname, sizeof (prefname), "%s/colors.conf", get_xdir_fs ());
	f = open (prefname, O_RDONLY | OFLAGS);
	if (f == -1) {
		for (i = 0; i < 32; i++)
			custom_palette[i] = palette_schemes[0][i];
		for (i = 0; i < 5; i++)
			custom_colors[i] = color_schemes[0][i];
		return;
	}

	fstat (f, &st);
	cfg = g_malloc (st.st_size + 1);
	if (cfg) {
		cfg[0] = '\0';
		l = read (f, cfg, st.st_size);
		if (l >= 0)
			cfg[l] = '\0';

		for (i = 0; i < 32; i++) {
			snprintf (prefname, sizeof (prefname), "color_%d", i);
			cfg_get_color (cfg, prefname, &red, &green, &blue);
			custom_palette[i].red = red;
			custom_palette[i].green = green;
			custom_palette[i].blue = blue;
		}

		strcpy (prefname, "color_258");
		cfg_get_color (cfg, prefname, &red, &green, &blue);
		custom_colors[0].red = red;
		custom_colors[0].green = green;
		custom_colors[0].blue = blue;

		strcpy (prefname, "color_259");
		cfg_get_color (cfg, prefname, &red, &green, &blue);
		custom_colors[1].red = red;
		custom_colors[1].green = green;
		custom_colors[1].blue = blue;

		strcpy (prefname, "color_256");
		cfg_get_color (cfg, prefname, &red, &green, &blue);
		custom_colors[2].red = red;
		custom_colors[2].green = green;
		custom_colors[2].blue = blue;

		strcpy (prefname, "color_257");
		cfg_get_color (cfg, prefname, &red, &green, &blue);
		custom_colors[3].red = red;
		custom_colors[3].green = green;
		custom_colors[3].blue = blue;

		strcpy (prefname, "color_264");
		cfg_get_color (cfg, prefname, &red, &green, &blue);
		custom_colors[4].red = red;
		custom_colors[4].green = green;
		custom_colors[4].blue = blue;

		strcpy (prefname, "color_261");
		cfg_get_color (cfg, prefname, &red, &green, &blue);
		custom_colors[5].red = red;
		custom_colors[5].green = green;
		custom_colors[5].blue = blue;

		strcpy (prefname, "color_262");
		cfg_get_color (cfg, prefname, &red, &green, &blue);
		custom_colors[6].red = red;
		custom_colors[6].green = green;
		custom_colors[6].blue = blue;

		strcpy (prefname, "color_263");
		cfg_get_color (cfg, prefname, &red, &green, &blue);
		custom_colors[7].red = red;
		custom_colors[7].green = green;
		custom_colors[7].blue = blue;

		strcpy (prefname, "color_260");
		cfg_get_color (cfg, prefname, &red, &green, &blue);
		custom_colors[8].red = red;
		custom_colors[8].green = green;
		custom_colors[8].blue = blue;

		free (cfg);
	}
	close (f);
}

void
palette_save (void)
{
	int i, f;
	char prefname[256];

	snprintf (prefname, sizeof (prefname), "%s/colors.conf", get_xdir_fs ());
	f = open (prefname, O_TRUNC | O_WRONLY | O_CREAT | OFLAGS, 0600);
	if (f != -1) {
		for (i = 0; i < 32; i++) {
			snprintf (prefname, sizeof (prefname), "color_%d", i);
			cfg_put_color (f, custom_palette[i].red, custom_palette[i].green, custom_palette[i].blue, prefname);
		}

		cfg_put_color (f, custom_colors[0].red, custom_colors[0].green, custom_colors[0].blue, "color_258");
		cfg_put_color (f, custom_colors[1].red, custom_colors[1].green, custom_colors[1].blue, "color_259");
		cfg_put_color (f, custom_colors[2].red, custom_colors[2].green, custom_colors[2].blue, "color_256");
		cfg_put_color (f, custom_colors[3].red, custom_colors[3].green, custom_colors[3].blue, "color_257");
		cfg_put_color (f, custom_colors[4].red, custom_colors[4].green, custom_colors[4].blue, "color_264");
		cfg_put_color (f, custom_colors[5].red, custom_colors[5].green, custom_colors[5].blue, "color_261");
		cfg_put_color (f, custom_colors[6].red, custom_colors[6].green, custom_colors[6].blue, "color_262");
		cfg_put_color (f, custom_colors[7].red, custom_colors[7].green, custom_colors[7].blue, "color_263");
		cfg_put_color (f, custom_colors[8].red, custom_colors[8].green, custom_colors[8].blue, "color_260");

		close (f);
	}
}

void
palette_alloc (GtkWidget *widget)
{
	static gboolean done = FALSE;
	GdkColormap *cmap;
	int i;

	cmap = gtk_widget_get_colormap (widget);

	if (done)
		gdk_colormap_free_colors (cmap, colors, 40);

	for (i = 0; i < 40; i++)
		gdk_colormap_alloc_color (cmap, &colors[i], FALSE, TRUE);

	done = TRUE;
}
