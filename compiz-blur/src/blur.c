/*
 * blur.c - A compiz plugin which blurs window contents behind other
 *          transparent windows.
 *
 * Copyright (C) 2006 David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
#include <compiz/compiz.h>
#include <glib.h>

#define GET_BLUR_DISPLAY(display) \
	((BlurDisplay *) (display)->privates[displayPrivateIndex].ptr)

#define GET_BLUR_SCREEN_PRIV(screen, blurDisplay) \
	((BlurScreen *) (screen)->privates[(blurDisplay)->screenPrivateIndex].ptr)

#define GET_BLUR_SCREEN(screen) \
	GET_BLUR_SCREEN_PRIV (screen, GET_BLUR_DISPLAY (screen->display))

#define GET_BLUR_WINDOW_PRIV(window, blurScreen) \
	((BlurWindow *) (window)->privates[(blurScreen)->windowPrivateIndex].ptr)

#define GET_BLUR_WINDOW(window)	\
	GET_BLUR_WINDOW_PRIV (window, GET_BLUR_SCREEN (window->screen))

static Bool blurInit        (CompPlugin *plugin);
static void blurFini        (CompPlugin *plugin);
static Bool blurInitDisplay (CompPlugin *plugin, CompDisplay *display);
static void blurFiniDisplay (CompPlugin *plugin, CompDisplay *display);
static Bool blurInitScreen  (CompPlugin *plugin, CompScreen  *screen);
static void blurFiniScreen  (CompPlugin *plugin, CompScreen  *screen);
static Bool blurInitWindow  (CompPlugin *plugin, CompWindow  *window);
static void blurFiniWindow  (CompPlugin *plugin, CompWindow  *window);

/*
 * This is an index into a global array of private data structures.  We store
 * a pointer to our BlurDisplay structure in this array.
 */
static int displayPrivateIndex;

/*
 * These structures contain private data and function overrides for various
 * pieces of the display model (display, screen, window).
 */
typedef struct _BlurDisplay {
	int screenPrivateIndex;
} BlurDisplay;

typedef struct _BlurScreen {
	int windowPrivateIndex;
} BlurScreen;

typedef struct _BlurWindow {
} BlurWindow;

/*
 * The list of dependencies.  I really have no idea how these dependencies are
 * decided on, so for now it's just arbitrarily chosen.
 */
CompPluginDep blurDeps[] = {
	{CompPluginRuleAfter, "decoration"}
};

/*
 * Virtual table for the plugin.  This contains all the info that compiz needs
 * to load and use this plugin.
 */
static CompPluginVTable blurVTable = {
	/* name */
	"blur",

	/* short description */
	"Blur window contents behind translucent windows",

	/* long description */
	"Blurs the contents of windows when they are behind translucent "
	"windows, allowing eye-candy to coexist with usability",

	blurInit,		/* plugin initialize */
	blurFini,		/* plugin finish */

	blurInitDisplay,	/* display initialize */
	blurFiniDisplay,	/* display finish */

	blurInitScreen,		/* screen initialize */
	blurFiniScreen,		/* screen finish */

	blurInitWindow,		/* window initialize */
	blurFiniWindow,		/* window finish */

	NULL,			/* get display options */
	NULL,			/* set display option */
	NULL,			/* get screen options */
	NULL,			/* set screen option */

	blurDeps,
	G_N_ELEMENTS (blurDeps),
};

CompPluginVTable *
getCompPluginInfo (void)
{
	return &blurVTable;
}

static Bool
blurInit (CompPlugin *plugin)
{
	displayPrivateIndex = allocateDisplayPrivateIndex ();
	if (displayPrivateIndex < 0)
		return FALSE;
	return TRUE;
}

static void
blurFini (CompPlugin *plugin)
{
	if (displayPrivateIndex >= 0)
		freeDisplayPrivateIndex (displayPrivateIndex);
}

static Bool
blurInitDisplay (CompPlugin *plugin, CompDisplay *display)
{
	BlurDisplay *bd;

	bd = g_new0 (BlurDisplay, 1);
	if (bd == NULL)
		return FALSE;
	bd->screenPrivateIndex = allocateScreenPrivateIndex (display);
	if (bd->screenPrivateIndex < 0) {
		g_free (bd);
		return FALSE;
	}

	display->privates[displayPrivateIndex].ptr = bd;

	return TRUE;
}

static void
blurFiniDisplay (CompPlugin *plugin, CompDisplay *display)
{
	BlurDisplay *bd = GET_BLUR_DISPLAY (display);

	if (bd->screenPrivateIndex >= 0)
		freeScreenPrivateIndex (display, bd->screenPrivateIndex);
	g_free (bd);
}

static Bool
blurInitScreen  (CompPlugin *plugin, CompScreen *screen)
{
	BlurScreen  *bs;
	BlurDisplay *bd;

	bd = GET_BLUR_DISPLAY (screen->display);

	bs = g_new0 (BlurScreen, 1);
	if (bs == NULL)
		return FALSE;
	bs->windowPrivateIndex = allocateWindowPrivateIndex (screen);
	if (bs->windowPrivateIndex < 0) {
		g_free (bs);
		return FALSE;
	}

	screen->privates[bd->screenPrivateIndex].ptr = bs;
	return TRUE;
}

static void
blurFiniScreen  (CompPlugin *plugin, CompScreen *screen)
{
	BlurScreen *bs = GET_BLUR_SCREEN (screen);

	freeWindowPrivateIndex (screen, bs->windowPrivateIndex);

	g_free (bs);
}

static Bool
blurInitWindow  (CompPlugin *plugin, CompWindow *window)
{
	BlurWindow *bw;
	BlurScreen *bs;

	bs = GET_BLUR_SCREEN (window->screen);

	bw = g_new0 (BlurWindow, 1);
	if (bw == NULL)
		return FALSE;
	window->privates[bs->windowPrivateIndex].ptr = bw;
	return TRUE;
}

static void
blurFiniWindow  (CompPlugin *plugin, CompWindow *window)
{
	BlurWindow *bw;

	bw = GET_BLUR_WINDOW (window);
	g_free (bw);
}
