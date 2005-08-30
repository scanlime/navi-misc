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

/* Enumerated type of different status levels. */
typedef enum
{
	NOTIF_NONE = 0,
	NOTIF_DATA,
	NOTIF_MSG,
	NOTIF_NICK
} NotifStatus;

/* Represents a channel entry in the pop-up menu. */
struct MenuChannel
{
	NotifStatus	status;
	GtkWidget*	menu_item;
};

static xchat_plugin*		ph;			/* Plugin handle. */
static xchat_gnome_plugin*	xgph;			/* xchat gnome plugin handle. */
static NotifStatus		status = NOTIF_NONE;	/* Current status level. */
static gboolean			focused = TRUE;		/* GTK_WIDGET_HAS_FOCUS doesn't seem to be working... */
static gboolean			persistant;		/* Keep the icon in the tray at all times? */
static GtkWidget*		main_window;		/* xchat-gnome's main window. */
static EggTrayIcon*		notification;		/* Notification area icon. */
static GtkWidget*		image;			/* The image displayed by the icon. */
static GdkPixbuf*		pixbufs[4];		/* Pixbufs */


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
		gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbufs[0]);

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
	if (status < (NotifStatus) msg_lvl && !focused) {
		status = (NotifStatus) msg_lvl;
		gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbufs[status]);
		gtk_widget_show_all (GTK_WIDGET (notification));
	}

	return 0;
}

static int
join_chan_cb (char **word, void *data)
{
	return 0;
}

static int
part_chan_cb (char **word, void *data)
{
	return 0;
}

static int
chan_changed_cb (char **word, void *data)
{
	return 0;
}

static gboolean
notification_clicked_cb (GtkWidget * widget, GdkEventButton * event, gpointer data)
{
	switch (event->button) {
		/* Left click. */
	case 1:
		if (gdk_window_get_state (main_window->window) & GDK_WINDOW_STATE_ICONIFIED) {
			gtk_window_present (GTK_WINDOW (main_window));
		} else if (persistant) {
			xchat_command (ph, "GUI HIDE");
		}
		break;

		/* Right click. */

	default:
		break;
	}

	return TRUE;
}


/*** Utility Functions ***/
gboolean
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
	*plugin_name = "Notification";
	*plugin_desc = "A notification area plugin.";
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
	GtkWidget*		box;
	GdkPixbuf*		p;
	GConfClient*	client = gconf_client_get_default ();

	ph = plugin_handle;

	/* Set the plugin info. */
	xchat_plugin_get_info (plugin_name, plugin_desc, plugin_version, NULL);

	/* Get our preferences from gconf. */
	persistant = gconf_client_get_bool (client, "/apps/xchat/plugins/notification/persistant", NULL);

	/* FIXME It would be nice to determine the size of the panel and load these
	 *       images at that size.
	 */

	/* Load the pixbufs. */
	/* xchat-gnome logo. */
	p = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/xchat-gnome-small.png", 0);
	pixbufs[0] = gdk_pixbuf_scale_simple (p, 16, 16, GDK_INTERP_BILINEAR);

	/* New data image. */
	p = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/newdata.png", 0);
	pixbufs[1] = gdk_pixbuf_scale_simple (p, 16, 16, GDK_INTERP_BILINEAR);

	/* New message image. */
	p = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/global-message.png", 0);
	pixbufs[2] = gdk_pixbuf_scale_simple (p, 16, 16, GDK_INTERP_BILINEAR);

	/* Nick said image. */
	p = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/nicksaid.png", 0);
	pixbufs[3] = gdk_pixbuf_scale_simple (p, 16, 16, GDK_INTERP_BILINEAR);

	/* Create the notification icon. */
	notification = egg_tray_icon_new ("xchat-gnome");
	box = gtk_event_box_new ();
	image = gtk_image_new_from_pixbuf (pixbufs[0]);

	g_signal_connect (G_OBJECT (box), "button-press-event", G_CALLBACK (notification_clicked_cb), NULL);

	gtk_container_add (GTK_CONTAINER (box), image);
	gtk_container_add (GTK_CONTAINER (notification), box);

	gtk_widget_show_all (GTK_WIDGET (notification));

	/* FIXME: Saw this in Gaim's notification plugin. Not sure it's necessary,
	 *        will require investigation at a later date.
	 */
	g_object_ref (G_OBJECT (notification));


	/* Hook up our callbacks. */
	xchat_hook_print (ph, "Channel Notice",			XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_DATA);
	xchat_hook_print (ph, "Channel Message",		XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_MSG);
	xchat_hook_print (ph, "Channel Action",			XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_MSG);
	xchat_hook_print (ph, "Channel Msg Hilight",		XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_NICK);
	xchat_hook_print (ph, "Channel Action Hilight",		XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_NICK);
	xchat_hook_print (ph, "Private Message to Dialog",	XCHAT_PRI_NORM, new_msg_cb, (gpointer) NOTIF_MSG);
	xchat_hook_print (ph, "You Join",			XCHAT_PRI_NORM, join_chan_cb, NULL);
	xchat_hook_print (ph, "You Part",			XCHAT_PRI_NORM, part_chan_cb, NULL);
	xchat_hook_print (ph, "Focus Tab",			XCHAT_PRI_NORM, chan_changed_cb, NULL);

	xchat_print (ph, "Notification plugin loaded.\n");

	return TRUE;
}

int
xchat_plugin_deinit ()
{
	/* Disconnect the signal handlers. */
	g_signal_handlers_disconnect_by_func (main_window, G_CALLBACK (got_focus_cb), NULL);
	g_signal_handlers_disconnect_by_func (main_window, G_CALLBACK (lost_focus_cb), NULL);

	g_object_unref (G_OBJECT (notification));
	gtk_widget_destroy (GTK_WIDGET (notification));

	xchat_print (ph, "Notification plugin unloaded.\n");

	return 1;
}
