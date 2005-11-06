/*
 * notify-osd.c - xchat plugin which provides "OSD" like functionality through
 *                libnotify
 *
 * Copyright (C) 2005 David Trowbridge
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

#include "xchat-plugin.h"
#include "xg-plugin.h"

#define NOTIFY_OSD_NAME        "OSD"
#define NOTIFY_OSD_VERSION     "0.1"
#define NOTIFY_OSD_DESCRIPTION "Pops up notification of important messages when xchat doesn't have the focus"

static xchat_plugin       *ph;
static xchat_gnome_plugin *xgph;
static GtkWidget          *main_window;
static gboolean            focused = TRUE;

static gboolean
got_focus_cb (GtkWidget *wigdet, GdkEventFocus *event, gpointer data)
{
	focused = TRUE;
	return FALSE;
}

static gboolean
lost_focus_cb (GtkWidget *wigdet, GdkEventFocus *event, gpointer data)
{
	focused = FALSE;
	return FALSE;
}

static int
new_msg_cb (char *word[], gpointer data)
{
	g_print ("new msg cb\n");
	return XCHAT_EAT_NONE;
}

static int
new_action_cb (char *word[], gpointer data)
{
	g_print ("new action cb\n");
	return XCHAT_EAT_NONE;
}

static int
privmsg_cb (char *word[], char *word_eol[], gpointer data)
{
	if (strcmp (word[4], ":+\001ACTION") == 0) {
		char *to = word[3];
	} else {
		char *to = word[3];
		char *message = (word_eol[4][1] == '+') ? word_eol[4] + 2 : word_eol[4] + 1;
	}
	return XCHAT_EAT_NONE;
}

int
xchat_gnome_plugin_init (xchat_gnome_plugin *xg_plugin)
{
	xgph = xg_plugin;

	/* Hook up focus callbacks */
	main_window = xg_get_main_window ();
	g_signal_connect (main_window, "focus-in-event",  G_CALLBACK (got_focus_cb),  NULL);
        g_signal_connect (main_window, "focus-out-event", G_CALLBACK (lost_focus_cb), NULL);

	return TRUE;
}

void
xchat_plugin_get_info (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved)
{
	*plugin_name    = NOTIFY_OSD_NAME;
	*plugin_desc    = NOTIFY_OSD_DESCRIPTION;
	*plugin_version = NOTIFY_OSD_VERSION;
}

int
xchat_plugin_init (xchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg)
{
	xchat_plugin_get_info (plugin_name, plugin_desc, plugin_version, NULL);

	ph = plugin_handle;

	if (notify_init ("Xchat OSD")) {
		xchat_hook_server (ph, "PRIVMSG", XCHAT_PRI_NORM, privmsg_cb, NULL);

		xchat_hook_print (ph, "Channel Msg Hilight",    XCHAT_PRI_NORM, new_msg_cb,    NULL);
		xchat_hook_print (ph, "Channel Action Hilight", XCHAT_PRI_NORM, new_action_cb, NULL);

		xchat_print (ph, "OSD loaded\n");

		return TRUE;
	}
	return FALSE;
}
