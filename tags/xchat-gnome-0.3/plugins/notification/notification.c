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
#include <gconf/gconf-client.h>
#include <dlfcn.h>

#include "xchat-plugin.h"
//#include "navigation_tree.h"
#include "eggtrayicon.h"

#define NOTIFICATION_VERSION "0.1"


typedef enum
{
	NOTIF_NONE,
	NOTIF_DATA,
	NOTIF_MSG,
	NOTIF_NICK
} NotifStatus;


static xchat_plugin *ph;				// Plugin handle.
static NotifStatus status = NOTIF_NONE;	// Current status level.
static gboolean window_visible = TRUE;	// Keep track of whether the window is visible.
//static NavTree *nav_tree;				// A reference to the navigation tree.
static EggTrayIcon *notification;		// Notification area icon.
//static GtkMenu *menu;					// The menu that pops up.
static GtkWidget *image;				// The image displayed by the icon.
static GdkPixbuf *logo,					// Pixbufs
				 *nick_said,
				 *msg_said,
				 *new_data;

static gboolean notification_clicked_cb (GtkWidget *widget, GdkEventButton *event, gpointer data);
static int new_text_cb (char **word, void *data);


void
xchat_plugin_get_info (char **plugin_name, char **plugin_desc,
		char **plugin_version, void **reserved)
{
	*plugin_name = "Notification";
	*plugin_desc = "A notification area plugin.";
	*plugin_version = NOTIFICATION_VERSION;

	if (reserved)
		*reserved = NULL;
}

#if 0
gboolean
notification_menu_add_channel (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	GtkWidget *item;
	GdkPixbuf *image = NULL;
	gchar *channel;

	/* Create a new menu item with a perdy picture. */
	gtk_tree_model_get (model, iter, 0, &image, 1, &channel, -1);
	item = gtk_image_menu_item_new_with_label (channel);

	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), gtk_image_new_from_pixbuf (image));

	/* Shove it in the menu. */
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
	gtk_widget_show (item);

	return FALSE;
}
#endif

int
xchat_plugin_init (xchat_plugin *plugin_handle, char **plugin_name,
		char **plugin_desc, char **plugin_version, char *arg)
{
	GtkWidget *box;
	GdkPixbuf *p;
	//void *mp;
	//NavTree *(*get_nt)(void);
	//char *error;

	/*mp = dlopen (NULL, RTLD_NOW);
	dlerror ();
	get_nt = dlsym (mp, "get_navigation_tree");
	error = dlerror ();
	if (error) {
		g_print ("%s\n\n", error);
		return FALSE;
	}
	*/
	ph = plugin_handle;

	/* Set the plugin info. */
	xchat_plugin_get_info (plugin_name, plugin_desc, plugin_version, NULL);

	/* Load the pixbufs. */
	p = gdk_pixbuf_new_from_file (XCHATSHAREDIR"/xchat-gnome-small.png", 0);
	logo = gdk_pixbuf_scale_simple (p, 16, 16, GDK_INTERP_BILINEAR);

	p = gdk_pixbuf_new_from_file (XCHATSHAREDIR"/nicksaid.png", 0);
	nick_said = gdk_pixbuf_scale_simple (p, 16, 16, GDK_INTERP_BILINEAR);

	p = gdk_pixbuf_new_from_file (XCHATSHAREDIR"/global-message.png", 0);
	msg_said = gdk_pixbuf_scale_simple (p, 16, 16, GDK_INTERP_BILINEAR);

	p = gdk_pixbuf_new_from_file (XCHATSHAREDIR"/newdata.png", 0);
	new_data = gdk_pixbuf_scale_simple (p, 16, 16, GDK_INTERP_BILINEAR);

	/* Create the notification icon. */
	notification = egg_tray_icon_new ("xchat-gnome");
	box = gtk_event_box_new ();
	image = gtk_image_new_from_pixbuf (logo);

	g_signal_connect (G_OBJECT (box), "button-press-event", G_CALLBACK (notification_clicked_cb), NULL);

	gtk_container_add (GTK_CONTAINER (box), image);
	gtk_container_add (GTK_CONTAINER (notification), box);

	gtk_widget_show_all (GTK_WIDGET (notification));

	/* FIXME: Saw this in Gaim's notification plugin. Not sure it's necessary,
	 * will require investigation at a later date.
	 */
	g_object_ref (G_OBJECT (notification));

	/* Create the menu. */
	/*
	menu = GTK_MENU (gtk_menu_new ());
	nav_tree = get_nt ();
	gtk_tree_model_foreach (nav_tree->model->sorted,
			(GtkTreeModelForeachFunc)notification_menu_add_channel, NULL);
	gtk_widget_show (GTK_WIDGET (menu));
	*/

	/* Hook up our callbacks. */
	xchat_hook_print (ph, "Channel Message", XCHAT_PRI_NORM, new_text_cb, 0);
	xchat_hook_print (ph, "Private Message to Dialog", XCHAT_PRI_NORM, new_text_cb, 0);

	xchat_print (ph, "Notification plugin loaded.\n");

	return TRUE;
}

int
xchat_plugin_deinit ()
{
	g_object_unref (G_OBJECT (notification));
	gtk_widget_destroy (GTK_WIDGET (notification));

	xchat_print (ph, "Notification plugin unloaded.\n");

	return 1;
}
/*
static void
notification_menu_show (GdkEventButton *event)
{
	gtk_menu_popup (menu, NULL, NULL, NULL, NULL, event->button, event->time);
}
*/
static gboolean
notification_clicked_cb (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	switch (event->button) {
		/* Left click. */
		case 1:
			if (window_visible) {
				window_visible = FALSE;
				xchat_command (ph, "GUI HIDE");
			}
			else {
				window_visible = TRUE;
				xchat_command (ph, "GUI SHOW");
			}
			break;

		/* Right click. */
		case 3:
			//notification_menu_show (event);
			break;

		default:
			break;
	}

	return TRUE;
}

static int
new_text_cb (char **word, void *data)
{
}
/*** The End ***/
