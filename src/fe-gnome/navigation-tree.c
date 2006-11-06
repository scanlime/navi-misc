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
        // Server context menu
	{"ServerReconnect",   GTK_STOCK_REFRESH,        N_("_Reconnect"),       "", NULL, NULL},
	{"ServerDisconnect",  GTK_STOCK_STOP,           N_("_Disconnect"),      "", NULL, NULL},
	{"ServerClose",       GTK_STOCK_CLOSE,          N_("_Close"),           "", NULL, NULL},
	{"ServerChannels",    GTK_STOCK_INDEX,          N_("_Channels..."),     "", NULL, NULL},

	// Channel context menu
	{"ChannelSave",       GTK_STOCK_SAVE,           N_("_Save Transcript"), "", NULL, NULL},
	{"ChannelLeave",      GTK_STOCK_QUIT,           N_("_Leave"),           "", NULL, NULL},
	{"ChannelClose",      GTK_STOCK_CLOSE,          N_("_Close"),           "", NULL, NULL},
	{"ChannelJoin",       GTK_STOCK_JUMP_TO,        N_("_Join"),            "", NULL, NULL},
	{"ChannelFind",       GTK_STOCK_FIND,           N_("_Find..."),         "", NULL, NULL},
	{"ChannelBans",       GTK_STOCK_DIALOG_WARNING, N_("_Bans..."),         "", NULL, NULL},

	// Dialog context menu
	{"DialogSave",        GTK_STOCK_SAVE,           N_("_Save Transcript"), "", NULL, NULL},
	{"DialogClose",       GTK_STOCK_CLOSE,          N_("_Close"),           "", NULL, NULL},
	{"DialogFind",        GTK_STOCK_FIND,           N_("_Find..."),         "", NULL, NULL}
};

static GtkToggleActionEntry toggle_action_entries[] = {
	// Server context menu
	{"ServerAutoConnect", NULL, N_("_Auto-connect on startup"), "", NULL, NULL, FALSE},

	// Channel context menu
	{"ChannelAutoJoin",   NULL, N_("_Auto-join on connect"),    "", NULL, NULL, FALSE}
};


/***** NavTree *****/
static void navigation_tree_init       (NavTree *navtree);
static void navigation_tree_class_init (NavTreeClass *klass);
static void navigation_tree_dispose    (GObject *object);
static void navigation_tree_finalize   (GObject *object);
static gint tree_iter_sort_func_nocase (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data);


#define NAVTREE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), NAVTREE_TYPE, NavTreePriv))
struct _NavTreePriv
{
	GtkActionGroup *action_group;
};

enum
{
	COLUMN_NAME,
	COLUMN_SESSION,
	COLUMN_ICON,
	COLUMN_STATUS,
	COLUMN_COLOR,
	COLUMN_CONNECTED,
	N_COLUMNS
};

/********** NavModel **********/

static void     navigation_model_init       (NavModel * navmodel);
static void     navigation_model_class_init (NavModelClass * klass);
static void     navigation_model_dispose    (GObject * object);
static void     navigation_model_finalize   (GObject * object);

static gboolean find_server  (NavModel *model, server *server, GtkTreeIter *iter);
static gboolean find_session (NavModel *model, session *sess, GtkTreeIter *iter, GtkTreeIter *parent);

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

	navtree->priv->action_group = gtk_action_group_new ("NavigationContext");
	gtk_action_group_set_translation_domain (navtree->priv->action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (navtree->priv->action_group, action_entries, G_N_ELEMENTS (action_entries), NULL);
	gtk_action_group_add_toggle_actions (navtree->priv->action_group, toggle_action_entries, G_N_ELEMENTS (toggle_action_entries), NULL);
	gtk_ui_manager_insert_action_group (gui.manager, navtree->priv->action_group, -1);
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
	NavTree *tree = NAVTREE (g_object_new (navigation_tree_get_type (), NULL));
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree), GTK_TREE_MODEL (model));
	return tree;
}

void
navigation_tree_select_session (NavTree *tree, session *sess)
{
	// FIXME: implement
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
	GtkTreeIter   iter;
	gtk_tree_store_append (GTK_TREE_STORE (model), &iter, NULL);

	/* Set default values */
	gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
	                    COLUMN_NAME,      _("<none>"),
	                    COLUMN_SESSION,   sess,
	                    COLUMN_ICON,      NULL,
	                    COLUMN_STATUS,    0,
	                    COLUMN_COLOR,     NULL,
	                    COLUMN_CONNECTED, FALSE,
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
