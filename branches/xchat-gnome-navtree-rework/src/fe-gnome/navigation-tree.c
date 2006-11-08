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
#include "util.h"

/***** Actions *****/
static GtkActionEntry action_entries[] = {
	// Discussion context menu
	{"DiscussionSave",       GTK_STOCK_SAVE,           N_("_Save Transcript"), "", NULL, NULL},
	{"DiscussionLeave",      GTK_STOCK_QUIT,           N_("_Leave"),           "", NULL, NULL},
	{"DiscussionClose",      GTK_STOCK_CLOSE,          N_("_Close"),           "", NULL, NULL},
	{"DiscussionJoin",       GTK_STOCK_JUMP_TO,        N_("_Join"),            "", NULL, NULL},
	{"DiscussionFind",       GTK_STOCK_FIND,           N_("_Find..."),         "", NULL, NULL},
	{"DiscussionBans",       GTK_STOCK_DIALOG_WARNING, N_("_Bans..."),         "", NULL, NULL},
};

static GtkToggleActionEntry toggle_action_entries[] = {
	// Server context menu
	{"NetworkAutoConnect", NULL, N_("_Auto-connect on startup"), "", NULL, NULL, FALSE},

	// Discussion context menu
	{"DiscussionAutoJoin",   NULL, N_("_Auto-join on connect"),    "", NULL, NULL, FALSE}
};


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

	GtkTreeIter iter;
	if (find_session (model, sess, &iter, NULL) == FALSE) {
		return;
	}

	GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
	gtk_tree_selection_select_iter (selection, &iter);
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

			GtkAction *action;

			action = gtk_action_group_get_action (tree->priv->action_group, "DiscussionAutoJoin");
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
