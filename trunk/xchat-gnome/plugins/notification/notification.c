/* notification.c
 *
 * A notification area plugin for xchat-gnome.
 *
 * Copyright (C) 2005 W. Evan Sheehan
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
#include <dlfcn.h>

#include "navigation-tree.h"
#include "plugins.h"
#include "xchat-plugin.h"
#include "xg-plugin.h"
#include "eggtrayicon.h"

#define NOTIFICATION_VERSION "0.1"

void xchat_plugin_get_info   (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved);
int  xchat_plugin_init       (xchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg);
int  xchat_plugin_deinit     (void);
int  xchat_gnome_plugin_init (xchat_gnome_plugin *xg_plugin);
static gboolean add_channels_foreach_cb (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
static gboolean tray_destroy_cb (GtkObject *object, gpointer data);

/* Enumerated type of different status levels. */
typedef enum
{
	NOTIF_NONE = 0,
	NOTIF_DATA,
	NOTIF_MSG,
	NOTIF_NICK,
	N_NOTIF
} NotifStatus;

static EggTrayIcon*        notification;         /* Notification area icon. */
static gboolean            focused = TRUE;       /* GTK_WIDGET_HAS_FOCUS doesn't seem to be working... */
static gboolean            persistant;           /* Keep the icon in the tray at all times? */
static gboolean            hidden = FALSE;       /* True when the main window is hidden. */
static GdkPixbuf*          pixbufs[N_NOTIF];     /* Pixbufs */
static GtkWidget*          image;                /* The image displayed by the icon. */
static GtkWidget*          main_window;          /* xchat-gnome's main window. */
static NotifStatus         status = NOTIF_NONE;  /* Current status level. */
static xchat_gnome_plugin* xgph;                 /* xchat gnome plugin handle. */
static xchat_plugin*       ph;                   /* Plugin handle. */
static guint               tray_destroy_handler; /* Signal handler to recreate widget if the notification area disappears */

/*** Callbacks ***/
static gboolean
got_focus_cb (GtkWidget * widget, GdkEventFocus * event, gpointer data)
{
	focused = TRUE;

	/* Reset the status. */
	status = NOTIF_NONE;

	if (!persistant) {
		/* Hide the notification icon. */
		gtk_widget_hide_all (GTK_WIDGET (notification));
	} else {
		gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbufs[NOTIF_NONE]);

		/* Show the notification icon. */
		gtk_widget_show_all (GTK_WIDGET (notification));
	}

	return FALSE;
}

static gboolean
lost_focus_cb (GtkWidget * widget, GdkEventFocus * event, gpointer data)
{
	focused = FALSE;
	return FALSE;
}

static int
new_msg_cb (char **word, void *msg_lvl)
{
	if (status <= (NotifStatus) msg_lvl && (NotifStatus) msg_lvl < N_NOTIF && !focused) {
		status = (NotifStatus) msg_lvl;
		gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbufs[status]);
		gtk_widget_show_all (GTK_WIDGET (notification));
	}

	return 0;
}

static gboolean
notification_clicked_cb (GtkWidget * widget, GdkEventButton * event, gpointer data)
{
	switch (event->button) {
		/* Left click. */
		case 1:
			if (persistant) {
				if (hidden) {
					xchat_command (ph, "GUI SHOW");
					hidden = FALSE;
				} else {
					xchat_command (ph, "GUI HIDE");
					hidden = TRUE;
				}

				hidden = !hidden;
			} else {
				gtk_window_present (GTK_WINDOW (main_window));
			}

			break;

		default:
			break;
	}

	return TRUE;
}


/*** Utility Functions ***/
static gboolean
add_channels_foreach_cb (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
	gchar* channel;

	gtk_tree_model_get (model, iter, 1, &channel, -1);

	return FALSE;
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

	/* Create the menu. */

	return 1;
}


/*** xchat plugin functions ***/
int
xchat_plugin_init (xchat_plugin * plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg)
{
	GdkPixbuf   *icon, *newdata, *global, *nicksaid;
	GConfClient *client = gconf_client_get_default ();

	ph = plugin_handle;

	/* Set the plugin info. */
	xchat_plugin_get_info (plugin_name, plugin_desc, plugin_version, NULL);

	/* Get our preferences from gconf. */
	persistant = gconf_client_get_bool (client, "/apps/xchat/plugins/notification/persistant", NULL);

	/* FIXME It would be nice to determine the size of the panel and load these
	 *       images at that size.
	 */

	/* Load the pixbufs. */
	if (g_file_test ("../../data/xchat-gnome-small.png", G_FILE_TEST_EXISTS)) {
		icon     = gdk_pixbuf_new_from_file ("../../data/xchat-gnome.png", 0);
		newdata  = gdk_pixbuf_new_from_file ("../../data/newdata.png", 0);
		global   = gdk_pixbuf_new_from_file ("../../data/global-message.png", 0);
		nicksaid = gdk_pixbuf_new_from_file ("../../data/nicksaid.png", 0);
	} else {
		icon     = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/xchat-gnome.png", 0);
		newdata  = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/newdata.png", 0);
		global   = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/global-message.png", 0);
		nicksaid = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/nicksaid.png", 0);
	}

	pixbufs[NOTIF_NONE] = gdk_pixbuf_scale_simple (icon,     16, 16, GDK_INTERP_BILINEAR);
	pixbufs[NOTIF_DATA] = gdk_pixbuf_scale_simple (newdata,  16, 16, GDK_INTERP_BILINEAR);
	pixbufs[NOTIF_MSG]  = gdk_pixbuf_scale_simple (global,   16, 16, GDK_INTERP_BILINEAR);
	pixbufs[NOTIF_NICK] = gdk_pixbuf_scale_simple (nicksaid, 16, 16, GDK_INTERP_BILINEAR);

	/* Create the notification icon. */
	tray_destroy_cb (NULL, NULL);

	/* Hook up our callbacks. */
	xchat_hook_print (ph, "Channel Notice",			XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_DATA);
	xchat_hook_print (ph, "Channel Message",		XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_MSG);
	xchat_hook_print (ph, "Channel Action",			XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_MSG);
	xchat_hook_print (ph, "Channel Msg Hilight",		XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_NICK);
	xchat_hook_print (ph, "Channel Action Hilight",		XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_NICK);
	xchat_hook_print (ph, "Private Message to Dialog",	XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_NICK);

	xchat_print (ph, _("Notification plugin loaded.\n"));

	return TRUE;
}

static gboolean
tray_destroy_cb (GtkObject *object,
		 gpointer data)
{
	GtkWidget   *box;

	if (notification) {
		gtk_object_sink (notification);
		notification = NULL;
	}

	notification = egg_tray_icon_new ("xchat-gnome");
	box = gtk_event_box_new ();
	image = gtk_image_new_from_pixbuf (pixbufs[NOTIF_NONE]);

	g_signal_connect (G_OBJECT (box), "button-press-event", G_CALLBACK (notification_clicked_cb), NULL);

	gtk_container_add (GTK_CONTAINER (box), image);
	gtk_container_add (GTK_CONTAINER (notification), box);

	tray_destroy_handler = g_signal_connect_object (G_OBJECT (notification), "destroy", G_CALLBACK (tray_destroy_cb), data, 0);

 	gtk_widget_show_all (notification);
 	return TRUE;
}


int
xchat_plugin_deinit (void)
{
	/* Disconnect the signal handlers. */
	g_signal_handlers_disconnect_by_func (main_window, G_CALLBACK (got_focus_cb), NULL);
	g_signal_handlers_disconnect_by_func (main_window, G_CALLBACK (lost_focus_cb), NULL);

	if (notification) {
		g_signal_handler_disconnect (G_OBJECT(notification), tray_destroy_handler);
		gtk_widget_destroy (GTK_WIDGET (notification));
		notification = NULL;
	}

	xchat_print (ph, _("Notification plugin unloaded.\n"));

	return 1;
}
