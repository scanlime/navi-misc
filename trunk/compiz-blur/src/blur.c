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
#include <GL/glew.h>
#include <compiz/compiz.h>
#include <glib.h>
#include <stdio.h>

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

#define CHECK_FRAMEBUFFER_STATUS()						\
	{									\
		GLenum status;							\
		status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);	\
		switch (status) {						\
			case GL_FRAMEBUFFER_COMPLETE_EXT:			\
				break;						\
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:			\
				/* choose different formats */			\
				break;						\
			default:						\
				assert (0);					\
		}								\
	}

#define WIN_W(w) ((w)->width + (w)->output.left + (w)->output.right)
#define WIN_H(w) ((w)->height + (w)->output.top + (w)->output.bottom)

static Bool blurInit               (CompPlugin  *plugin);
static void blurFini               (CompPlugin  *plugin);
static Bool blurInitDisplay        (CompPlugin  *plugin,
                                    CompDisplay *display);
static void blurFiniDisplay        (CompPlugin  *plugin,
                                    CompDisplay *display);
static Bool blurInitScreen         (CompPlugin  *plugin,
                                    CompScreen  *screen);
static void blurFiniScreen         (CompPlugin  *plugin,
                                    CompScreen  *screen);
static Bool blurInitWindow         (CompPlugin  *plugin,
                                    CompWindow  *window);
static void blurFiniWindow         (CompPlugin  *plugin,
                                    CompWindow  *window);

static Bool blurDamageWindowRect   (CompWindow  *window,
                                    Bool         initial,
                                    BoxPtr       rect);
static void blurDrawWindowGeometry (CompWindow  *window);
static void blurWindowResizeNotify (CompWindow  *window);

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

	DrawWindowGeometryProc drawWindowGeometry;
	DamageWindowRectProc   damageWindowRect;
	WindowResizeNotifyProc windowResizeNotify;
} BlurScreen;

typedef struct _BlurWindow {
	GLuint blur_texture;
	GLuint fbo;
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
	GLenum err;

	displayPrivateIndex = allocateDisplayPrivateIndex ();
	if (displayPrivateIndex < 0)
		return FALSE;

	err = glewInit ();
	if (err != GLEW_OK) {
		fprintf (stderr, "GLEW init failed: %s\n", glewGetErrorString (err));
		return FALSE;
	}

	if (!GLEW_EXT_framebuffer_object) {
		fprintf (stderr, "Blur init failed: EXT_framebuffer_object not found\n");
		return FALSE;
	}
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
blurInitScreen (CompPlugin *plugin, CompScreen *screen)
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

	WRAP (bs, screen, drawWindowGeometry, blurDrawWindowGeometry);
	WRAP (bs, screen, damageWindowRect,   blurDamageWindowRect);
	WRAP (bs, screen, windowResizeNotify, blurWindowResizeNotify);

	screen->privates[bd->screenPrivateIndex].ptr = bs;
	return TRUE;
}

static void
blurFiniScreen (CompPlugin *plugin, CompScreen *screen)
{
	BlurScreen *bs = GET_BLUR_SCREEN (screen);

	UNWRAP (bs, screen, drawWindowGeometry);
	UNWRAP (bs, screen, damageWindowRect);
	UNWRAP (bs, screen, windowResizeNotify);

	freeWindowPrivateIndex (screen, bs->windowPrivateIndex);

	g_free (bs);
}

static Bool
blurInitWindow (CompPlugin *plugin, CompWindow *window)
{
	BlurWindow *bw;
	BlurScreen *bs;

	bs = GET_BLUR_SCREEN (window->screen);

	bw = g_new0 (BlurWindow, 1);
	if (bw == NULL)
		return FALSE;
	window->privates[bs->windowPrivateIndex].ptr = bw;

	glGenTextures (1, &bw->blur_texture);
	glBindTexture (GL_TEXTURE_2D, bw->blur_texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffersEXT (1, &bw->fbo);

	return TRUE;
}

static void
blurFiniWindow (CompPlugin *plugin, CompWindow *window)
{
	BlurWindow *bw;

	bw = GET_BLUR_WINDOW (window);

	glDeleteTextures (1, &bw->blur_texture);
	glDeleteRenderbuffersEXT (1, &bw->fbo);

	g_free (bw);
}

static Bool
blurDamageWindowRect (CompWindow *window, Bool initial, BoxPtr rect)
{
	BlurScreen *bs;
	BlurWindow *bw;
	Bool        result;

	bw = GET_BLUR_WINDOW (window);
	bs = GET_BLUR_SCREEN (window->screen);

	UNWRAP (bs, window->screen, damageWindowRect);
	result = (*window->screen->damageWindowRect) (window, initial, rect);
	WRAP (bs, window->screen, damageWindowRect, blurDamageWindowRect);

	return result;
}

static void
blurDrawWindowGeometry (CompWindow *window)
{
	BlurWindow *bw;
	BlurScreen *bs;
	int         texUnit = window->texUnits;
	int         currentTexUnit = 0;
	int         stride = (1 + texUnit) * 2;
	GLfloat    *vertices = window->vertices + (stride - 2);

	stride *= sizeof (GLfloat);

	glVertexPointer (2, GL_FLOAT, stride, vertices);

	bw = GET_BLUR_WINDOW (window);
	bs = GET_BLUR_SCREEN (window->screen);

	/*
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, bw->fbo);
	glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, bw->blur_texture, 0);

	CHECK_FRAMEBUFFER_STATUS ();
	*/

	while (texUnit--) {
		if (texUnit != currentTexUnit) {
			window->screen->clientActiveTexture (GL_TEXTURE0_ARB + texUnit);
			currentTexUnit = texUnit;
		}
		vertices -= 2;
		glTexCoordPointer (2, GL_FLOAT, stride, vertices);
	}

	glDrawArrays (GL_QUADS, 0, window->vCount);

	/*
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, bw->blur_texture);

	UNWRAP (bs, window->screen, drawWindowGeometry);
	(*window->screen->drawWindowGeometry) (window);
	WRAP (bs, window->screen, drawWindowGeometry, blurDrawWindowGeometry);
	*/
}

static void
blurWindowResizeNotify (CompWindow  *window)
{
	BlurWindow *bw;
	BlurScreen *bs;

	bw = GET_BLUR_WINDOW (window);
	bs = GET_BLUR_SCREEN (window->screen);

	glBindTexture (GL_TEXTURE_2D, bw->blur_texture);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, WIN_W (window), WIN_H (window), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	UNWRAP (bs, window->screen, windowResizeNotify);
	(*window->screen->windowResizeNotify) (window);
	WRAP (bs, window->screen, windowResizeNotify, blurWindowResizeNotify);
}
