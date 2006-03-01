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

CompPluginDep blurDeps[] = {
	{CompPluginRuleAfter, "decoration"}
};

static CompPluginVTable blurVTable = {
	/* name */
	"blur",

	/* short description */
	"Blur window contents behind translucent windows",

	/* long description */
	"Blurs the contents of windows when they are behind translucent "
	"windows, allowing eye-candy to coexist with usability",

	NULL,	/* plugin initialize */
	NULL,	/* plugin finish */

	NULL,	/* display initialize */
	NULL,	/* display finish */

	NULL,	/* window initialize */
	NULL,	/* window finish */

	NULL,	/* get display options */
	NULL,	/* set display option */
	NULL,	/* get screen options */
	NULL,	/* set screen option */

	blurDeps,
	G_N_ELEMENTS (blurDeps),
};

CompPluginVTable *
getCompPluginInfo (void)
{
	return &blurVTable;
}
