/*
 * navigation-tree.c - functions to create and maintain the navigation tree
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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
#include <glib/gi18n.h>
#include "navigation-tree.h"
#include "palette.h"
#include "util.h"
#include "../common/fe.h"

/***** NavTree *****/
static void navigation_tree_init       (NavTree *navtree);
static void navigation_tree_class_init (NavTreeClass *klass);
static void navigation_tree_dispose    (GObject *object);
static void navigation_tree_finalize   (GObject *object);
static gint tree_iter_sort_func_nocase (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data);

static void     row_inserted      (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, NavTree *tree);
static void     selection_changed (GtkTreeSelection *selection, NavTree *tree);
static gboolean button_pressed    (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static gboolean button_released   (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static gboolean popup_menu        (GtkWidget *widget, GdkEventButton *event);

static void     jump_to_discussion (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void     select_nth_channel (NavTree *navtree, gint num);

static void     go_previous_network    (GtkAction *action, gpointer data);
static void     go_next_network        (GtkAction *action, gpointer data);
static void     go_previous_discussion (GtkAction *action, gpointer data);
static void     go_next_discussion     (GtkAction *action, gpointer data);
static void     join_channel           (GtkAction *action, gpointer data);


#define NAVTREE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NAVTREE_TYPE, NavTreePriv))
struct _NavTreePriv
{
	GtkActionGroup *action_group;

	gint selection_changed_id;
	GtkTreeRowReference *selected;
};

enum
{
	COLUMN_NAME,
	COLUMN_SESSION,
	COLUMN_ICON,
	COLUMN_STATUS,
	COLUMN_COLOR,
	COLUMN_CONNECTED,
	COLUMN_REFCOUNT,
	N_COLUMNS
};

/********** NavModel **********/

static void     navigation_model_init       (NavModel * navmodel);
static void     navigation_model_class_init (NavModelClass * klass);
static void     navigation_model_dispose    (GObject * object);
static void     navigation_model_finalize   (GObject * object);

/********** Utility **********/

static gboolean find_server  (NavModel *model, server *server, GtkTreeIter *iter);
static gboolean find_session (NavModel *model, session *sess, GtkTreeIter *iter, GtkTreeIter *parent);
static gboolean channel_is_autojoin (session *sess);


/***** Actions *****/
static GtkActionEntry action_entries[] = {
	// Go menu
	{ "GoPreviousNetwork",    NULL, N_("Pre_vious Network"),    "<control>Up",   NULL, G_CALLBACK (go_previous_network) },
	{ "GoNextNetwork",        NULL, N_("Nex_t Network"),        "<control>Down", NULL, G_CALLBACK (go_next_network) },
	{ "GoPreviousDiscussion", NULL, N_("_Previous Discussion"), "<alt>Up",       NULL, G_CALLBACK (go_previous_discussion) },
	{ "GoNextDiscussion",     NULL, N_("_Next Discussion"),     "<alt>Down",     NULL, G_CALLBACK (go_next_discussion) },

	// Discussion context menu
	{"DiscussionJoin", GTK_STOCK_JUMP_TO, N_("_Join"), "", NULL, G_CALLBACK (join_channel) },
};

static GtkToggleActionEntry toggle_action_entries[] = {
	// Server context menu
	{"NetworkAutoConnect", NULL, N_("_Auto-connect on startup"), "", NULL, NULL, FALSE},

	// Discussion context menu
	{"DiscussionAutoJoin",   NULL, N_("_Auto-join on connect"),    "", NULL, NULL, FALSE}
};


GType
navigation_tree_get_type (void)
{
	static GType navigation_tree_type = 0;

	if (!navigation_tree_type) {
		static const GTypeInfo navigation_tree_info = {
			sizeof (NavTreeClass),
			NULL,		/* base init. */
			NULL,		/* base finalize. */
			(GClassInitFunc) navigation_tree_class_init,
			NULL,		/* class finalize. */
			NULL,		/* class data. */
			sizeof (NavTree),
			0,		/* n_preallocs. */
			(GInstanceInitFunc) navigation_tree_init,
		};

		navigation_tree_type = g_type_register_static (GTK_TYPE_TREE_VIEW, "NavTree", &navigation_tree_info, 0);
	}

	return navigation_tree_type;
}

static void
navigation_tree_init (NavTree *navtree)
{
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;

	navtree->priv = NAVTREE_GET_PRIVATE (navtree);

	g_object_set ((gpointer) navtree, "headers-visible", FALSE, NULL);

	column = gtk_tree_view_column_new ();
	icon_renderer = gtk_cell_renderer_pixbuf_new ();
	text_renderer = gtk_cell_renderer_text_new ();

	gtk_tree_view_column_pack_start (column, icon_renderer, FALSE);
	gtk_tree_view_column_set_attributes (column, icon_renderer, "icon-name", COLUMN_ICON, NULL);

	gtk_tree_view_column_pack_start (column, text_renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, text_renderer, "text", COLUMN_NAME, "foreground-gdk", COLUMN_COLOR, NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW (navtree), column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_BROWSE);
	navtree->priv->selection_changed_id = g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (selection_changed), navtree);

	navtree->priv->action_group = gtk_action_group_new ("NavigationContext");
	gtk_action_group_set_translation_domain (navtree->priv->action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (navtree->priv->action_group, action_entries, G_N_ELEMENTS (action_entries), NULL);
	gtk_action_group_add_toggle_actions (navtree->priv->action_group, toggle_action_entries, G_N_ELEMENTS (toggle_action_entries), NULL);
	gtk_ui_manager_insert_action_group (gui.manager, navtree->priv->action_group, -1);

	g_signal_connect (G_OBJECT (navtree), "button_press_event", G_CALLBACK (button_pressed), NULL);
	g_signal_connect (G_OBJECT (navtree), "button_release_event", G_CALLBACK (button_released), NULL);
	g_signal_connect (G_OBJECT (navtree), "popup_menu", G_CALLBACK (popup_menu), NULL);
}

static void
navigation_tree_class_init (NavTreeClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;

	g_type_class_add_private (object_class, sizeof (NavTreePriv));

	object_class->dispose = navigation_tree_dispose;
	object_class->finalize = navigation_tree_finalize;
}

static void
navigation_tree_dispose (GObject *object)
{
}

static void
navigation_tree_finalize (GObject *object)
{
}

NavTree *
navigation_tree_new (NavModel *model)
{
	g_return_val_if_fail (model != NULL, NULL);

	NavTree *tree = NAVTREE (g_object_new (navigation_tree_get_type (), NULL));
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree), GTK_TREE_MODEL (model));

	g_signal_connect (G_OBJECT (model), "row-inserted", G_CALLBACK (row_inserted), tree);

	return tree;
}

void
navigation_tree_select_session (NavTree *tree, session *sess)
{
	NavModel *model = NAVMODEL (gtk_tree_view_get_model (GTK_TREE_VIEW (tree)));
	g_assert (model != NULL);

	GtkTreeIter iter;
	if (find_session (model, sess, &iter, NULL) == FALSE) {
		return;
	}

	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
	gtk_tree_selection_select_iter (selection, &iter);
}

void
navigation_tree_remove_session (NavTree *tree, session *sess)
{
	NavModel *model = NAVMODEL (gtk_tree_view_get_model (GTK_TREE_VIEW (tree)));
	g_assert (model != NULL);

	GtkTreeIter iter;
	if (find_session (model, sess, &iter, NULL) == FALSE) {
		return;
	}

	if (gtk_tree_model_iter_has_child (GTK_TREE_MODEL (model), &iter)) {
		GtkTreeIter child;

		/*
		 * This will cause navigation_tree_remove_session to get
		 * called for each child, invalidating the child iter.  To
		 * get around this, just iterate until the parent iter has
		 * no children.
		 */
		while (gtk_tree_model_iter_children (GTK_TREE_MODEL (model), &child, &iter)) {
			session *sess;
			gtk_tree_model_get (GTK_TREE_MODEL (model), &child, COLUMN_SESSION, &sess, -1);
			fe_close_window (sess);
		}
	}

	// Try to find an appropriate item to select.
	GtkTreeIter new_selection = iter;
	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
	if (gtk_tree_model_iter_next (GTK_TREE_MODEL (model), &new_selection)) {
		gtk_tree_selection_select_iter (selection, &new_selection);
	} else {
		// Try to select the first item.
		if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (model), &new_selection)) {
			gtk_tree_selection_select_iter (selection, &new_selection);
		}
	}

	gtk_tree_store_remove (GTK_TREE_STORE (model), &iter);
}

static void
row_inserted (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, NavTree *tree)
{
	/*
	 * If this is the first child of a server, expand that server.  This is
	 * so things will be expanded by default, but if someone closes a
	 * server, it won't override that any time there's a new channel opened.
	 */
	if (gtk_tree_path_get_depth (path) == 2) {
		GtkTreeIter parent;
		gtk_tree_model_iter_parent (model, &parent, iter);
		if (gtk_tree_model_iter_n_children (model, &parent) == 1) {
			gtk_tree_view_expand_to_path (GTK_TREE_VIEW (tree), path);
		}
	}

	/*
	 * If there is no selection, select this row.
	 */
	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
	if (gtk_tree_selection_get_selected (selection, NULL, NULL) == FALSE) {
		gtk_tree_selection_select_path (selection, path);
	}
}

static void
selection_changed (GtkTreeSelection *selection, NavTree *tree)
{
	GtkTreeIter   iter;
	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree));

	if (tree->priv->selected && gtk_tree_row_reference_valid (tree->priv->selected)) {
		GtkTreePath *path = gtk_tree_row_reference_get_path (tree->priv->selected);
		gtk_tree_row_reference_free (tree->priv->selected);
		tree->priv->selected = NULL;

		gtk_tree_model_get_iter (model, &iter, path);

		gint refcount;
		gtk_tree_model_get (model, &iter, COLUMN_REFCOUNT, &refcount, -1);
		gtk_tree_store_set (GTK_TREE_STORE (model), &iter, COLUMN_REFCOUNT, refcount - 1, -1);

		gtk_tree_path_free (path);
	}

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		session *sess;
		gtk_tree_model_get (model, &iter, COLUMN_SESSION, &sess, -1);
		if (sess) {
			fe_set_current (sess);

			GtkAction *action = gtk_action_group_get_action (tree->priv->action_group, "DiscussionAutoJoin");
			gtk_action_set_sensitive (action, sess->server->network != NULL);
			gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), channel_is_autojoin (sess));

			ircnet *network = (ircnet*) sess->server->network;
			if (network != NULL) {
				action = gtk_action_group_get_action (tree->priv->action_group, "NetworkAutoConnect");
				gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), network->flags & FLAG_AUTO_CONNECT);
			}
		}

		GtkTreePath *path = gtk_tree_model_get_path (model, &iter);
		tree->priv->selected = gtk_tree_row_reference_new (model, path);
		gtk_tree_path_free (path);
	}
}

static gboolean
button_pressed (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	if (event == NULL) {
		return FALSE;
	}

	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
	if (gtk_tree_selection_get_selected (selection, NULL, NULL) == FALSE) {
		// Corner case
		return FALSE;
	}

	if (event->button == 3) {
		GtkTreePath *path;
		if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (widget), event->x, event->y, &path, 0, 0, 0)) {
			gtk_tree_selection_unselect_all (selection);
			gtk_tree_selection_select_path (selection, path);
			gtk_tree_path_free (path);
			popup_menu (widget, event);
		}

		return TRUE;
	}

	g_object_set (G_OBJECT (widget), "can-focus", FALSE, NULL);
	return FALSE;
}

static gboolean
button_released (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	/*
	 * Grab focus back to the text entry, so people don't have to re-focus
	 * it after switching channels.  GtkEntry selects the entire thing when
	 * it grabs focus, so this requires saving the cursor position first,
	 * and restoring it after.
	 */
	gint position = gtk_editable_get_position (GTK_EDITABLE (gui.text_entry));
	gtk_widget_grab_focus (gui.text_entry);
	gtk_editable_set_position (GTK_EDITABLE (gui.text_entry), position);
	g_object_set (G_OBJECT (widget), "can-focus", TRUE, NULL);
	return FALSE;
}

static gboolean
popup_menu (GtkWidget *widget, GdkEventButton *event)
{
	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));

	GtkTreeModel *model;
	GtkTreeIter   iter;
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		session *sess;
		gtk_tree_model_get (model, &iter, COLUMN_SESSION, &sess, -1);

		g_assert (sess != NULL);

		GtkWidget *menu = NULL;

		switch (sess->type) {
		case SESS_SERVER:
			menu = gtk_ui_manager_get_widget (gui.manager, "/NetworkPopup");
			break;

		case SESS_CHANNEL:
			if (sess->total) {
				menu = gtk_ui_manager_get_widget (gui.manager, "/ChannelJoinedPopup");
			} else {
				menu = gtk_ui_manager_get_widget (gui.manager, "/ChannelUnjoinedPopup");
			}
			break;

		case SESS_DIALOG:
			menu = gtk_ui_manager_get_widget (gui.manager, "/DialogPopup");
			break;
		}

		g_assert (menu != NULL);

		if (event) {
			gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
			                event->button, event->time);
		} else {
			gtk_menu_popup (GTK_MENU (menu), NULL, NULL,
			                menu_position_under_tree_view, widget,
			                0, gtk_get_current_event_time ());
			gtk_menu_shell_select_first (GTK_MENU_SHELL (menu), FALSE);
		}
	}

	return TRUE;
}

GType
navigation_model_get_type (void)
{
	static GType navigation_model_type = 0;
	if (!navigation_model_type) {
		static const GTypeInfo navigation_model_info = {
			sizeof (NavTreeClass),
			NULL, /* base init. */
			NULL, /* base finalize. */
			(GClassInitFunc) navigation_model_class_init,
			NULL, /* class_finalize. */
			NULL, /* class_data. */
			sizeof (NavModel),
			0, /* n_preallocs. */
			(GInstanceInitFunc) navigation_model_init,
		};

		navigation_model_type = g_type_register_static (GTK_TYPE_TREE_STORE, "NavModel", &navigation_model_info, 0);
	}

	return navigation_model_type;
}

static void
navigation_model_init (NavModel *navmodel)
{
	GType column_types[] = {
		G_TYPE_STRING,  // name
		G_TYPE_POINTER, // session pointer
		G_TYPE_STRING,  // status image icon-name
		G_TYPE_INT,     // status value (for tracking highest state)
		GDK_TYPE_COLOR, // status color (disconnected, etc.)
		G_TYPE_BOOLEAN, // connected
		G_TYPE_INT,     // reference count
	};

	gtk_tree_store_set_column_types (GTK_TREE_STORE (navmodel), N_COLUMNS, column_types);

	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (navmodel), 1, GTK_SORT_ASCENDING);
	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (navmodel), 1, tree_iter_sort_func_nocase, NULL, NULL);
}

static void
navigation_model_class_init (NavModelClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;
	object_class->dispose = navigation_model_dispose;
	object_class->finalize = navigation_model_finalize;
}

static void
navigation_model_dispose (GObject *object)
{
}

static void
navigation_model_finalize (GObject *object)
{
}

NavModel *
navigation_model_new ()
{
	return NAVMODEL (g_object_new (navigation_model_get_type (), NULL));
}

void
navigation_model_add_server (NavModel *model, session *sess)
{
	GtkTreeIter iter;
	gtk_tree_store_append (GTK_TREE_STORE (model), &iter, NULL);

	/* Set default values */
	gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
	                    COLUMN_NAME,      _("<none>"),
	                    COLUMN_SESSION,   sess,
	                    COLUMN_ICON,      NULL,
	                    COLUMN_STATUS,    0,
	                    COLUMN_COLOR,     NULL,
	                    COLUMN_CONNECTED, FALSE,
	                    COLUMN_REFCOUNT,  0,
	                    -1);
}

void
navigation_model_add_channel (NavModel *model, session *sess)
{
	GtkTreeIter parent;
	if (find_server (model, sess->server, &parent) == FALSE) {
		return;
	}

	GtkTreeIter child;
	gtk_tree_store_append (GTK_TREE_STORE (model), &child, &parent);
	gtk_tree_store_set (GTK_TREE_STORE (model), &child,
	                    COLUMN_NAME,      sess->channel,
	                    COLUMN_SESSION,   sess,
	                    COLUMN_ICON,      NULL,
	                    COLUMN_STATUS,    0,
	                    COLUMN_COLOR,     NULL,
	                    COLUMN_CONNECTED, TRUE,
	                    COLUMN_REFCOUNT,  0,
	                    -1);
}

void
navigation_model_update (NavModel *model, session *sess)
{
	GtkTreeIter iter;
	if (find_session (model, sess, &iter, NULL) == FALSE) {
		return;
	}

	switch (sess->type) {
	case SESS_SERVER:
		gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
		                    COLUMN_NAME,      sess->channel,
		                    COLUMN_COLOR,     NULL,
		                    COLUMN_CONNECTED, TRUE,
		                    -1);
		break;
	case SESS_CHANNEL:
		gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
		                    COLUMN_NAME,      sess->channel,
		                    COLUMN_COLOR,     NULL,
		                    COLUMN_CONNECTED, TRUE,
		                    -1);
		break;
	}
}

void
navigation_model_set_disconnected (NavModel *model, session *sess)
{
	GtkTreeIter iter;
	if (find_session (model, sess, &iter, NULL) == FALSE) {
		return;
	}

	gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
	                    COLUMN_COLOR,     &colors[40],
	                    COLUMN_CONNECTED, FALSE,
	                    -1);
}

void
navigation_model_set_hilight (NavModel *model, session *sess)
{
	GtkTreeIter iter;
	if (find_session (model, sess, &iter, NULL) == FALSE) {
		return;
	}

	gint shown_level, refcount;
	gtk_tree_model_get (GTK_TREE_MODEL (model), &iter,
	                    COLUMN_STATUS,   &shown_level,
	                    COLUMN_REFCOUNT, &refcount,
	                    -1);

	if (refcount > 0) {
		return;
	}

	if (sess->nick_said || (sess->msg_said && sess->type == SESS_DIALOG)) {
		gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
		                    COLUMN_ICON,   "xchat-gnome-message-nickname-said",
		                    COLUMN_STATUS, 3,
		                    -1);
	} else if (sess->msg_said && shown_level <= 1) {
		gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
		                    COLUMN_ICON,   "xchat-gnome-message-new",
		                    COLUMN_STATUS, 2,
		                    -1);
	} else if (sess->new_data && shown_level == 0) {
		gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
		                    COLUMN_ICON,   "xchat-gnome-message-data",
		                    COLUMN_STATUS, 1,
		                    -1);
	}
}

void
navigation_model_set_current (NavModel *model, session *sess)
{
	GtkTreeIter iter;
	if (find_session (model, sess, &iter, NULL) == FALSE) {
		return;
	}

	gint refcount;
	gtk_tree_model_get (GTK_TREE_MODEL (model), &iter,
	                    COLUMN_REFCOUNT, &refcount,
	                    -1);

	gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
	                    COLUMN_ICON,     NULL,
	                    COLUMN_STATUS,   0,
			    COLUMN_REFCOUNT, refcount + 1,
	                    -1);
}

static gint
tree_iter_sort_func_nocase (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data)
{
	gchar *as, *bs;
	gint result;

	gtk_tree_model_get (model, a, COLUMN_NAME, &as, -1);
	gtk_tree_model_get (model, b, COLUMN_NAME, &bs, -1);

	if (as == NULL) {
		return 1;
	}

	if (bs == NULL) {
		g_free (as);
		return -1;
	}

	result = strcasecmp (as, bs);

	g_free (as);
	g_free (bs);

	/*
	 * GtkTreeSortable has undefined results if this function isn't
	 * reflexive, antisymmetric and transitive.  If the two strings are
	 * equal, compare session pointers
	 */
	if (result == 0) {
		gpointer ap, bp;
		gtk_tree_model_get (model, a, COLUMN_SESSION, &ap, -1);
		gtk_tree_model_get (model, b, COLUMN_SESSION, &bp, -1);

		return (GPOINTER_TO_UINT(ap) - GPOINTER_TO_UINT(bp));
	}

	return result;

}

static gboolean
find_server (NavModel *model, server *server, GtkTreeIter *iter)
{
	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (model), iter) == FALSE) {
		return FALSE;
	}

	do {
		session *sess;
		gtk_tree_model_get (GTK_TREE_MODEL (model), iter, COLUMN_SESSION, &sess, -1);

		if (sess->type == SESS_SERVER && sess->server == server) {
			return TRUE;
		}
	} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (model), iter));

	return FALSE;
}

static gboolean
find_session (NavModel *model, session *sess, GtkTreeIter *iter, GtkTreeIter *parent)
{
	if (parent == NULL) {
		if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (model), iter) == FALSE) {
			return FALSE;
		}
	} else {
		if (gtk_tree_model_iter_children (GTK_TREE_MODEL (model), iter, parent) == FALSE) {
			return FALSE;
		}
	}

	do {
		session *sess2;
		gtk_tree_model_get (GTK_TREE_MODEL (model), iter, COLUMN_SESSION, &sess2, -1);

		if (sess == sess2) {
			return TRUE;
		}

		if (gtk_tree_model_iter_has_child (GTK_TREE_MODEL (model), iter)) {
			GtkTreeIter child;
			if (find_session (model, sess, &child, iter) == TRUE) {
				*iter = child;
				return TRUE;
			}
		}
	} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (model), iter));

	return FALSE;
}

static gboolean
channel_is_autojoin (session *sess)
{
	ircnet *network = (ircnet *) sess->server->network;

	if (network == NULL || network->autojoin == NULL) {
		return FALSE;
	}

	gchar **autojoin = g_strsplit (network->autojoin, " ", 0);
	gchar **channels = g_strsplit (autojoin[0], ",", 0);

	gboolean found = FALSE;
	for (int i = 0; channels[i]; i++) {
		if (!strcmp (channels[i], sess->channel)) {
			found = TRUE;
			break;
		}
	}

	g_strfreev (autojoin);
	g_strfreev (channels);

	return found;
}

void
navigation_tree_add_accels (NavTree *navtree, GtkWindow *window)
{
	GtkAccelGroup *discussion_accel = gtk_accel_group_new ();
	GClosure *closure;

	/*
	 * For alt-1 through alt-9 we just loop to set up the accelerators.
	 * We want the data passed with the callback to be one less then the
	 * button pressed (e.g. alt-1 requests the channel who's path is 0:0)
	 * so we loop from 0 <= i < 1. We use i for the user data and the ascii
	 * value of i+1 to get the keyval.
	 */
	for (int i = 0; i < 9; i++) {
		// Set up our GClosure with user data set to i.
		closure = g_cclosure_new (G_CALLBACK (jump_to_discussion), GINT_TO_POINTER (i), NULL);

		// Connect up the accelerator.
		gtk_accel_group_connect (discussion_accel, GDK_1 + i, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);

		// Delete the reference to the GClosure.
		g_closure_unref (closure);
	}

	// Now we set up keypress alt-0 with user data 9.
	closure = g_cclosure_new (G_CALLBACK (jump_to_discussion), GUINT_TO_POINTER (9), NULL);
	gtk_accel_group_connect (discussion_accel, gdk_keyval_from_name ("0"), GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);
	g_closure_unref (closure);

	// alt+ and alt-
	closure = g_cclosure_new (G_CALLBACK (go_next_discussion), NULL, NULL);
	gtk_accel_group_connect (discussion_accel, gdk_keyval_from_name ("equal"), GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);
	g_closure_unref (closure);

	closure = g_cclosure_new (G_CALLBACK (go_previous_discussion), NULL, NULL);
	gtk_accel_group_connect (discussion_accel, gdk_keyval_from_name ("minus"), GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);
	g_closure_unref (closure);

	/*
	 * We've had a couple of requests to hook up Ctrl-Alt-PgUp and
	 * Ctrl-Alt-PgDown to discussion navigation. As far as I can
	 * tell this is not HIG compliant, but for the time being we'll
	 * put it in. It's easy enough to delete it later.
	 */
	closure = g_cclosure_new (G_CALLBACK (go_next_discussion), NULL, NULL);
	gtk_accel_group_connect (discussion_accel, GDK_Page_Down, GDK_MOD1_MASK | GDK_CONTROL_MASK , GTK_ACCEL_VISIBLE, closure);
	g_closure_unref (closure);

	closure = g_cclosure_new (G_CALLBACK (go_previous_discussion), NULL, NULL);
	gtk_accel_group_connect (discussion_accel, GDK_Page_Up, GDK_MOD1_MASK | GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, closure);
	g_closure_unref (closure);

	// Add the accelgroup to the main window.
	gtk_window_add_accel_group (window, discussion_accel);
}

static void
jump_to_discussion (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data)
{
	select_nth_channel (gui.server_tree, GPOINTER_TO_INT (data));
}

static void
select_nth_channel (NavTree *navtree, gint num)
{
	GtkTreeView *view = GTK_TREE_VIEW (navtree);
	GtkTreeSelection *selection = gtk_tree_view_get_selection (view);
	GtkTreeModel *model = gtk_tree_view_get_model (view);

	// Make sure we get the an iter in the tree.
	GtkTreeIter server;
	if (model == NULL || gtk_tree_model_get_iter_first (model, &server) == FALSE) {
		return;
	}

	// Loop until we run out of channels or until we find the one we're looking for.
	do {
		// Only count the channels in the server if the list is expanded.
		GtkTreePath *path = gtk_tree_model_get_path (model, &server);
		if (!gtk_tree_view_row_expanded (view, path)) {
			continue;
		}

		gint kids = gtk_tree_model_iter_n_children (model, &server);
		if (num < kids) {
			GtkTreeIter child;
			gtk_tree_model_iter_nth_child (model, &child, &server, num);
			gtk_tree_selection_select_iter (selection, &child);
			return;
		}

		/*
		 * If our number wants a channel out of the range of this server
		 * subtract the number of channels in the current server so that
		 * when we find the server that contains the channel we want chan_num
		 * will be the channel's position in the list.
		 */
		num -= kids;
	} while (gtk_tree_model_iter_next (model, &server));
}

static void
go_previous_network (GtkAction *action, gpointer data)
{
	NavTree *navtree = gui.server_tree;
	NavTreePriv *priv = NAVTREE_GET_PRIVATE (navtree);

	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreePath *path;
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		// If there is a selection set path to that point.
		path = gtk_tree_model_get_path (model, &iter);
	} else {
		if (priv->selected) {
			path = gtk_tree_row_reference_get_path (priv->selected);
		} else {
			return;
		}
	}

	// If the path isn't a server move it up one.
	if (gtk_tree_path_get_depth (path) == 2) {
		gtk_tree_path_up (path);
	}

	if (gtk_tree_path_prev (path)) {
		gtk_tree_selection_select_path (selection, path);
	}

	gtk_tree_path_free(path);
}

static void
go_next_network (GtkAction *action, gpointer data)
{
	NavTree *navtree = gui.server_tree;
	NavTreePriv *priv = NAVTREE_GET_PRIVATE (navtree);

	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreePath *path;
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		// If there is a selection set path to that point.
		path = gtk_tree_model_get_path (model, &iter);
	} else {
		if (priv->selected) {
			path = gtk_tree_row_reference_get_path (priv->selected);
		} else {
			return;
		}
	}

	// If the path isn't a server move it up one.
	if (gtk_tree_path_get_depth (path) == 2) {
		gtk_tree_path_up (path);
	}

	if (gtk_tree_model_get_iter (model, &iter, path)) {
		if (gtk_tree_model_iter_next (model, &iter)) {
			gtk_tree_selection_select_iter (selection, &iter);
		}
	}

	gtk_tree_path_free(path);
}

static void
go_previous_discussion (GtkAction *action, gpointer data)
{
	NavTree *navtree = gui.server_tree;

	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	/*
	 * Try to get the currently selected item. Failing that, select the
	 * first visible channel.
	 */
	GtkTreeIter iter;
	GtkTreeModel *model;
	if (!gtk_tree_selection_get_selected (selection, &model, &iter)) {
		navigation_tree_select_session (navtree, 0);
		return;
	}

	GtkTreePath *path = gtk_tree_model_get_path (model, &iter);
	gint depth = gtk_tree_path_get_depth (path);

	if (depth == 2) {
		// If a channel is selected...
		if (!gtk_tree_path_prev (path)) {
			/*
			 * If it's the first channel on the server, pretend we've got
			 * that server selected.
			 */
			depth = 1;
			gtk_tree_path_up (path);
		}
	}

	if (depth == 1) {
		if (!gtk_tree_path_prev (path)) {
			/*
			 * If it's the first server, just move to the last
			 * channel in the tree.
			 */
			gtk_tree_path_free (path);

			GtkTreeIter parent;
			gtk_tree_model_iter_nth_child (model, &parent, NULL, gtk_tree_model_iter_n_children (model, NULL) - 1);

			GtkTreeIter child;
			gtk_tree_model_iter_nth_child (model, &child, &parent, gtk_tree_model_iter_n_children (model, &parent) - 1);
			path = gtk_tree_model_get_path (model, &child);
		} else {
			// Find a server somewhere above that has children.
			do {
				int children = 0;

				gtk_tree_model_get_iter (model, &iter, path);
				children = gtk_tree_model_iter_n_children (model, &iter);

				/*
				 * If the server has children and is expanded,
				 * set the path to the last entry on this server.
				 */
				if (children > 0 && gtk_tree_view_row_expanded (GTK_TREE_VIEW (navtree), path)) {
					gtk_tree_path_append_index (path, children-1);
					break;
				}
			} while (gtk_tree_path_prev (path));

			/*
			 * If we haven't selected a channel at this point, we
			 * need to select the last channel in the tree.
			 */
			if (gtk_tree_path_get_depth (path) == 1) {
				gtk_tree_path_free (path);

				GtkTreeIter parent;
				gtk_tree_model_iter_nth_child (model, &parent, NULL, gtk_tree_model_iter_n_children (model, NULL) - 1);

				GtkTreeIter child;
				gtk_tree_model_iter_nth_child (model, &child, &parent, gtk_tree_model_iter_n_children (model, &parent) - 1);
				path = gtk_tree_model_get_path (model, &child);
			}
		}
	}

	/*
	 * At this point path should point to the correct channel for
	 * selection.  If we can't get that iter for some reason, clean up
	 * and bail out.
	 */
	if (!gtk_tree_model_get_iter (model, &iter, path)) {
		gtk_tree_path_free (path);
		return;
	}

	gtk_tree_path_free (path);
	gtk_tree_selection_select_iter (selection, &iter);
}

static void
go_next_discussion (GtkAction *action, gpointer data)
{
	NavTree *navtree = gui.server_tree;

	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	// If nothing is currently selected, select the first visible channel.
	GtkTreeIter iter;
	GtkTreeModel *model;
	if (!gtk_tree_selection_get_selected (selection, &model, &iter)) {
		select_nth_channel (navtree, 0);
		return;
	}

	GtkTreeIter parent;
	GtkTreePath *path = gtk_tree_model_get_path (model, &iter);
	gint depth = gtk_tree_path_get_depth (path);
	gtk_tree_path_free (path);


	if (depth == 2) {
		gtk_tree_model_iter_parent (model, &parent, &iter);

		// Simple case
		if (gtk_tree_model_iter_next (model, &iter)) {
			gtk_tree_selection_select_iter (selection, &iter);
			return;
		}

		if (!gtk_tree_model_iter_next (model, &parent)) {
			gtk_tree_model_get_iter_first (model, &parent);
		}

		do {
			/*
			 * This can proceed indefinitely, because there is
			 * definitely at least one channel joined.
			 *
			 * FIXME: however, it's possible that said channel is
			 * inside an un-expanded server.  That'd be bad.
			 */
			GtkTreePath *path = gtk_tree_model_get_path (model, &parent);
			if (gtk_tree_model_iter_has_child (model, &parent) &&
			    gtk_tree_view_row_expanded (GTK_TREE_VIEW (navtree), path)) {
				gtk_tree_model_iter_children (model, &iter, &parent);
				gtk_tree_selection_select_iter (selection, &iter);
				gtk_tree_path_free (path);
				return;
			}
			gtk_tree_path_free (path);

			if (!gtk_tree_model_iter_next (model, &parent)) {
				gtk_tree_model_get_iter_first (model, &parent);
			}
		} while (TRUE);
	} else {
		parent = iter;

		GtkTreePath *old = gtk_tree_model_get_path (model, &iter);
		do {
			GtkTreePath *path = gtk_tree_model_get_path (model, &parent);
			if (gtk_tree_model_iter_has_child (model, &parent) &&
			    gtk_tree_view_row_expanded (GTK_TREE_VIEW (navtree), path)) {
				gtk_tree_model_iter_children (model, &iter, &parent);
				gtk_tree_selection_select_iter (selection, &iter);
				gtk_tree_path_free (path);
				return;
			}
			gtk_tree_path_free (path);

			if (!gtk_tree_model_iter_next (model, &parent)) {
				gtk_tree_model_get_iter_first (model, &parent);
			}

			/*
			 * FIXME: Since this search wraps, it's possible that it could
			 * infinite loop if there are no channels joined.  Fix that.
			 */
			GtkTreePath *current = gtk_tree_model_get_path (model, &parent);
			if (gtk_tree_path_compare (old, current) == 0) {
				gtk_tree_path_free (old);
				gtk_tree_path_free (current);
				return;
			}
			gtk_tree_path_free (current);
		} while (TRUE);
	}
}

static void
join_channel (GtkAction *action, gpointer data)
{
	NavTree *navtree = gui.server_tree;

	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	// If nothing is currently selected, bail out
	GtkTreeIter iter;
	GtkTreeModel *model;
	if (!gtk_tree_selection_get_selected (selection, &model, &iter)) {
		return;
	}

	session *sess;
	gchar *channel;
	gboolean connected;
	gtk_tree_model_get (model, &iter,
	                    COLUMN_NAME,      &channel,
	                    COLUMN_SESSION,   &sess,
	                    COLUMN_CONNECTED, &connected,
	                    -1);
	g_assert (sess->type == SESS_CHANNEL);
	g_assert (sess->server != NULL);
	g_assert (sess->server->connected == TRUE);
	sess->server->p_join(sess->server, channel, sess->channelkey);
	g_free (channel);
}

static void
set_action_state (NavTree *navtree)
{
	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	// If nothing is currently selected, very few things will work
	GtkTreeIter iter;
	GtkTreeModel *model;
	if (!gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_action_set_sensitive (gtk_action_group_get_action (navtree->priv->action_group, "DiscussionJoin"),     FALSE);
		gtk_action_set_sensitive (gtk_action_group_get_action (navtree->priv->action_group, "NetworkAutoConnect"), FALSE);
		gtk_action_set_sensitive (gtk_action_group_get_action (navtree->priv->action_group, "DiscussionAutoJoin"), FALSE);

		return;
	}

	session *sess;
	gboolean connected;
	gtk_tree_model_get (model, &iter,
	                    COLUMN_SESSION,   &sess,
	                    COLUMN_CONNECTED, &connected,
	                    -1);

}
