/* autoaway.c
 *
 * An auto away plugin for xchat-gnome.
 *
 * Copyright (C) 2005 Isak Savo <isak.savo@gmail.com>
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
#include <glib.h>
#include <dlfcn.h>
#include <string.h>
#include <xchat-plugin.h>

#include "autoaway.h"
#include "gscreensaver.h"
#include "xscreensaver.h"

void xchat_plugin_get_info (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved);
int  xchat_plugin_init     (xchat_plugin * plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg);
int  xchat_plugin_deinit   (void);

static xchat_plugin *ph;		/* Plugin handle. */
static xchat_hook *timeout_hook = NULL; /* Timeout handle */
static GList *networks = NULL;		/* List of connected networks */

typedef enum {
	STATE_ACTIVE,
	STATE_AWAY
} aa_state_t;

typedef enum {
	SS_NONE,	/* This plugin is useless if this is set		*/
	SS_GNOME,	/* Use DBus to talk to gnome-screensaver		*/
	SS_X11		/* We're talking to xscreensaver			*/
} screensaver_t;

static screensaver_t screensaver_type = SS_NONE;
static aa_state_t state = STATE_ACTIVE;

static screensaver_t
get_screensaver_type (void)
{
	if (get_gs_has_ipc ())
		return SS_GNOME;

	if (get_xss_has_ipc ())
		return SS_X11;

	return SS_NONE;
}

static gboolean
is_away (void)
{
	if (xchat_get_context (ph))
		return (xchat_get_info (ph, "away") != NULL);

	return FALSE;
}

/* Use whatever available resource to find if the screensaver has kicked in or not */
static gboolean
get_screensaver_active (void)
{
	switch (screensaver_type) {
		case SS_X11:
			return get_xss_screensaver_active ();

		case SS_GNOME:
			return get_gs_screensaver_active();

		case SS_NONE:
			return FALSE;
	}

	return FALSE;
}


/* Go away on all networks and do a nickname change if desired */
static void
set_away (void)
{
	GList *node = g_list_first (networks);
	xchat_context *cur_context = NULL;

	while (node)
	{
		const gchar *nw = (const gchar*) node->data;
		cur_context = xchat_find_context (ph, nw, NULL);
		if (!cur_context) {
			node = node->next;
			continue;
		}
		xchat_set_context (ph, cur_context);
		if (is_away()) {
			node = g_list_next (node);
			continue;
		}

		xchat_command (ph, "AWAY");

		node = g_list_next (node);
	}
}

/* Come back from away and restore nickname if needed */
static void
come_back (void)
{
	GList *node = g_list_first (networks);
	xchat_context *cur_context = NULL;

	/* Iterate over all networks */
	while (node) {
		const char *nw = (const gchar*) node->data;
		cur_context = xchat_find_context (ph, nw, NULL);
		if (!cur_context) {
			node = g_list_next (node);
			continue;
		}
		xchat_set_context (ph, cur_context);

		if (!is_away ()) {
			node = g_list_next (node);
			continue;
		}
		xchat_command (ph, "AWAY");

		node = g_list_next (node);
	}
}

/*** Callbacks ***/

static int
timeout_cb (gpointer user_data)
{
	xchat_set_context (ph, xchat_find_context (ph, NULL, NULL));

	if (!networks) {
		trace ("Not connected");
		return 1;
	}
	switch (state) {
		case STATE_ACTIVE:
			if (get_screensaver_active ()) {
				set_away ();
				state = STATE_AWAY;
			}
			break;

		case STATE_AWAY:
			if (get_screensaver_active () == FALSE) {
				come_back ();
				state = STATE_ACTIVE;
			}
			break;

		default:
			break;
	}


	return 1;
}

/* New or lost connection - update network list */
static int
network_change_cb (gchar *word[], gpointer user_data)
{
	gboolean new_conn = GPOINTER_TO_INT (user_data);
	const gchar *servername;
	GList *node = NULL;

	servername = xchat_get_info (ph, "network");

	if (servername == NULL)
		return XCHAT_EAT_NONE;

	if (new_conn) {
		trace ("New connection on nw %s", servername);
		networks = g_list_append (networks, g_strdup (servername));
	} else {
		trace ("Connection closed on nw %s", servername);
		node = g_list_find_custom (networks, servername, (GCompareFunc) g_utf8_collate);
		if (node) {
			g_free (node->data);
			networks = g_list_delete_link (networks, node);
		}
	}

	return XCHAT_EAT_NONE;
}

/*** xchat-gnome plugin functions ***/
void
xchat_plugin_get_info (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved)
{
	*plugin_name = _("Auto Away");
	*plugin_desc = _("Automatically go away / come back");
	*plugin_version = AUTOAWAY_VERSION;

	if (reserved)
		*reserved = NULL;
}

/*** xchat plugin functions ***/
int
xchat_plugin_init (xchat_plugin * plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg)
{
	ph = plugin_handle;
	init_gs_connection ();

	/* Set the plugin info. */
	xchat_plugin_get_info (plugin_name, plugin_desc, plugin_version, NULL);

	/* Hook up our callbacks. */
	xchat_hook_print (ph, "Disconnected", XCHAT_PRI_NORM, network_change_cb, GINT_TO_POINTER (FALSE));
	xchat_hook_print (ph, "Connected",    XCHAT_PRI_NORM, network_change_cb, GINT_TO_POINTER (TRUE));

	timeout_hook = xchat_hook_timer (ph, 5000, timeout_cb, NULL);

	xchat_set_context (ph, xchat_find_context (ph, NULL, NULL));

	screensaver_type = get_screensaver_type ();

	/* All done */

	/* FIXME: Perhaps return FALSE if we failed to find a running screensaver? */
	return TRUE;
}

int
xchat_plugin_deinit (void)
{
	xchat_unhook (ph, timeout_hook);

	if (networks) {
		g_list_foreach (networks, (GFunc) g_free, NULL);
		g_list_free (networks);
	}

	close_gs_connection ();

	xchat_set_context (ph, xchat_find_context (ph, NULL, NULL));
	return TRUE;
}
