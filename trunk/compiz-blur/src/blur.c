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

#define GET_BLUR_DISPLAY(d) ((BlurDisplay *) (d)->privates[displayPrivateIndex].ptr)

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

typedef struct _BlurDisplay {
	int screenPrivateIndex;
} BlurDisplay;

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

	/*
	 * Here's where we allocate our private display structure and set up
	 * overrides for various functions
	 */
	bd = g_new0 (BlurDisplay, 1);
	if (!bd)
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
blurInitScreen  (CompPlugin *plugin, CompScreen  *screen)
{
	return TRUE;
}

static void
blurFiniScreen  (CompPlugin *plugin, CompScreen  *screen)
{
}

static Bool
blurInitWindow  (CompPlugin *plugin, CompWindow  *window)
{
	return TRUE;
}

static void
blurFiniWindow  (CompPlugin *plugin, CompWindow  *window)
{
}
