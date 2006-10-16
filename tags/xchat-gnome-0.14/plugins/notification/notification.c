/* notification.c
 *
 * A notification area plugin for xchat-gnome.
 *
 * Copyright (C) 2005 W. Evan Sheehan
 * Copyright (C) 2006 Christian Persch
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
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gconf/gconf-client.h>
#include <libgnome/gnome-program.h>
#include <dlfcn.h>

#include "navigation-tree.h"
#include "plugins.h"
#include "xchat-plugin.h"
#include "xg-plugin.h"

#define NOTIFICATION_VERSION "0.1"

void xchat_plugin_get_info   (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved);
int  xchat_plugin_init       (xchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg);
int  xchat_plugin_deinit     (void);
int  xchat_gnome_plugin_init (xchat_gnome_plugin *xg_plugin);

/* Enumerated type of different status levels. */
typedef enum
{
	NOTIF_NONE = 0,
	NOTIF_DATA,
	NOTIF_MSG,
	NOTIF_NICK,
	N_NOTIF
} NotifStatus;

static GtkStatusIcon*      status_icon;          /* Notification area icon. */
static gboolean            focused = TRUE;       /* GTK_WIDGET_HAS_FOCUS doesn't seem to be working... */
static gint		   level = 0;		 /* Level of messages that we want to notify */
static gboolean            hidden = FALSE;       /* True when the main window is hidden. */
static GdkPixbuf*          pixbufs[N_NOTIF];     /* Pixbufs */
static GtkWidget*          main_window;          /* xchat-gnome's main window. */
static NotifStatus         status = NOTIF_NONE;  /* Current status level. */
static xchat_gnome_plugin* xgph;                 /* xchat gnome plugin handle. */
static xchat_plugin*       ph;                   /* Plugin handle. */
static guint		   gconf_notify_id;

static GdkPixbuf *
load_pixbuf (const char *name, gint size)
{
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	char *uninstalled_path, *path;

	uninstalled_path = g_strdup_printf ("../../data/%s", name);
	if (g_file_test (uninstalled_path, G_FILE_TEST_EXISTS)) {
		path = uninstalled_path;
	} else {
		g_free (uninstalled_path);
		path = gnome_program_locate_file (gnome_program_get (), GNOME_FILE_DOMAIN_APP_DATADIR, name, FALSE, NULL);
	}

	pixbuf = gdk_pixbuf_new_from_file_at_scale (path, size, size, TRUE, &error);
	if (error != NULL) {
		g_warning ("Failed to load pixbuf %s at size %d: %s\n", path, size, error->message);
		g_error_free (error);
	}

	g_free (path);

	return pixbuf;
}

static void
load_tray_icons (gint size)
{
	static const char *icons[N_NOTIF] = {
		"xchat-gnome.png",
		"newdata.png",
		"global-message.png",
		"nicksaid.png"
	};
	static int old_size = -1;
	guint i;

	if (size == old_size) return;
	old_size = size;

	for (i = 0; i < N_NOTIF; ++i) {
		if (pixbufs[i] != NULL) {
			g_object_unref (pixbufs[i]);
		}
		pixbufs[i] = load_pixbuf (icons[i], size);
	}
}

static void
update_tray (void)
{
	gtk_status_icon_set_from_pixbuf (status_icon, pixbufs[status]);
	gtk_status_icon_set_visible (status_icon, status >= level);
}

/*** Callbacks ***/
static gboolean
got_focus_cb (GtkWidget * widget, GdkEventFocus * event, gpointer data)
{
	focused = TRUE;

	/* Reset the status. */
	status = NOTIF_NONE;

	update_tray ();

	return FALSE;
}

static gboolean
lost_focus_cb (GtkWidget * widget, GdkEventFocus * event, gpointer data)
{
	focused = FALSE;
	return FALSE;
}

static int
new_msg_cb (char *word[], void *statusptr)
{
	NotifStatus new_status = (NotifStatus) GPOINTER_TO_UINT (statusptr);

	if (status <= new_status &&
	    new_status < N_NOTIF &&
	    !focused && 
	    new_status >= level) {
		status = new_status;
		update_tray ();
	}

	return 0;
}

static gboolean
status_icon_size_changed_cb (GtkStatusIcon *icon, gint size, gpointer user_data)
{
	load_tray_icons (size);

	update_tray ();

	return TRUE;
}

static void
status_icon_activate_cb (GtkStatusIcon *icon, gpointer data)
{
	if (level == 0) {
		if (hidden) {
			xchat_command (ph, "GUI SHOW");
		} else {
			xchat_command (ph, "GUI HIDE");
		}

		hidden = !hidden;
	} else {
		gtk_window_present (GTK_WINDOW (main_window));
	}
}

/*** xchat-gnome plugin functions ***/
void
xchat_plugin_get_info (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved)
{
	*plugin_name = _("Notification");
	*plugin_desc = _("A notification area plugin.");
	*plugin_version = NOTIFICATION_VERSION;

	if (reserved)
		*reserved = NULL;
}

int
xchat_gnome_plugin_init (xchat_gnome_plugin * xg_plugin)
{
	xgph = xg_plugin;

	/* Hook up callbacks for changing focus on the main window. */
	main_window = xg_get_main_window ();
	g_signal_connect (main_window, "focus-in-event", G_CALLBACK (got_focus_cb), NULL);
	g_signal_connect (main_window, "focus-out-event", G_CALLBACK (lost_focus_cb), NULL);
	/* FIXMEchpe: where are those handlers disconnected again?? */

	/* Create the menu. */

	return 1;
}

static void
level_changed (GConfClient *client, guint cnxn_id,  GConfEntry *entry, gpointer data)
{
	level = gconf_value_get_int (gconf_entry_get_value (entry));
	update_tray ();
}

/*** xchat plugin functions ***/
int
xchat_plugin_init (xchat_plugin * plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg)
{
	GConfClient *client = gconf_client_get_default ();
	guint i;

	ph = plugin_handle;

	/* Set the plugin info. */
	xchat_plugin_get_info (plugin_name, plugin_desc, plugin_version, NULL);

	/* Get our preferences from gconf. */
	level = gconf_client_get_int (client, "/apps/xchat/plugins/notification/level", NULL);

	focused = TRUE;
	hidden = FALSE;
	status = NOTIF_NONE;

	/* Create the notification icon. */
	for (i = 0; i < N_NOTIF; ++i) {
		pixbufs[i] = 0;
	}

	status_icon = gtk_status_icon_new ();
	g_signal_connect (status_icon, "activate", G_CALLBACK (status_icon_activate_cb), NULL);
	g_signal_connect (status_icon, "size-changed", G_CALLBACK (status_icon_size_changed_cb), NULL);
	update_tray ();

	/* Hook up our callbacks. */
	xchat_hook_print (ph, "Channel Notice",			XCHAT_PRI_NORM, new_msg_cb, GUINT_TO_POINTER (NOTIF_DATA));
	xchat_hook_print (ph, "Channel Message",		XCHAT_PRI_NORM, new_msg_cb, GUINT_TO_POINTER (NOTIF_MSG));
	xchat_hook_print (ph, "Channel Action",			XCHAT_PRI_NORM, new_msg_cb, GUINT_TO_POINTER (NOTIF_MSG));
	xchat_hook_print (ph, "Channel Msg Hilight",		XCHAT_PRI_NORM, new_msg_cb, GUINT_TO_POINTER (NOTIF_NICK));
	xchat_hook_print (ph, "Channel Action Hilight",		XCHAT_PRI_NORM, new_msg_cb, GUINT_TO_POINTER (NOTIF_NICK));
	xchat_hook_print (ph, "Private Message to Dialog",	XCHAT_PRI_NORM, new_msg_cb, GUINT_TO_POINTER (NOTIF_NICK));

	gconf_notify_id = gconf_client_notify_add (client, "/apps/xchat/plugins/notification/level", (GConfClientNotifyFunc) level_changed, NULL, NULL, NULL);
	g_object_unref (client);

	xchat_print (ph, _("Notification plugin loaded.\n"));

	return TRUE;
}

int
xchat_plugin_deinit (void)
{
	GConfClient *client = gconf_client_get_default ();
	guint i;

	/* Disconnect the signal handlers. */
	g_signal_handlers_disconnect_by_func (main_window, G_CALLBACK (got_focus_cb), NULL);
	g_signal_handlers_disconnect_by_func (main_window, G_CALLBACK (lost_focus_cb), NULL);

	g_object_unref (status_icon);
	status_icon = NULL;

	gconf_client_notify_remove (client, gconf_notify_id);
	g_object_unref (client);
	gconf_notify_id = 0;

	for (i = 0; i < N_NOTIF; ++i) {
		if (pixbufs[i]) {
			g_object_unref (pixbufs[i]);
		}
		pixbufs[i] = NULL;
	}

	xchat_print (ph, _("Notification plugin unloaded.\n"));

	return 1;
}
