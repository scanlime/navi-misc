/* X-Chat
 * Copyright (C) 1998 Peter Zelezny.
 *
 * Palette optimization
 * Copyright (C) 2006 David Trowbridge.
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
 * at crayola.  These colors are defined as boxes within the CIE L*a*b* color
 * space -- while they're not fully inclusive, they are "safe" in that anywhere
 * within a given region is guaranteed to be the expected color.
 */
float color_regions[][6] = {
	{75.0f, 100.0f,    0.0f,   0.0f,    0.0f,   0.0f}, /*  0 white       */
	{ 0.0f,  30.0f,    0.0f,   0.0f,    0.0f,   0.0f}, /*  1 black       */
	{ 0.0f,  20.0f, -100.0f,  50.0f, -100.0f, -60.0f}, /*  2 dark blue   */
	{15.0f,  40.0f, -100.0f, -80.0f,   80.0f, 100.0f}, /*  3 green       */
	{10.0f,  40.0f,   90.0f, 100.0f,   70.0f, 100.0f}, /*  4 dark red    */
	{30.0f,  60.0f,   30.0f,  50.0f,   70.0f, 100.0f}, /*  5 brown       */
	{35.0f,  45.0f,   85.0f, 100.0f,  -90.0f, -80.0f}, /*  6 purple      */
	{65.0f,  80.0f,   20.0f,  65.0f,   90.0f, 100.0f}, /*  7 orange      */
	{90.0f, 100.0f,    5.0f,  15.0f,   92.5f, 105.0f}, /*  8 yellow      */
	{80.0f,  90.0f, -100.0f, -70.0f,   70.0f, 100.0f}, /*  9 light green */
	{40.0f,  60.0f, -100.0f, -80.0f,  -10.0f,  20.0f}, /* 10 teal        */
	{50.0f,  70.0f, -100.0f, -20.0f, -100.0f, -50.0f}, /* 11 cyan        */
	{40.0f,  50.0f, -100.0f,   0.0f, -100.0f, -60.0f}, /* 12 light blue  */
	{70.0f,  95.0f,   90.0f, 100.0f, -100.0f,   0.0f}, /* 13 pink        */
	{30.0f,  60.0f,    0.0f,   0.0f,    0.0f,   0.0f}, /* 14 grey        */
	{50.0f,  80.0f,    0.0f,   0.0f,    0.0f,   0.0f}, /* 15 light grey  */
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

static float
srgb_to_xyz_g (float K)
{
	const float a     = 0.055f;
	const float gamma = 2.4f;

	/*
	 * This performs the non-linear transformation that accounts for the
	 * gamma curve in sRGB, but avoids numerical problems.
	 */

	if (K > 0.04045)
		return pow((K + a) / (1 + a), gamma);
	else
		return K / 12.92;
}

static float
xyz_to_srgb_C (float K)
{
	const float a     = 0.055;
	const float gamma = 2.4;

	if (K > 0.00304)
		return (1 + a) * pow (K, (1.0 / gamma)) - a;
	else
		return K * 12.92;
}


static void
srgb_to_xyz (guint16 r, guint16 g, guint16 b, float *x, float *y, float *z)
{
	float gr = srgb_to_xyz_g(r / 65535.0f);
	float gg = srgb_to_xyz_g(g / 65535.0f);
	float gb = srgb_to_xyz_g(b / 65535.0f);

	*x = 0.412424f * gr + 0.357579f * gg + 0.180464f * gb;
	*y = 0.212656f * gr + 0.715158f * gg + 0.072186f * gb;
	*z = 0.019332f * gr + 0.119193f * gg + 0.950444f * gb;
}

static float
xyz_to_lab_f (float t)
{
	if (t > 0.008856f)
		return pow(t, 1/3.0f);
	else
		return 7.787*t + 16/116.0f;
}

static void
xyz_to_lab (float x, float y, float z, float *L, float *a, float *b)
{
	float Xn, Yn, Zn;
	float fy;

	/*
	 * This is our reference white point.  Since we're treating RGB as sRGB,
	 * this is the sRGB white point, that has a correlated color temperature
	 * of 6500K.
	 */
	Xn = 0.93819f;
	Yn = 0.98705f;
	Zn = 1.07475f;

	fy = xyz_to_lab_f (y / Yn);

	*L = 116 * fy - 16;
	*a = 500 * (xyz_to_lab_f (x / Xn) - fy);
	*b = 200 * (fy - xyz_to_lab_f (z / Zn));
}

static void
lab_to_xyz (float L, float a, float b, float *x, float *y, float *z)
{
	float fy, fx, fz, delta, delta2;
	float Xn, Yn, Zn;

	fy = (L + 16) / 116;
	fx = fy + a / 500;
	fz = fy - b / 200;
	delta = 6.0f / 29;
	delta2 = pow (delta, 2);

	Xn = 0.93819f;
	Yn = 0.98705f;
	Zn = 1.07475f;

	if (fx > delta)
		*x = Xn * pow (fx, 3);
	else
		*x = (fx - 16.0f/116) * 3 * delta2 * Xn;

	if (fy > delta)
		*y = Yn * pow (fy, 3);
	else
		*y = (fy - 16.0f/116) * 3 * delta2 * Yn;

	if (fz > delta)
		*z = Zn * pow (fz, 3);
	else
		*z = (fz - 16.0f/116) * 3 * delta2 * Zn;
}

static int
clamp (int C)
{
	if (C < 0)
		return 0;
	if (C > 65535)
		return 65535;
	return C;
}

static void
xyz_to_srgb (float x, float y, float z, guint16 *r, guint16 *g, guint16 *b)
{
	float rs, gs, bs;

	/* Compute the tristimulus values */
	rs =  3.2410f * x - 1.5374f * y - 0.4986f * z;
	gs = -0.9692f * x + 1.8760f * y + 0.0416f * z;
	bs =  0.0556f * x - 0.2040f * y + 1.0570f * z;

	/* Convert to 8-bit sRGB */
	*r = clamp ((int) round(xyz_to_srgb_C (rs) * 65535));
	*g = clamp ((int) round(xyz_to_srgb_C (gs) * 65535));
	*b = clamp ((int) round(xyz_to_srgb_C (bs) * 65535));
}

static void
srgb_to_lab (guint16 R, guint16 G, guint16 B, float *L, float *a, float *b)
{
	float x, y, z;

	srgb_to_xyz (R, G, B, &x, &y, &z);
	xyz_to_lab (x, y, z, L, a, b);
}

static void
lab_to_srgb (float L, float a, float b, guint16 *R, guint16 *G, guint16 *B)
{
	float x, y, z;

	lab_to_xyz (L, a, b, &x, &y, &z);
	xyz_to_srgb (x, y, z, R, G, B);
}

static float
L_distance (float la, float lb)
{
	return fabs (lb - la);
}

static float
ab_distance (float aa, float ba, float ab, float bb)
{
	float da, db;

	da = fabs (ab - aa);
	db = fabs (bb - ba);
	return sqrt (da*da + db*db);
}

static float
distance (float La, float aa, float ba, float Lb, float ab, float bb)
{
	float dL, da, db;
	dL = fabs (Lb - La);
	da = fabs (ab - aa);
	db = fabs (bb - ba);
	return sqrt (dL*dL + da*da + db*db);
}

/*
 * This optimizes the palette for the current background color.  It does this
 * by defining colors not as a specific point but as a region in L*a*b* space,
 * and then choosing the corner of that region which is "furthest" from the
 * background color by a sort of distance metric which is biased for maximum
 * contrast.  This makes it so things are generally readable, no matter what
 * the chosen background color.
 */
static void
optimize_palette (guint16 R, guint16 G, guint16 B)
{
	int i, j;
	float L, a, b;

	srgb_to_lab (R, G, B, &L, &a, &b);

	for (i = 0; i < 16; i++) {
		int max_color;
		float max_dist;
		float ld, cd;
		float points[8][3];
		GdkColor color;

		points[0][0] = color_regions[i][0]; points[0][1] = color_regions[i][2]; points[0][2] = color_regions[i][4];
		points[1][0] = color_regions[i][0]; points[1][1] = color_regions[i][2]; points[1][2] = color_regions[i][5];
		points[2][0] = color_regions[i][0]; points[2][1] = color_regions[i][3]; points[2][2] = color_regions[i][4];
		points[3][0] = color_regions[i][0]; points[3][1] = color_regions[i][3]; points[3][2] = color_regions[i][5];
		points[4][0] = color_regions[i][1]; points[4][1] = color_regions[i][2]; points[4][2] = color_regions[i][4];
		points[5][0] = color_regions[i][1]; points[5][1] = color_regions[i][2]; points[5][2] = color_regions[i][5];
		points[6][0] = color_regions[i][1]; points[6][1] = color_regions[i][3]; points[6][2] = color_regions[i][4];
		points[7][0] = color_regions[i][1]; points[7][1] = color_regions[i][3]; points[7][2] = color_regions[i][5];

		max_dist = 0;
		max_color = 0;
		for (j = 0; j < 8; j++) {
			float dist = distance (L, a, b, points[j][0], points[j][1], points[j][2]);
			if (dist > max_dist) {
				max_dist = dist;
				max_color = j;
			}
		}

		/*
		 * If the luminosity distance is really short, extend the
		 * vector further out.  This may push it outside the bounds of
		 * the region that a color is specified in, but it keeps things
		 * readable when the background and foreground are really close.
		 */
		ld = L_distance (L, points[max_color][0]);
		cd = ab_distance (a, b, points[max_color][1], points[max_color][2]);
		if ((ld < 10.0f) && (cd < 60.0f)) {
			float dL, da, db;
			dL = points[max_color][0] - L;
			da = points[max_color][1] - a;
			db = points[max_color][2] - b;

			points[max_color][0] = L + (dL * 4.0f);
			points[max_color][1] = a + (da * 1.5f);
			points[max_color][2] = b + (db * 1.5f);
		}

		color.pixel = 0;
		lab_to_srgb(points[max_color][0],
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
