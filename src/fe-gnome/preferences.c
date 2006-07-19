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

static void set_nickname       (const gchar *text);
static void nickname_changed   (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data);
static void realname_changed   (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data);
static void awaymsg_changed    (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data);
static void quitmsg_changed    (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data);
static void partmsg_changed    (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data);
static void showcolors_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data);
static void colors_changed     (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data);

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
	int color_scheme;
	gboolean showcolors;
	gchar *text;

	client = gconf_client_get_default ();

	text = gconf_client_get_string (client, "/apps/xchat/irc/nickname", NULL);
	if (text) {
		set_nickname (text);
		g_free (text);
	}
	gconf_client_notify_add (client, "/apps/xchat/irc/nickname", nickname_changed, NULL, NULL, NULL);

	text = gconf_client_get_string (client, "/apps/xchat/irc/realname", NULL);
	if (text) {
		strcpy (prefs.realname, text);
		g_free (text);
	}
	gconf_client_notify_add (client, "/apps/xchat/irc/realname", realname_changed, NULL, NULL, NULL);

	text = gconf_client_get_string (client, "/apps/xchat/irc/awaymsg", NULL);
	if (text) {
		strcpy (prefs.awayreason, text);
		g_free (text);
	}
	gconf_client_notify_add (client, "/apps/xchat/irc/awaymsg", awaymsg_changed, NULL, NULL, NULL);

	text = gconf_client_get_string (client, "/apps/xchat/irc/quitmsg", NULL);
	if (text) {
		strcpy (prefs.quitreason, text);
		g_free (text);
	}
	gconf_client_notify_add (client, "/apps/xchat/irc/quitmsg", quitmsg_changed, NULL, NULL, NULL);

	text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
	if (text) {
		strcpy (prefs.partreason, text);
		g_free (text);
	}
	gconf_client_notify_add (client, "/apps/xchat/irc/partmsg", partmsg_changed, NULL, NULL, NULL);

	showcolors = gconf_client_get_bool (client, "/apps/xchat/irc/showcolors", NULL);
	prefs.stripcolor = (unsigned int) (!showcolors);
	gconf_client_notify_add (client, "/apps/xchat/irc/showcolors", showcolors_changed, NULL, NULL, NULL);

	color_scheme = gconf_client_get_int (client, "/apps/xchat/irc/color_scheme", NULL);
	load_colors (color_scheme);
	load_palette (color_scheme);
	gconf_client_notify_add (client, "/apps/xchat/irc/color_scheme", colors_changed, NULL, NULL, NULL);

	g_object_unref (client);
}

void set_version (void)
{
	GConfClient *client;

	client = gconf_client_get_default ();

	gconf_client_set_string (client, "/apps/xchat/version", PACKAGE_VERSION, NULL);
	g_object_unref (client);
}

static void
set_nickname (const gchar *text)
{
	gchar *text2;

	strcpy (prefs.nick1, text);

	text2 = g_strdup_printf ("%s_", text);
	strcpy (prefs.nick2, text2);
	g_free (text2);

	text2 = g_strdup_printf ("%s__", text);
	strcpy (prefs.nick3, text2);
	g_free (text2);
}

static void
nickname_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data)
{
	gchar *text = gconf_client_get_string (client, "/apps/xchat/irc/nickname", NULL);
	if (text) {
		set_nickname (text);
		g_free (text);
	}
}

static void
realname_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data)
{
	gchar *text = gconf_client_get_string (client, "/apps/xchat/irc/realname", NULL);
	if (text) {
		strcpy (prefs.realname, text);
		g_free (text);
	}
}

static void
awaymsg_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data)
{
	gchar *text = gconf_client_get_string (client, "/apps/xchat/irc/awaymsg", NULL);
	if (text) {
		strcpy (prefs.awayreason, text);
		g_free (text);
	}
}

static void
quitmsg_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data)
{
	gchar *text = gconf_client_get_string (client, "/apps/xchat/irc/quitmsg", NULL);
	if (text) {
		strcpy (prefs.quitreason, text);
		g_free (text);
	}
}

static void
partmsg_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data)
{
	gchar *text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
	if (text) {
		strcpy (prefs.partreason, text);
		g_free (text);
	}
}

static void
showcolors_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data)
{
	gboolean showcolors = gconf_client_get_bool (client, "/apps/xchat/irc/showcolors", NULL);
	prefs.stripcolor = (unsigned int) (!showcolors);
}

static void
colors_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data)
{
	gint color_scheme = gconf_client_get_int (client, "/apps/xchat/irc/color_scheme", NULL);
	load_colors (color_scheme);
	load_palette (color_scheme);
}
