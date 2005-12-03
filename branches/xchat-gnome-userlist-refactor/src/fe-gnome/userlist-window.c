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
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "userlist-window.h"

#include "gui.h"
#include "palette.h"
#include "pixmaps.h"

static void         userlist_window_class_init  (UserlistWindowClass *klass);
static void         userlist_window_init        (UserlistWindow      *window);
static void         userlist_window_finalize    (GObject             *object);
static void         userlist_window_grab        (UserlistWindow      *window);
static gboolean     userlist_window_grab_broken (GtkWidget           *widget,
                                                 GdkEventGrabBroken  *event);
static gboolean     userlist_window_event       (GtkWidget           *widget,
                                                 GdkEvent            *event);
static GdkPixbuf   *get_user_icon               (struct server       *serv,
                                                 struct User         *user);
static GtkTreeIter *find_user                   (GtkListStore        *store,
                                                 struct User         *user);

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
	GObjectClass   *gobject_class;
	GtkWidgetClass *widget_class;

	parent_class = g_type_class_peek_parent (klass);

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = userlist_window_finalize;

	widget_class = GTK_WIDGET_CLASS (klass);
	widget_class->grab_broken_event = userlist_window_grab_broken;
	widget_class->event             = userlist_window_event;
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

	gtk_scrolled_window_set_policy      (GTK_SCROLLED_WINDOW (scrolledwindow),
	                                     GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_ETCHED_IN);
	gtk_container_add                   (GTK_CONTAINER (scrolledwindow), window->priv->treeview);
	gtk_container_set_border_width      (GTK_CONTAINER (scrolledwindow), 3);
	gtk_frame_set_shadow_type           (GTK_FRAME (frame), GTK_SHADOW_OUT);
	gtk_container_add                   (GTK_CONTAINER (frame), scrolledwindow);
	gtk_container_add                   (GTK_CONTAINER (window), frame);

	g_object_set                        (G_OBJECT   (window), "type", GTK_WINDOW_POPUP, NULL);
	gtk_window_set_decorated            (GTK_WINDOW (window), FALSE);
	gtk_window_set_type_hint            (GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_MENU);

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

static gboolean
userlist_window_grab_broken (GtkWidget *widget, GdkEventGrabBroken *event)
{
	UserlistWindow *window;

	window = USERLIST_WINDOW (widget);

	if (window->priv->have_grab && event->grab_window == NULL)
		userlist_window_hide (window);
	if (GTK_WIDGET_CLASS (parent_class)->grab_broken_event)
		return GTK_WIDGET_CLASS (parent_class)->grab_broken_event (widget, event);
	return TRUE;
}

static gboolean
userlist_window_event (GtkWidget *widget, GdkEvent *event)
{
	if (event->type == GDK_KEY_PRESS) {
		if (((GdkEventKey *)event)->keyval == GDK_Escape) {
			userlist_window_hide (USERLIST_WINDOW (widget));
			return FALSE;
		}
	}
	if (GTK_WIDGET_CLASS (parent_class)->event)
		return GTK_WIDGET_CLASS (parent_class)->event (widget, event);
	return FALSE;
}

static GdkPixbuf *
get_user_icon (struct server *serv, struct User *user)
{
	char *pre;
	int level;

	if (user == NULL)
		return NULL;

	/* handle common cases */
	switch (user->prefix[0]) {
	case '\0': return NULL;
	case '@':  return pix_op;
	case '%':  return pix_hop;
	case '+':  return pix_voice;
	}

	/* the user is someone very special.  find out how many levels above
	 * operator they are and return corresponding icon.
	 */
	pre = strchr (serv->nick_prefixes, '@');
	if (pre && pre != serv->nick_prefixes) {
		pre--;
		level = 0;
		while (1) {
			if (pre[0] == user->prefix[0]) {
				switch (level) {
				case 0: return pix_red;
				case 1: return pix_purple;
				}
				/* 3+, no icons */
				break;
			}
			level++;
			if (pre == serv->nick_prefixes)
				break;
			pre--;
		}
	}
	return NULL;
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
userlist_window_insert_user (UserlistWindow *window, struct session *sess, struct User *user, int row, gboolean selected)
{
	GtkListStore *store;
	GdkPixbuf    *icon;
	GtkTreeIter   iter;

	store = g_hash_table_lookup (window->priv->stores, sess);
	if (store == NULL) {
		store = gtk_list_store_new (4, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER, GDK_TYPE_COLOR);
		/* FIXME: create completion */
		g_hash_table_insert (window->priv->stores, sess, store);
	}

	icon = get_user_icon (sess->server, user);
	gtk_list_store_insert (store, &iter, row);
	gtk_list_store_set    (store, &iter, 0, icon, 1, user->nick, 2, user,
	                       3, user->away ? &colors[40] : NULL, -1);
	/* FIXME: completion */
}

gboolean
userlist_window_remove_user (UserlistWindow *window, struct session *sess, struct User *user)
{
	GtkListStore *store;

	store = g_hash_table_lookup (window->priv->stores, sess);
	if (store == NULL)
		return FALSE;

	/* FIXME: more */
}

static GtkTreeIter *
find_user (GtkListStore *store, struct User *user)
{
	static GtkTreeIter  iter;
	struct User        *row_user;
	GtkTreeModel       *model;

	model = GTK_TREE_MODEL (store);

	if (gtk_tree_model_get_iter_first (model, &iter)) {
		do {
			gtk_tree_model_get (model, &iter, 2, &row_user, -1);
			if (row_user == user)
				return &iter;
		} while (gtk_tree_model_iter_next (model, &iter));
	}
	return NULL;
}

void
userlist_window_update (UserlistWindow *window, struct session *sess, struct User *user)
{
	GtkListStore *store;
	gchar        *nick;
	GtkTreeIter  *iter;
	GList        *item;

	store = g_hash_table_lookup (window->priv->stores, sess);
	if (store == NULL)
		return;

	iter = find_user (store, user);
	gtk_tree_model_get (GTK_TREE_MODEL (store), iter, 1, &nick, -1);

	/* FIXME: remove from completion */

	gtk_list_store_set (store, iter, 1, user->nick, 3, user->away ? &colors[40] : NULL, -1);

	/* FIXME: add to completion */

	g_free (nick);
}

void
userlist_window_set_current (UserlistWindow *window, struct session *sess)
{
	GtkTreeModel *model;

	if (sess == window->priv->current)
		return;

	model = g_hash_table_lookup (window->priv->stores, sess);
	gtk_tree_view_set_model (GTK_TREE_VIEW (window->priv->treeview), model);

	window->priv->current = sess;
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
