/*
 * preferences.h - interface to storing preferences
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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

#include <config.h>
#include <glib/gi18n.h>
#include <string.h>
#include <gconf/gconf-client.h>
#include "preferences.h"
#include "palette.h"
#include "../common/xchat.h"
#include "../common/xchatc.h"

gboolean
preferences_exist (void)
{
	GConfClient *client;
	char *text;

	client = gconf_client_get_default ();

	text = gconf_client_get_string (client, "/apps/xchat/version", NULL);
	if (text == NULL)
		return FALSE;
	else
		g_free (text);

	g_object_unref (client);

	/* probably eventually do some checking on the version to migrate from
	   previous versions when new features are added */
	return TRUE;
}

void
load_preferences (void)
{
	GConfClient *client;
	int color_scheme, palette_scheme;
	gboolean showcolors;
	char *text;

	client = gconf_client_get_default ();

	text = gconf_client_get_string (client, "/apps/xchat/irc/nickname", NULL);
	if (text != NULL)
		strcpy (prefs.nick1, text);
	g_free (text);

	text = gconf_client_get_string (client, "/apps/xchat/irc/realname", NULL);
	if (text != NULL)
		strcpy (prefs.realname, text);
	g_free (text);

	text = gconf_client_get_string (client, "/apps/xchat/irc/awaymsg", NULL);
	if (text != NULL)
		strcpy (prefs.awayreason, text);
	g_free (text);

	text = gconf_client_get_string (client, "/apps/xchat/irc/quitmsg", NULL);
	if (text != NULL)
		strcpy (prefs.quitreason, text);
	g_free (text);

	text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
	if (text != NULL)
		strcpy (prefs.partreason, text);
	g_free (text);

	showcolors = gconf_client_get_bool (client, "/apps/xchat/irc/showcolors", NULL);
	prefs.stripcolor = (unsigned int) (!showcolors);

	color_scheme = gconf_client_get_int (client, "/apps/xchat/irc/color_scheme", NULL);
	load_colors (color_scheme);
	palette_scheme = gconf_client_get_int (client, "/apps/xchat/irc/palette_scheme", NULL);
	load_palette (palette_scheme);

	g_object_unref (client);
}

void set_version (void)
{
	GConfClient *client;

	client = gconf_client_get_default ();

	gconf_client_set_string (client, "/apps/xchat/version", VERSION, NULL);
	g_object_unref (client);
}
