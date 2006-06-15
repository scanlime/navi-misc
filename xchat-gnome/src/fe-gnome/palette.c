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
	{0, 0xffff, 0x0000, 0x0000}, /* foreground */
	{0, 0x0000, 0xffff, 0x0000}, /* background */
	{0, 0x0000, 0x0000, 0xffff}, /* marktext fore */
	{0, 0xffff, 0xffff, 0x0000}, /* marktext back */
	{0, 0xffff, 0x0000, 0xffff}, /* away user */
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

/*
 * These numbers are completely arbitrary decisions, uninformed by the experts
 * at crayola.  They seem to match up with my knowledge of color response in
 * the human eye, so maybe it's not complete bunk.  These colors are defined as
 * wedges within the HSV color space -- while they're not fully inclusive, they
 * are "safe" in that anywhere within a region can be called that color.
 */
float color_wedges[][6] = {
	//H min,  H max, S min,  S max, V min,  V max
	{  0.0f,   0.0f,  0.0f,   0.0f, 75.0f, 100.0f}, /*  0 white       */
	{  0.0f,   0.0f,  0.0f,   0.0f,  0.0f,  25.0f}, /*  1 black       */
	{220.0f, 235.0f, 80.0f, 100.0f, 40.0f,  70.0f}, /*  2 dark blue   */
	{110.0f, 150.0f, 80.0f, 100.0f, 40.0f,  70.0f}, /*  3 dark green  */
	{355.0f,   5.0f, 80.0f, 100.0f, 40.0f,  70.0f}, /*  4 dark red    */
	{ 10.0f,  25.0f, 80.0f, 100.0f, 50.0f,  70.0f}, /*  5 brown       */
	{270.0f, 300.0f, 80.0f, 100.0f, 35.0f,  80.0f}, /*  6 purple      */
	{ 10.0f,  30.0f, 90.0f, 100.0f, 85.0f, 100.0f}, /*  7 orange      */
	{ 55.0f,  65.0f, 90.0f, 100.0f, 90.0f, 100.0f}, /*  8 yellow      */
	{100.0f, 120.0f, 80.0f, 100.0f, 70.0f, 100.0f}, /*  9 light green */
	{170.0f, 180.0f, 70.0f,  90.0f, 55.0f,  70.0f}, /* 10 teal        */
	{175.0f, 190.0f, 80.0f, 100.0f, 70.0f,  85.0f}, /* 11 cyan        */
	{210.0f, 230.0f, 70.0f, 100.0f, 70.0f, 100.0f}, /* 12 light blue  */
	{340.0f, 360.0f, 15.0f,  35.0f, 80.0f, 100.0f}, /* 13 pink        */
	{  0.0f,   0.0f,  0.0f,   0.0f, 30.0f,  60.0f}, /* 14 grey        */
	{  0.0f,   0.0f,  0.0f,   0.0f, 50.0f,  80.0f}, /* 15 light grey  */
};

GdkColor custom_palette[32];
GdkColor optimized_palette[32];

const GdkColor *palette_schemes[] =
{
	palette_black_on_white,
	palette_white_on_black,
	custom_palette,
	optimized_palette,
};

#define MIN3(x, y, z) (((x) < (y) && (x) < (z)) ? (x) : ((y) < (x) && (y) < (z)) ? (y) : z)
#define MAX3(x, y, z) (((x) > (y) && (x) > (z)) ? (x) : ((y) > (x) && (y) > (z)) ? (y) : z)

static void
rgb_to_hsv (guint16 r, guint16 g, guint16 b, float *h, float *s, float *v)
{
	float rf, gf, bf, color_min, color_max, delta;

	rf = ((float) r) / 65535;
	gf = ((float) g) / 65535;
	bf = ((float) b) / 65535;

	color_min = MIN3 (rf, gf, bf);
	color_max = MAX3 (rf, gf, bf);

	*v = color_max;
	delta = color_max - color_min;

	if (color_max != 0.0f) {
		*s = (delta / color_max);
	} else {
		/*
		 * R = G = B = 0; Black, at the bottom of the HSV hexacone.
		 * S = 0, H and V are undefined. H = S = V = 0 seems like a
		 * reasonable representation.
		 */
		*h = *s = *v = 0.0f;
		return;
	}

	if (delta == 0.0f) {
		// Achromatic (grey)
		*h = 0.0f;
		*s = 0.0f;
		*v = color_max * 100;
		return;
	} else {
		if (rf == color_max) {
			*h = (gf - bf) / delta;
		} else if (gf == color_max) {
			*h = 2 + (bf - rf) / delta;
		} else {
			*h = 4 + (rf - gf) / delta;
		}
	}

	// Convert to degrees
	*h = *h * 60;
	if (*h < 0) {
		*h = *h + 360;
	}
	*s = *s * 100;
	*v = *v * 100;
}

static void
hsv_to_rgb (float h, float s, float v, guint16 *r, guint16 *g, guint16 *b)
{
	int i;
	float f, p, q, t;

	h /= 60.0f;
	s /= 100.0f;
	v /= 100.0f;

	if (s == 0.0f) {
		// Achromatic (grey)
		*r = (int) (v * 0xffff);
		*g = (int) (v * 0xffff);
		*b = (int) (v * 0xffff);
		return;
	}

	i = (int) floor (h);
	f = h - i;
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));

	// This selects the proper section of the HSV hexacone
	switch (i) {
	case 0:
		*r = (int) (v * 0xffff);
		*g = (int) (t * 0xffff);
		*b = (int) (p * 0xffff);
		return;
	case 1:
		*r = (int) (q * 0xffff);
		*g = (int) (v * 0xffff);
		*b = (int) (p * 0xffff);
		return;
	case 2:
		*r = (int) (p * 0xffff);
		*g = (int) (v * 0xffff);
		*b = (int) (t * 0xffff);
		return;
	case 3:
		*r = (int) (p * 0xffff);
		*g = (int) (q * 0xffff);
		*b = (int) (v * 0xffff);
		return;
	case 4:
		*r = (int) (t * 0xffff);
		*g = (int) (p * 0xffff);
		*b = (int) (v * 0xffff);
		return;
	case 5:
		*r = (int) (v * 0xffff);
		*g = (int) (p * 0xffff);
		*b = (int) (q * 0xffff);
		return;
	}
}

static float
hue_distance (float ha, float hb)
{
	float hd1, hd2, hd3;

	// This is truly a pain, because it's a cylindrical coordinate system,
	// and angular values are modulo 360.
	hd1 = fabs (hb - ha);
	hd2 = fabs (hb - ha - 360);
	hd3 = fabs (hb - ha + 360);
	return MIN3 (hd1, hd2, hd3);
}

static float
hsv_distance (float ha, float sa, float va, float hb, float sb, float vb)
{
	float dh, ds, dv;

	dh = hue_distance (ha, hb);
	ds = fabs (sb - sa);
	dv = fabs (vb - va);

	// We bias the value component here, because it is principal in our
	// perception of contrast.
	return (sqrt (dh*dh + ds*ds) + dv);
}

/*
 * This optimizes the palette for the current background color.  It does this
 * by defining colors not as a specific point but as a region in HSV space,
 * and then choosing the corner of that region which is "furthest" from the
 * background color by a sort of distance metric which is biased for maximum
 * contrast.  This makes it so things are generally readable, no matter what
 * the chosen background color.
 */
static void
optimize_palette (guint16 r, guint16 g, guint16 b)
{
	int i, j;
	float h, s, v;

	rgb_to_hsv(r, g, b, &h, &s, &v);

	for (i = 0; i < 16; i++) {
		int max_color;
		float max_dist;
		float hd;
		float points[8][3];
		GdkColor color;

		points[0][0] = color_wedges[i][0]; points[0][1] = color_wedges[i][2]; points[0][2] = color_wedges[i][4];
		points[1][0] = color_wedges[i][0]; points[1][1] = color_wedges[i][2]; points[1][2] = color_wedges[i][5];
		points[2][0] = color_wedges[i][0]; points[2][1] = color_wedges[i][3]; points[2][2] = color_wedges[i][4];
		points[3][0] = color_wedges[i][0]; points[3][1] = color_wedges[i][3]; points[3][2] = color_wedges[i][5];
		points[4][0] = color_wedges[i][1]; points[4][1] = color_wedges[i][2]; points[4][2] = color_wedges[i][4];
		points[5][0] = color_wedges[i][1]; points[5][1] = color_wedges[i][2]; points[5][2] = color_wedges[i][5];
		points[6][0] = color_wedges[i][1]; points[6][1] = color_wedges[i][3]; points[6][2] = color_wedges[i][4];
		points[7][0] = color_wedges[i][1]; points[7][1] = color_wedges[i][3]; points[7][2] = color_wedges[i][5];

		max_dist = 0;
		max_color = 0;
		for (j = 0; j < 8; j++) {
			float distance = hsv_distance(h, s, v, points[j][0], points[j][1], points[j][2]);
			if (distance > max_dist) {
				max_dist = distance;
				max_color = j;
			}
		}

		/*
		 * If the distance in hue is really short, extend the value
		 * component a little further.  This may push it outside the
		 * bounds of the wedge we've specified, but it keeps things
		 * readable when the background is right smack in the middle of
		 * a wedge.  However, we only want to do this when the
		 * saturation is high enough for it to matter (white, grey and
		 * black have an undefined hue value).
		 */
		hd = hue_distance(points[max_color][0], h);
		if (hd <= 70.0f && points[max_color][1] >= 20.0f && s >= 20.0f) {
			float vd = points[max_color][2] - v;
			v = v + (vd * 1.1);
			if (v < 0)
				v = 0;
			if (v > 100)
				v = 100;
			points[max_color][2] = v;
		}

		color.pixel = 0;
		hsv_to_rgb(points[max_color][0],
		           points[max_color][1],
		           points[max_color][2],
		           &color.red,
		           &color.green,
		           &color.blue);

		optimized_palette[i]      = color;
		optimized_palette[i + 16] = color;
	}
}

static void
extract_theme_colors (void)
{
		GtkWidget* w;
		w = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_widget_ensure_style (w);
		theme_colors[0] = w->style->text[GTK_STATE_NORMAL];
		theme_colors[1] = w->style->base[GTK_STATE_NORMAL];
		theme_colors[2] = w->style->text[GTK_STATE_SELECTED];
		theme_colors[3] = w->style->base[GTK_STATE_SELECTED];
		theme_colors[4] = w->style->text[GTK_STATE_INSENSITIVE];
		optimize_palette (theme_colors[1].red,
		                  theme_colors[1].green,
		                  theme_colors[1].blue);
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

	if (selection == 3) {
		extract_theme_colors();
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
