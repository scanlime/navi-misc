/*
 * userlist-window.c - The userlist pop-up window
 *
 * Copyright (C) 2005 xchat-gnome team
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
#include <gtk/gtkframe.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtktreeview.h>
#include <gtk/gtkliststore.h>
#include "userlist-window.h"

static void userlist_window_class_init (UserlistWindowClass *klass);
static void userlist_window_init       (UserlistWindow      *window);
static void userlist_window_finalize   (GObject             *object);

static GtkWindowClass *parent_class;

G_DEFINE_TYPE (UserlistWindow, userlist_window, GTK_TYPE_WINDOW);

struct _UserlistWindowPriv
{
	GtkWidget      *treeview;

	GHashTable     *stores;
	struct session *current;
};

static void
userlist_window_class_init (UserlistWindowClass *klass)
{
	GObjectClass *gobject_class;

	parent_class = g_type_class_peek_parent (klass);

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = userlist_window_finalize;
}

static void
userlist_window_init (UserlistWindow *window)
{
	GtkWidget *frame;
	GtkWidget *scrolledwindow;

	window->priv = g_new0 (UserlistWindowPriv, 1);
	window->priv->treeview = gtk_tree_view_new ();

	frame          = gtk_frame_new (NULL);
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);

	window->priv->stores = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_object_unref);

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow),
	                                GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_container_add              (GTK_CONTAINER (scrolledwindow), window->priv->treeview);
	gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 3);
	gtk_frame_set_shadow_type      (GTK_FRAME (frame), GTK_SHADOW_OUT);
	gtk_container_add              (GTK_CONTAINER (frame), scrolledwindow);
	gtk_container_add              (GTK_CONTAINER (window), frame);

	g_object_set                   (G_OBJECT   (window), "type", GTK_WINDOW_POPUP);
	gtk_window_set_decorated       (GTK_WINDOW (window), FALSE);
	gtk_window_set_type_hint       (GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_MENU);

	gtk_widget_show (frame);
	gtk_widget_show (scrolledwindow);
	gtk_widget_show (window->priv->treeview);
}

static void
userlist_window_finalize (GObject *object)
{
	UserlistWindow *window;

	window = USERLIST_WINDOW (object);

	g_hash_table_destroy (window->priv->stores);
	g_free (window->priv);

	if (G_OBJECT_CLASS (parent_class)->finalize)
		G_OBJECT_CLASS (parent_class)->finalize (object);
}

GtkWidget *
userlist_window_new (void)
{
	return GTK_WIDGET (g_object_new (userlist_window_get_type (), NULL));
}

void
userlist_window_show (UserlistWindow *window)
{
}

void
userlist_window_hide (UserlistWindow *window)
{
}

void
userlist_window_set_current (UserlistWindow *window, struct session *sess)
{
}

void
userlist_window_remove_session (UserlistWindow *window, struct session *sess)
{
}
