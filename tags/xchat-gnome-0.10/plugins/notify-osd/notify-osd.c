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

#define NOTIFY_OSD_NAME        _("On-screen display")
#define NOTIFY_OSD_VERSION     "0.1"
#define NOTIFY_OSD_DESCRIPTION _("Pops up notification of important messages when XChat-GNOME doesn't have the focus")

int  xchat_gnome_plugin_init (xchat_gnome_plugin *xg_plugin);
void xchat_plugin_get_info   (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved);
int  xchat_plugin_init       (xchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg);
int  xchat_plugin_deinit     (void);

enum
{
	STRIP_COLORS = 1 << 0,
	STRIP_ATTRS  = 1 << 1,
};

static xchat_plugin       *ph;
static xchat_gnome_plugin *xgph;
static GtkWidget          *main_window;
static gboolean            focused = TRUE;
static GdkPixbuf          *notify_icon;
static GSList             *notifications = NULL;

static void
hide_notifications (NotifyNotification *notify)
{
	GError *error = NULL;
	if (!notify_notification_close (notify, &error)) {
		g_warning (_("Error closing notification: %s\n"), error->message);
		g_error_free (error);
	}
}

static void
add_notify (char *summary, char *message)
{
	NotifyNotification *notify = NULL;
	GError *error = NULL;

	notify = notify_notification_new (summary, message, NULL, NULL);
	notify_notification_set_urgency (notify, NOTIFY_URGENCY_NORMAL);
	notify_notification_set_icon_from_pixbuf (notify, notify_icon);
	if (!notify_notification_show (notify, &error)) {
		g_warning (_("Failed to send notification: %s\n"), error->message);
		g_error_free (error);
		return;
	}

	notifications = g_slist_prepend (notifications, notify);
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
	gchar *stripped, *escaped, *message, *summary;

	if (focused)
		return XCHAT_EAT_NONE;

	channel = xchat_get_info (ph, "channel");
	stripped = xchat_strip (ph, word[2], -1, STRIP_COLORS | STRIP_ATTRS);

	message = g_strdup_printf ("<%s> %s", word[1], stripped);
	escaped = g_markup_escape_text(message, strlen(message));

	if (channel[0] == '#')
		summary = g_strdup_printf (_("Message in %s"), channel);
	else
		summary = g_strdup_printf (_("Message from %s"), channel);

	add_notify (summary, escaped);

	xchat_free (ph, stripped);
	g_free (message);
	g_free (summary);
	g_free (escaped);
	return XCHAT_EAT_NONE;
}

static int
new_action_cb (char *word[], gpointer data)
{
	const char *channel;
	gchar *stripped, *message, *summary;

	if (focused)
		return XCHAT_EAT_NONE;

	channel = xchat_get_info (ph, "channel");
	stripped = xchat_strip (ph, word[2], -1, STRIP_COLORS | STRIP_ATTRS);

	message = g_strdup_printf ("* %s %s", word[1], stripped);
	if (channel[0] == '#')
		summary = g_strdup_printf (_("Message in %s"), channel);
	else
		summary = g_strdup_printf (_("Message from %s"), channel);

	add_notify (summary, message);

	xchat_free (ph, stripped);
	g_free (message);
	g_free (summary);
	return XCHAT_EAT_NONE;
}

static int
private_msg_cb (char *word[], gpointer data)
{
	gchar *message, *summary;

	if (focused)
		return XCHAT_EAT_NONE;

	message = xchat_strip (ph, word[2], -1, STRIP_COLORS | STRIP_ATTRS);
	summary = g_strdup_printf (_("Private Message from %s"), word[1]);

	add_notify (summary, message);

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
		if (g_file_test ("../../data/xchat-gnome.png", G_FILE_TEST_EXISTS))
			notify_icon = gdk_pixbuf_new_from_file ("../../data/xchat-gnome.png", 0);
		else
			notify_icon = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/xchat-gnome.png", 0);

		xchat_hook_print (ph, "Channel Msg Hilight",       XCHAT_PRI_NORM, new_msg_cb,     NULL);
		xchat_hook_print (ph, "Channel Action Hilight",    XCHAT_PRI_NORM, new_action_cb,  NULL);
		xchat_hook_print (ph, "Private Message",           XCHAT_PRI_NORM, private_msg_cb, NULL);
		xchat_hook_print (ph, "Private Message to Dialog", XCHAT_PRI_NORM, private_msg_cb, NULL);

		xchat_print (ph, _("OSD loaded\n"));

		return TRUE;
	} else {
		xchat_print (ph, _("OSD initialization failed\n"));
	}
	return FALSE;
}

int
xchat_plugin_deinit (void)
{
	notify_uninit ();
	return TRUE;
}
