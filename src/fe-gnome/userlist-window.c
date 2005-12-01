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
#include <gtk/gtk.h>
#include "userlist-window.h"
#include "gui.h"

static void userlist_window_class_init (UserlistWindowClass *klass);
static void userlist_window_init       (UserlistWindow      *window);
static void userlist_window_finalize   (GObject             *object);
static void userlist_window_grab       (UserlistWindow      *window);

static GtkWindowClass *parent_class;

G_DEFINE_TYPE (UserlistWindow, userlist_window, GTK_TYPE_WINDOW);

struct _UserlistWindowPriv
{
	GtkWidget      *treeview;

	GHashTable     *stores;
	struct session *current;
	gboolean        have_grab;
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

	window->priv->stores = g_hash_table_new (g_direct_hash, g_direct_equal);

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

static void
userlist_window_grab (UserlistWindow *window)
{
	if (window->priv->have_grab)
		return;
	window->priv->have_grab = (gdk_pointer_grab (GTK_WIDGET (window)->window,
	                                             TRUE, GDK_POINTER_MOTION_MASK |
	                                             GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
	                                             GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK,
	                                             NULL, NULL, GDK_CURRENT_TIME) ==
	                           GDK_GRAB_SUCCESS);
	if (window->priv->have_grab) {
		window->priv->have_grab = (gdk_keyboard_grab (GTK_WIDGET (window)->window, TRUE, GDK_CURRENT_TIME) == GDK_GRAB_SUCCESS);
		if (window->priv->have_grab == FALSE) {
			/* something bad happened */
			gdk_pointer_ungrab (GDK_CURRENT_TIME);
			userlist_window_hide (window);
			return;
		}
		gtk_grab_add (GTK_WIDGET (window));
	}
}

GtkWidget *
userlist_window_new (void)
{
	return GTK_WIDGET (g_object_new (userlist_window_get_type (), NULL));
}

void
userlist_window_show (UserlistWindow *window)
{
	gint screen_width, screen_height;
	gint desired_height;
	gint window_x, window_y;
	gint  mouse_x,  mouse_y;

	GdkDisplay *display;
	GdkScreen *mouse_screen;
	GtkRequisition request;

	/* FIXME: emit shown signal */

	/* Get the tree view's size request.  This lets us know how big the
	 * tree view would be if it could get all of the space it wanted.
	 */
	if (!GTK_WIDGET_REALIZED (window))
		gtk_widget_realize (GTK_WIDGET (window));
	gtk_widget_size_request (window->priv->treeview, &request);

	display = gdk_display_get_default ();
        gdk_display_get_pointer (display, &mouse_screen, &mouse_x, &mouse_y, NULL);

	gtk_window_set_screen (GTK_WINDOW (window), mouse_screen);
	screen_width  = gdk_screen_get_width  (mouse_screen);
        screen_height = gdk_screen_get_height (mouse_screen);

	/* Buffer of 20 pixels.  Would be nice to know exactly how much space
	 * the rest of the window's UI goop used up, but oh well.
	 */
	desired_height = request.height + 20;
	if (desired_height > screen_height)
		desired_height = screen_height;

	window_x = mouse_x - 100;
	window_y = mouse_y - (desired_height / 2);

	if (window_x < 0)
		window_x = 0;
	if (window_x + 250 > screen_width)
		window_x = screen_width - 250;
	if (window_y < 0)
		window_y = 0;
	if (window_y + desired_height > screen_height)
		window_y = screen_height - desired_height;
	gtk_window_move (GTK_WINDOW (window), window_x, window_y);

	gtk_window_resize (GTK_WINDOW (window), 250, desired_height);
	gtk_widget_show (GTK_WIDGET (window));
	gtk_window_set_focus (GTK_WINDOW (window), window->priv->treeview);
	userlist_window_grab (window);
}

void
userlist_window_hide (UserlistWindow *window)
{
	/* FIXME: emit hid signal */
	if (window->priv->have_grab) {
		gtk_grab_remove (GTK_WIDGET (window));
		gdk_pointer_ungrab (GDK_CURRENT_TIME);
		gdk_keyboard_ungrab (GDK_CURRENT_TIME);
		window->priv->have_grab = FALSE;
	}
	gtk_widget_hide (GTK_WIDGET (window));
	gtk_widget_grab_focus (gui.text_entry);
}

void
userlist_window_set_current (UserlistWindow *window, struct session *sess)
{
	GtkTreeModel *model;

	if (sess == window->priv->current)
		return;

	model = g_hash_table_lookup (window->priv->stores, sess);
	gtk_tree_view_set_model (GTK_TREE_VIEW (window->priv->treeview), model);
}

void
userlist_window_remove_session (UserlistWindow *window, struct session *sess)
{
	GtkTreeModel *model;

	model = g_hash_table_lookup (window->priv->stores, sess);
	g_hash_table_remove (window->priv->stores, sess);
	if (sess == window->priv->current)
		userlist_window_set_current (window, NULL);
	g_object_unref (model);
}
