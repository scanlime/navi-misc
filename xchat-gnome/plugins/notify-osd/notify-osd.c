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

#include <config.h>
#include <libnotify/notify.h>
#include <libgnome/gnome-i18n.h>
#include "xchat-plugin.h"
#include "xg-plugin.h"

#define NOTIFY_OSD_NAME        "OSD"
#define NOTIFY_OSD_VERSION     "0.1"
#define NOTIFY_OSD_DESCRIPTION "Pops up notification of important messages when xchat doesn't have the focus"

enum
{
	STRIP_COLORS = 1 << 0,
	STRIP_ATTRS  = 1 << 1,
};

static xchat_plugin       *ph;
static xchat_gnome_plugin *xgph;
static GtkWidget          *main_window;
static gboolean            focused = TRUE;
static NotifyIcon         *icon;
static GSList             *notifications = NULL;

static void
hide_notifications (NotifyHandle *handle)
{
	notify_close (handle);
}

static gboolean
got_focus_cb (GtkWidget *wigdet, GdkEventFocus *event, gpointer data)
{
	g_slist_foreach (notifications, (GFunc) hide_notifications, NULL);
	g_slist_free (notifications);
	notifications = NULL;
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
	const char *channel;
	gchar *stripped, *message, *summary;
	NotifyHandle *handle;

	if (focused)
		return XCHAT_EAT_NONE;

	channel = xchat_get_info (ph, "channel");
	stripped = xchat_strip (ph, word[2], -1, STRIP_COLORS | STRIP_ATTRS);

	message = g_strdup_printf ("&lt;%s&gt; %s", word[1], stripped);
	if (channel[0] == '#')
		summary = g_strdup_printf ("Message in %s", channel);
	else
		summary = g_strdup_printf ("Message from %s", channel);

	handle = notify_send_notification (NULL, NULL, NOTIFY_URGENCY_NORMAL, summary, message, icon, TRUE, 0, NULL, NULL, 0);
	notifications = g_slist_prepend (notifications, handle);

	xchat_free (ph, stripped);
	g_free (message);
	g_free (summary);
	return XCHAT_EAT_NONE;
}

static int
new_action_cb (char *word[], gpointer data)
{
	const char *channel;
	gchar *stripped, *message, *summary;
	NotifyHandle *handle;

	if (focused)
		return XCHAT_EAT_NONE;

	channel = xchat_get_info (ph, "channel");
	stripped = xchat_strip (ph, word[2], -1, STRIP_COLORS | STRIP_ATTRS);

	message = g_strdup_printf ("* %s %s", word[1], stripped);
	if (channel[0] == '#')
		summary = g_strdup_printf ("Message in %s", channel);
	else
		summary = g_strdup_printf ("Message from %s", channel);

	handle = notify_send_notification (NULL, NULL, NOTIFY_URGENCY_NORMAL, summary, message, icon, TRUE, 0, NULL, NULL, 0);
	notifications = g_slist_prepend (notifications, handle);

	xchat_free (ph, stripped);
	g_free (message);
	g_free (summary);
	return XCHAT_EAT_NONE;
}

static int
private_msg_cb (char *word[], gpointer data)
{
	gchar *message, *summary;
	NotifyHandle *handle;

	if (focused)
		return XCHAT_EAT_NONE;

	message = xchat_strip (ph, word[2], -1, STRIP_COLORS | STRIP_ATTRS);
	summary = g_strdup_printf ("Private Message from %s", word[1]);

	handle = notify_send_notification (NULL, NULL, NOTIFY_URGENCY_NORMAL, summary, message, icon, TRUE, 0, NULL, NULL, 0);
	notifications = g_slist_prepend (notifications, handle);

	xchat_free (ph, message);
	g_free (summary);
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
		icon = notify_icon_new_from_uri (XCHATSHAREDIR "/xchat-gnome.png");

		xchat_hook_print (ph, "Channel Msg Hilight",       XCHAT_PRI_NORM, new_msg_cb,     NULL);
		xchat_hook_print (ph, "Channel Action Hilight",    XCHAT_PRI_NORM, new_action_cb,  NULL);
		xchat_hook_print (ph, "Private Message",           XCHAT_PRI_NORM, private_msg_cb, NULL);
		xchat_hook_print (ph, "Private Message to Dialog", XCHAT_PRI_NORM, private_msg_cb, NULL);

		xchat_print (ph, "OSD loaded\n");

		return TRUE;
	} else {
		xchat_print (ph, "OSD initialization failed\n");
	}
	return FALSE;
}

int
xchat_plugin_deinit ()
{
	notify_icon_destroy (icon);
}
