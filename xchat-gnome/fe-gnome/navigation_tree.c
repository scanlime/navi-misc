/*
 * navtree.c - functions to create and maintain the navigation tree
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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

#include <gconf/gconf-client.h>
#include "navigation_tree.h"
#include "userlist_gui.h"
#include "userlist.h"
#include "textgui.h"
#include "pixmaps.h"
#include "palette.h"
#include "channel_list.h"
#include "main_window.h"
#include "topiclabel.h"
#include "../common/servlist.h"

/***** NavTree *****/
static void navigation_tree_init         (NavTree *navtree);
static void navigation_tree_class_init   (NavTreeClass *klass);
static void navigation_tree_dispose      (GObject *object);
static void navigation_tree_finalize     (GObject *object);
/* Context menus. */
static void navigation_context           (GtkWidget *treeview, session *selected);
static void server_context               (GtkWidget *treeview, session *selected);
static void channel_context              (GtkWidget *treeview, session *selected);
static void dialog_context               (GtkWidget *treeview, session *selected);
/* Callbacks. */
static gboolean click                    (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static gboolean declick                  (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static void navigation_selection_changed (GtkTreeSelection *treeselection, gpointer user_data);
static void row_expanded                 (GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data);

GType
navigation_tree_get_type (void)
{
  static GType navigation_tree_type = 0;

	/* If we haven't registered the type yet. */
  if (!navigation_tree_type) {
    static const GTypeInfo navigation_tree_info =
    {
      sizeof (NavTreeClass),
      NULL, /* base init. */
      NULL, /* base finalize. */
      (GClassInitFunc) navigation_tree_class_init,
      NULL, /* class finalize. */
      NULL, /* class data. */
      sizeof (NavTree),
      0,    /* n_preallocs. */
      (GInstanceInitFunc) navigation_tree_init,
    };

		/* Register the type. */
    navigation_tree_type = g_type_register_static(GTK_TYPE_TREE_VIEW, "NavTree", &navigation_tree_info, 0);
  }

  return navigation_tree_type;
}

static void
navigation_tree_init (NavTree *navtree)
{
  GtkCellRenderer *icon_renderer, *text_renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection *select;

  g_object_set((gpointer)navtree, "headers-visible", FALSE, NULL);
	navtree->current_path = NULL;
	navtree->model = NULL;
  navtree->selection_changed_id = 0;

	/* This sets up all our columns. */
  column = gtk_tree_view_column_new();
  icon_renderer = gtk_cell_renderer_pixbuf_new();
  text_renderer = gtk_cell_renderer_text_new();
	/* Icon columns. */
  gtk_tree_view_column_pack_start(column, icon_renderer, FALSE);
  gtk_tree_view_column_set_attributes(column, icon_renderer, "pixbuf", 0, NULL);
	/* text columns. */
  gtk_tree_view_column_pack_start(column, text_renderer, TRUE);
  gtk_tree_view_column_set_attributes(column, text_renderer, "text", 1, "foreground-gdk", 4, NULL);
	/* Add the column to the TreeView. */
  gtk_tree_view_append_column(GTK_TREE_VIEW(navtree),column);

	/* Set our selction mode. */
  select = gtk_tree_view_get_selection(GTK_TREE_VIEW(navtree));
  gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	/* FIXME: Should insert navtree into the window here when we replace the existing
	 * navigation tree stuff with this.
	 */

	/* Connect the callbacks. */
  navtree->selection_changed_id = g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(navigation_selection_changed), NULL);
	g_signal_connect(G_OBJECT(navtree), "row-expanded", G_CALLBACK(row_expanded), NULL);
  g_signal_connect(G_OBJECT(navtree), "button_press_event", G_CALLBACK(click), NULL);
  g_signal_connect(G_OBJECT(navtree), "button_release_event", G_CALLBACK(declick), NULL);
}

static void
navigation_tree_class_init (NavTreeClass *klass)
{
	GObjectClass *object_class = (GObjectClass*) klass;

	object_class->dispose = navigation_tree_dispose;
	object_class->finalize = navigation_tree_finalize;
}

static void
navigation_tree_dispose (GObject *object)
{
	NavTree *navtree = (NavTree*) object;
	if (navtree->model) {
	  g_object_unref((gpointer)navtree->model);
		navtree->model = NULL;
	}
}

static void
navigation_tree_finalize (GObject *object)
{
	NavTree *navtree = (NavTree*) object;
	gtk_tree_path_free(navtree->current_path);
}

/* New NavTree. */
NavTree*
navigation_tree_new (NavModel *model)
{
	NavTree *new_tree;

	/* Create the new NavTree. */
	new_tree = NAVTREE(g_object_new(navigation_tree_get_type(), NULL));

	/* Assign a NavModel to the NavTree. */
	new_tree->model = model;
  gtk_tree_view_set_model(GTK_TREE_VIEW(new_tree), new_tree->model->sorted);

	return new_tree;
}

/* Add/remove server/channel functions. */
void
navigation_tree_create_new_network_entry (NavTree *navtree, struct session *sess)
{
  GtkTreeIter *iter;

	navigation_model_add_new_network(navtree->model, sess);

  /* Because we added a network it is likely that the path to the current session has changed
   * so we update it.
   */
  iter = navigation_model_get_sorted_iter(navtree->model, gui.current_session);
  if (iter) {
    navtree->current_path = gtk_tree_model_get_path(GTK_TREE_MODEL(navtree->model->sorted),iter);
    gtk_tree_iter_free(iter);
  }

  navigation_tree_select_session(navtree, sess);
}

void
navigation_tree_create_new_channel_entry (NavTree *navtree, struct session *sess)
{
  GtkTreeIter *iter;

	navigation_model_add_new_channel(navtree->model, sess);

  /* Because we're adding a new channel it's possible that the path to the current
   * session has changed, so we'll update it.
   * XXX: We could probably add some kind of test to only update current_path
   *      if it's truly necessary (e.g. the new channel is on the same network as
   *      the old one), but do we need to bother?
   */
  iter = navigation_model_get_sorted_iter(navtree->model, gui.current_session);
  if (iter) {
    navtree->current_path = gtk_tree_model_get_path(GTK_TREE_MODEL(navtree->model->sorted), iter);
    gtk_tree_iter_free(iter);
  }

	navigation_tree_select_session(navtree, sess);

}

void
navigation_tree_remove (NavTree *navtree, struct session *sess)
{
	GtkTreePath *path = gtk_tree_path_copy(navtree->current_path);
	GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(navtree));

	if (!gtk_tree_path_prev(path)) {
	  if (!gtk_tree_path_up(path)) {
			gtk_tree_path_free(path);
			path = gtk_tree_path_new_from_string("0");
		}
	}

  /* Change the selection before we remove the session so that things can be
   * ref'ed and unref'ed properly without too much silliness.
   */
	gtk_tree_selection_select_path(select, path);
	navigation_model_remove (navtree->model, sess);

	gtk_tree_path_free(path);
}
/* Channel/server selection functions. */
void
navigation_tree_select_nth_channel (NavTree *navtree, gint chan_num)
{
	GtkTreeView *view;
	GtkTreeModel *model;
	GtkTreeIter server;
	GtkTreeSelection *selection;
	GtkTreePath *path;
	gint kids;

	view = GTK_TREE_VIEW (navtree);
	selection = gtk_tree_view_get_selection (view);
	model = gtk_tree_view_get_model (view);

	/* Make sure we get the an iter in the tree. */
	if (model != NULL && gtk_tree_model_get_iter_first (model, &server))
	{
		/* Loop until we run out of channels or until we find the one
		 * we're looking for.
		 */
		do
		{
			/* Get path to current server. */
			path = gtk_tree_model_get_path (model, &server);

			/* Only count the channels in the server if the list is expanded. */
			if (gtk_tree_view_row_expanded (view, path))
			{
				/* Get the number of channels on the current server starting
			 	 * with the first.
			 	 */
				kids = gtk_tree_model_iter_n_children (model, &server);
				/* If the server has enough channels to contain the one we're looking
			 	 * for select it and return.
			 	 */
				if (chan_num < kids)
				{
					GtkTreeIter new_iter;

					gtk_tree_model_iter_nth_child (model, &new_iter, &server, chan_num);
					gtk_tree_selection_select_iter (selection, &new_iter);

					return;
				}
				/* If our number wants a channel out of the range of this server
			 	 * subtract the number of channels in the current server so that
			 	 * when we find the server that contains the channel we want chan_num
			 	 * will be the channel's position in the list.
			 	 */
				chan_num -= kids;
			}
		/* Move to the next channel, if possible. */
		} while (gtk_tree_model_iter_next (model, &server));
	}
}

void
navigation_tree_select_session (NavTree *navtree, struct session *sess)
{
	GtkTreeIter *iter = navigation_model_get_sorted_iter (navtree->model, sess);

	if (iter) {
		GtkTreePath *path;
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

		/* Make sure that if we're selecting a channel it's server is expanded. */
		path = gtk_tree_model_get_path (navtree->model->sorted, iter);
		if (gtk_tree_path_get_depth (path) > 1)
		{
			gtk_tree_path_up (path);
			gtk_tree_view_expand_row (GTK_TREE_VIEW (navtree), path, TRUE);
		}

		gtk_tree_selection_select_iter (selection, iter);
		gtk_tree_path_free (path);
	}
	gtk_tree_iter_free (iter);
}

void
navigation_tree_select_next_channel (NavTree *navtree, gboolean wrap)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreePath *path;

	/* Get the GtkSelection from the GtkTreeView. */
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	/* If nothing is currently selected... */
	if (!gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		model = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
		/* If we have a current_path, use that. */
		if (navtree->current_path)
		{
			gtk_tree_model_get_iter (model, &iter, navtree->current_path);
		/* Otherwise return. */
		}
		else
		{
			/*gtk_tree_model_get_iter_first(model, &iter);
			gtk_tree_selection_select_iter(selection, &iter);*/
			return;
		}
	}

	/* Get a path to the iter. */
	path = gtk_tree_model_get_path (model, &iter);

	/* Get the iter from our path because it became invalid. */
	gtk_tree_model_get_iter (model, &iter, path);

	/* If we're on a server... */
	if (gtk_tree_path_get_depth (path) == 1)
	{
		/* If we have a child, move to the first one, select it and return. */
		if (gtk_tree_model_iter_has_child (model, &iter))
		{
		  /* Expand just in case. */
		  gtk_tree_view_expand_row (GTK_TREE_VIEW (navtree), path, TRUE);
			gtk_tree_path_down (path);
			gtk_tree_model_get_iter (model, &iter, path);
			gtk_tree_selection_select_iter (selection, &iter);
		}
		return;
	}

	/* Try to move forward, otherwise select the first channel on this network. */
	if (!gtk_tree_model_iter_next (model, &iter))
	{
		if (wrap) {
			  gtk_tree_path_up (path);
			  gtk_tree_path_down (path);
			  gtk_tree_model_get_iter (model, &iter, path);
		}
		else
		{
			GtkTreeIter parent;
			gtk_tree_path_up (path);
			gtk_tree_model_get_iter (model, &parent, path);

			if (!gtk_tree_model_iter_next (model, &parent))
			        return;

			if (!gtk_tree_model_iter_children (model, &iter, &parent))
				return;
		}
	}

	/* Select the channel. */
	gtk_tree_selection_select_iter (selection, &iter);
}

void
navigation_tree_select_prev_channel (NavTree *navtree, gboolean wrap)
{
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreePath *path;
	GtkTreeIter iter;

	/* Get the GtkTreeSelection. */
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	/* Try to get the currently selected item. */
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		path = gtk_tree_model_get_path (model, &iter);
	}
	else
	{
		/* If nothing is selected... */
		model = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
		/* If we have a current path stored set path to that. */
		if (navtree->current_path)
			path = gtk_tree_path_copy (navtree->current_path);
		else
			/* Otherwise make path point to the root of the tree. */
			path = gtk_tree_path_new_from_string ("0");
	}

	/* If the path is a server... */
	if (gtk_tree_path_get_depth (path) <= 1)
	{
		/* Expand it, just in case. */
		gtk_tree_view_expand_row (GTK_TREE_VIEW (navtree), path, TRUE);

		/* If it has children move it down. */
		if (gtk_tree_model_iter_has_child (model, &iter))
		{
			gtk_tree_path_down (path);
		}
		else
		{
			/* If there are no children then we can't go to the previous channel on this server
			 * so we make path NULL.
			 */
			gtk_tree_path_free (path);
			path = NULL; /* FIXME: extraneous? */
		}
	}
	else
	{
		/* If the path isn't a server expand it's parent, just in case. */
		GtkTreePath *parent;
		parent = gtk_tree_path_copy (path);
		gtk_tree_path_up (parent);
		gtk_tree_view_expand_row (GTK_TREE_VIEW (navtree), parent, TRUE);
	}

	/* As long as path isn't NULL move it back one. If it's the first channel in the list
	 * wrap around to the last one.
	 */
	if (path)
	{
		if (!gtk_tree_path_prev (path))
		{
			if (wrap)
			{
				GtkTreeIter current, previous;
				gtk_tree_model_get_iter (model, &current, path);
				previous = current;

				/* After this loop previous is an iter for the last channel in the list. */
				while (gtk_tree_model_iter_next (model, &current))
					previous = current;
				gtk_tree_path_free (path);
				path = gtk_tree_model_get_path (model, &previous);
			}
			else
			{
				GtkTreeIter temp;
				gtk_tree_path_up (path);
				if (!gtk_tree_path_prev (path))
					return;

				gtk_tree_model_get_iter (model, &iter, path);
				gtk_tree_path_append_index (path, gtk_tree_model_iter_n_children (model, &iter) - 1);
			}
		}
		/* Select the new path. */
		gtk_tree_selection_select_path (selection, path);
	}
}

void
navigation_tree_select_next_network (NavTree *navtree)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreePath *path;
	GtkTreeSelection *selection;

	/* Get the GtkTreeSelection. */
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	/* Try to get the current selection. */
	if (!gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		model = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
		/* If nothing is selected and we have a current_path set the iter to that path. */
		if (navtree->current_path)
		{
			path = gtk_tree_path_copy (navtree->current_path);
			gtk_tree_model_get_iter (model, &iter, path);
		}
		/* If we have no current_path and nothing selected select the first server and return. */
		else
		{
			gtk_tree_model_get_iter_first (model, &iter);
			gtk_tree_selection_select_iter (selection, &iter);
			return;
		}
	}
	else
	{
		/* If we have something selected we'll need to set path to the iter. */
		path = gtk_tree_model_get_path (model, &iter);
	}

	if (gtk_tree_path_get_depth (path) > 1)
	{
		/* If our path is at a depth greater than one it's not a server, make it one. */
		GtkTreeIter parent;
		gtk_tree_model_iter_parent (model, &parent, &iter);
		iter = parent;
	}
	if (!gtk_tree_model_iter_next (model, &iter))
	{
		/* If we can't move to the next we need to move back to the root. Move iter to the root. */
		GtkTreeIter root;
		gtk_tree_model_get_iter_first (model, &root);
		iter = root;
	}

	/* Select the iter. */
	gtk_tree_selection_select_iter (selection, &iter);
}

void
navigation_tree_select_prev_network (NavTree *navtree)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreePath *path;

	/* Get our tree view and selection. */
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	if (!gtk_tree_selection_get_selected (selection, &model, &iter)) {
		/* If there is nothing selected in the GtkTreeSelection... */
		/* Get the model. */
		model = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
		/* If there's a current_path set path to that. */
		if (navtree->current_path)
		{
			path = gtk_tree_path_copy (navtree->current_path);
		}
		/* Otherwise set path to the root. */
		else
			path = gtk_tree_path_new_from_string ("0");
	}
	else
	{
		/* If there is a selection set path to that point. */
		path = gtk_tree_model_get_path (model, &iter);
	}

	/* If the path isn't a server move it up one. */
	if (gtk_tree_path_get_depth (path) > 1)
		gtk_tree_path_up (path);

	/* If we can't move it back... */
	if (!gtk_tree_path_prev (path))
	{
		/* Find the last entry in the server. */
		GtkTreeIter current, previous;
		gtk_tree_model_get_iter_first (model, &current);
		previous = current;
		while (gtk_tree_model_iter_next (model, &current))
			previous = current;
		/* Free the old path and set it to the iter that points to the last entry. */
		gtk_tree_path_free (path);
		path = gtk_tree_model_get_path (model, &previous);
	}

	/* Select the path in our GtkTreeSelection. */
	gtk_tree_selection_select_path (selection, path);
}

/* Misc. Functions. */
static gboolean
navigation_tree_set_channel_name_iterate (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
/* Iterator function for set_channel_name. */
{
	gpointer s;
	gtk_tree_model_get (model, iter, 2, &s, -1);
	if (s == data)
	{
		struct session *sess = s;
		gtk_tree_store_set (GTK_TREE_STORE (model), iter, 1, (sess->channel), 4, NULL, -1);
		return TRUE;
	}
	return FALSE;
}

void
navigation_tree_set_channel_name (NavTree *navtree, struct session *sess)
{
	GtkTreeModel *store, *model;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
	store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));

	gtk_tree_model_foreach (store, navigation_tree_set_channel_name_iterate, sess);
}

/***** Context Menus *****/
static void
navigation_context (GtkWidget *treeview, session *selected)
{
	switch (selected->type)
	{
		case SESS_SERVER:	server_context (treeview, selected); return;
		case SESS_CHANNEL:	channel_context (treeview, selected); return;
		case SESS_DIALOG:	dialog_context (treeview, selected); return;
	}
}

/*** Server Context Menu ***/
static void
clear_dialog (gpointer data, guint action, GtkWidget *widget)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = GTK_TREE_VIEW (gui.server_tree);
	select = gtk_tree_view_get_selection (treeview);
	if (gtk_tree_selection_get_selected (select, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, 2, &s, -1);
		clear_buffer (s);
	}
}

static void
disconnect_server (gpointer data, guint action, GtkWidget *widget)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = GTK_TREE_VIEW (gui.server_tree);
	select = gtk_tree_view_get_selection (treeview);
	if (gtk_tree_selection_get_selected (select, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, 2, &s, -1);
		s->server->disconnect (s, TRUE, -1);
	}
}

static void
show_channel_list (gpointer data, guint action, GtkWidget *widget)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = GTK_TREE_VIEW (gui.server_tree);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW(treeview));
	if (gtk_tree_selection_get_selected (select, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, 2, &s, -1);
		create_channel_list (s);
	}
}

static void
server_context (GtkWidget *treeview, session *selected)
{
	static GnomeUIInfo server_context[] = {
		GNOMEUIINFO_ITEM_STOCK("_Information", NULL, NULL, GTK_STOCK_DIALOG_INFO),
		GNOMEUIINFO_SEPARATOR,
		GNOMEUIINFO_ITEM_STOCK("_Reconnect", NULL, NULL, GTK_STOCK_REFRESH),
		GNOMEUIINFO_ITEM_STOCK("_Disconnect", disconnect_server, NULL, GTK_STOCK_STOP),
		GNOMEUIINFO_SEPARATOR,
		GNOMEUIINFO_ITEM_STOCK("_Channels", show_channel_list, NULL, GNOME_STOCK_TEXT_BULLETED_LIST)
	};
	GtkWidget *menu;

	menu = gnome_popup_menu_new (server_context);
	gnome_popup_menu_do_popup (menu, NULL, NULL, NULL, NULL, treeview);
}

/*** Channel Context Menu ***/
static void
leave_dialog (gpointer data, guint action, GtkWidget *widget)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model, *store;
	GtkTreeIter iter, newiter;
	session *s;

	treeview = GTK_TREE_VIEW (gui.server_tree);
	select= gtk_tree_view_get_selection (treeview);
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 2, &s, -1);
		store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));
		if (s->type == SESS_CHANNEL)
		{
			GConfClient *client;
			gchar *text;

			client = gconf_client_get_default ();
			text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
			s->server->p_part(s->server, s->channel, text);
			g_free (text);
		}
		gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model), &newiter, &iter);
		gtk_tree_store_set (GTK_TREE_STORE (store), &newiter, 4, &colors[40], -1);
	}
}

static void
close_dialog (gpointer data, guint action, GtkWidget *widget)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = GTK_TREE_VIEW (gui.server_tree);
	select = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (select, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, 2, &s, -1);

		if (s->type == SESS_CHANNEL)
		{
			GConfClient *client;
			gchar *text;

			client = gconf_client_get_default ();
			text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
			s->server->p_part(s->server, s->channel, text);
			g_free (text);
		}

		fe_close_window (s);
		text_gui_remove_text_buffer (s);
	}
}

static void
channel_context (GtkWidget *treeview, session *selected)
{
	static GnomeUIInfo channel_context[] = {
		GNOMEUIINFO_MENU_SAVE_ITEM(NULL, NULL),
		GNOMEUIINFO_MENU_SAVE_AS_ITEM(NULL, NULL),
		GNOMEUIINFO_SEPARATOR,
		GNOMEUIINFO_ITEM_STOCK("_Leave", leave_dialog, NULL, GTK_STOCK_QUIT),
		GNOMEUIINFO_MENU_CLOSE_ITEM(close_dialog, NULL),
		GNOMEUIINFO_SEPARATOR,
		GNOMEUIINFO_MENU_FIND_ITEM(NULL, NULL),
		GNOMEUIINFO_MENU_FIND_AGAIN_ITEM(NULL, NULL),
		GNOMEUIINFO_MENU_CLEAR_ITEM(clear_dialog, NULL),
		GNOMEUIINFO_SEPARATOR,
		GNOMEUIINFO_ITEM_STOCK("_Bans", NULL, NULL, GTK_STOCK_DIALOG_WARNING)
	};
	GtkWidget *menu;

	menu = gnome_popup_menu_new (channel_context);
	gnome_popup_menu_do_popup (menu, NULL, NULL, NULL, NULL, treeview);
}

/*** Dialog Context Menu ***/
static void
dialog_context (GtkWidget *treeview, session *selected)
{
	static GnomeUIInfo dialog_context[] = {
		GNOMEUIINFO_MENU_SAVE_ITEM(NULL, NULL),
		GNOMEUIINFO_MENU_SAVE_AS_ITEM(NULL, NULL),
		GNOMEUIINFO_SEPARATOR,
		GNOMEUIINFO_MENU_CLOSE_ITEM(close_dialog, NULL),
		GNOMEUIINFO_SEPARATOR,
		GNOMEUIINFO_MENU_FIND_ITEM(NULL, NULL),
		GNOMEUIINFO_MENU_FIND_AGAIN_ITEM(NULL, NULL),
		GNOMEUIINFO_MENU_CLEAR_ITEM(clear_dialog, NULL)
	};
	GtkWidget *menu;

	menu = gnome_popup_menu_new (dialog_context);
	gnome_popup_menu_do_popup (menu, NULL, NULL, NULL, NULL, treeview);
}

/***** Callbacks *****/
static gboolean
click (GtkWidget *treeview, GdkEventButton *event, gpointer data)
{
	GtkTreePath *path;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	struct session *s;

	if (!event)
		return FALSE;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	if (event->button == 3)
	{
		if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (treeview), event->x, event->y, &path, 0, 0, 0)) {
			gtk_tree_selection_unselect_all (select);
			gtk_tree_selection_select_path (select, path);
			gtk_tree_path_free (path);
		}

		model = GTK_TREE_MODEL (NAVTREE (treeview)->model->sorted);
		gtk_tree_model_get_iter (model, &iter, NAVTREE (treeview)->current_path);
		gtk_tree_model_get (model, &iter, 2, &s, -1);
		if (s != NULL)
			/* FIXME */
			navigation_context (treeview, s);
		return TRUE;
	}

	g_object_set (G_OBJECT (treeview), "can-focus", FALSE, NULL);
	return FALSE;
}

static gboolean
declick (GtkWidget *treeview, GdkEventButton *e, gpointer data)
{
	GtkWidget *entry;

	entry = glade_xml_get_widget (gui.xml, "text entry");
	gtk_widget_grab_focus (entry);
	gtk_editable_set_position (GTK_EDITABLE (entry), -1);
	g_object_set (G_OBJECT (treeview), "can-focus", TRUE, NULL);
	return FALSE;
}

static void
navigation_selection_changed (GtkTreeSelection *treeselection, gpointer user_data)
{
	GtkTreeIter iter, newiter;
	GtkTreeModel *model, *store;
	GtkTreeView *treeview;
	gpointer *s;
	session *sess;
	session_gui *tgui;

	treeview = GTK_TREE_VIEW (glade_xml_get_widget (gui.xml, "userlist"));

	if (gui.server_tree->current_path)
		navigation_model_path_deref (gui.server_tree->model, gui.server_tree->current_path);

	/* XXX: This sets model to be the GtkTreeModelSort used by the NavTree, it is
	 *      not a GtkTreeModel. The iter is for that ModelSort.
	 */
	if (gtk_tree_selection_get_selected (treeselection, &model, &iter) && gui.current_session) {
		GtkWidget *topic, *entry;

		/* back up existing entry */
		tgui = (session_gui *) gui.current_session->gui;
		if (tgui == NULL)
			return;
		g_free (tgui->entry);
		entry = glade_xml_get_widget (gui.xml, "text entry");
		tgui->entry = g_strdup (gtk_entry_get_text (GTK_ENTRY (entry)));

		/* Update current_path. */
		if (gui.server_tree->current_path) {
			/* FIXME: Causes crashes. */
			gtk_tree_path_free (gui.server_tree->current_path);
		}

		/* FIXME: Causes crashes. */
		gui.server_tree->current_path = gtk_tree_model_get_path (model, &iter);
		navigation_model_path_ref (gui.tree_model, gui.server_tree->current_path);
		gtk_tree_selection_get_selected (treeselection, &model, &iter);

		/* Get the session for the new selection. */
		gtk_tree_model_get (model, &iter, 2, &s, -1);
		sess = (session *) s;

		/* Clear the icons. */
		sess->nick_said = FALSE;
		sess->msg_said = FALSE;
		sess->new_data = FALSE;

		/* Set tgui to the gui of the new session. */
		tgui = (session_gui *) sess->gui;
		if (tgui == NULL)
			return;

		/* Show the xtext buffer for the session. */
		gtk_xtext_buffer_show (gui.xtext, tgui->buffer, TRUE);

		/* Set the topic. */
		topic_label_set_text (TOPIC_LABEL (gui.topic), tgui->topic);

		/* Set the text entry field to whatever is in the text entry of this session. */
		entry = glade_xml_get_widget (gui.xml, "text entry");
		gtk_entry_set_text (GTK_ENTRY (entry), tgui->entry);

		/* Make this our current session. */
		gui.current_session = sess;

		/* Change the model in the userlist to the one for this session. */
		gtk_tree_view_set_model (treeview, GTK_TREE_MODEL (userlist_get_store (u, sess)));

		/* Set our nick. */
		set_nickname (sess->server, NULL);

		/* Change the window name. */
		if (sess->server->network == NULL)
		{
			rename_main_window (NULL, sess->channel);
		}
		else
		{
			ircnet *net = sess->server->network;
			rename_main_window (net->name, sess->channel);
		}

		/* remove any icon that exists */
		store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));
		gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model), &newiter, &iter);
		gtk_tree_store_set (GTK_TREE_STORE (store), &newiter, 0, NULL, 3, 0, -1);
	}
}

static void
row_expanded (GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data)
{
	/* If we had something selected before the row was collapsed make sure it gets selected. */
	if (NAVTREE (treeview)->current_path && gtk_tree_path_is_ancestor (path, NAVTREE (treeview)->current_path)) {
		GtkTreeSelection *selection;
		selection = gtk_tree_view_get_selection (treeview);
		g_signal_handler_block ((gpointer) selection, NAVTREE (treeview)->selection_changed_id);
		gtk_tree_selection_select_path (selection, NAVTREE (treeview)->current_path);
		g_signal_handler_unblock ((gpointer) selection, NAVTREE (treeview)->selection_changed_id);
	}
}



/********** NavModel **********/

static void navigation_model_init       (NavModel *navmodel);
static void navigation_model_class_init (NavModelClass *klass);
static void navigation_model_dispose    (GObject *object);
static void navigation_model_finalize   (GObject *object);

GType
navigation_model_get_type (void)
{
	static GType navigation_model_type = 0;
	if (!navigation_model_type)
	{
		static const GTypeInfo navigation_model_info =
		{
			sizeof (NavTreeClass),
			NULL, /* base init. */
			NULL, /* base finalize. */
			(GClassInitFunc) navigation_model_class_init,
			NULL, /* class_finalize. */
			NULL, /* class_data. */
			sizeof(NavModel),
			0,    /* n_preallocs. */
			(GInstanceInitFunc) navigation_model_init,
		};

		navigation_model_type = g_type_register_static (G_TYPE_OBJECT, "NavModel", &navigation_model_info, 0);
	}

	return navigation_model_type;
}

static void
navigation_model_init (NavModel *navmodel)
{
	/* FIXME: comment this up so we know what all the columns are. */
	navmodel->store = gtk_tree_store_new (6, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_INT, GDK_TYPE_COLOR, G_TYPE_INT);
	navmodel->sorted = gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (navmodel->store));
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (navmodel->sorted), 1, GTK_SORT_ASCENDING);

	/* Set up our hash table for direct hashing. */
	navmodel->session_rows = g_hash_table_new (g_direct_hash, g_direct_equal);
}

static void
navigation_model_class_init (NavModelClass *klass)
{
	GObjectClass *object_class = (GObjectClass*) klass;
	object_class->dispose = navigation_model_dispose;
	object_class->finalize = navigation_model_finalize;
}

static void
destroy_rowref (session *sess, GtkTreeRowReference* rowref, gpointer data)
{
	gtk_tree_row_reference_free (rowref);
}

static void
navigation_model_dispose (GObject *object)
{
	NavModel *navmodel = NAVMODEL (object);
	g_object_unref ((gpointer) navmodel->store);
	g_object_unref ((gpointer) navmodel->sorted);
	g_hash_table_foreach_remove (navmodel->session_rows, (GHRFunc) destroy_rowref, NULL);
}

static void
navigation_model_finalize (GObject *object)
{
	NavModel *model = (NavModel*) object;
	g_hash_table_destroy (model->session_rows);
}

/* New NavModel. */
NavModel*
navigation_model_new ()
{
	return NAVMODEL (g_object_new (navigation_model_get_type (), NULL));
}

void
navigation_model_add_new_network (NavModel *model, struct session *sess)
{
	GtkTreeIter iter;
	GtkTreePath *path;
	GtkTreeRowReference *rowref;

	gtk_tree_store_append (model->store, &iter, NULL);
	gtk_tree_store_set (model->store, &iter, 1, "<none>", 2, sess, 3, 0, 4, NULL, 5, 0, -1);

	path = gtk_tree_model_get_path (GTK_TREE_MODEL (model->store), &iter);
	rowref = gtk_tree_row_reference_new (GTK_TREE_MODEL (model->store), path);
	g_hash_table_insert (model->session_rows, (gpointer) sess, (gpointer) rowref);
	gtk_tree_path_free (path);
}

static gboolean
navigation_model_create_new_channel_entry_iterate (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, struct session *data)
{
	struct session *s;
	gtk_tree_model_get (model, iter, 2, &s, -1);
	if (s->type == SESS_SERVER && s->server == data->server)
	{
		GtkTreeIter child;
		GtkWidget *entry;
		GtkTreeModelSort *sort;
		GtkTreeRowReference *rowref;
		GtkTreePath *path;

		sort = GTK_TREE_MODEL_SORT (gui.tree_model->sorted);

		gtk_tree_store_append (GTK_TREE_STORE (model), &child, iter);
		gtk_tree_store_set (GTK_TREE_STORE (model), &child, 1, data->channel, 2, data, 3, 0, 4, NULL, 5, 0, -1);

		path = gtk_tree_model_get_path (model, &child);
		rowref = gtk_tree_row_reference_new (model, path);
		g_hash_table_insert (gui.tree_model->session_rows, (gpointer) data, (gpointer) rowref);

		entry = glade_xml_get_widget (gui.xml, "text entry");
		gtk_widget_grab_focus (entry);

		gtk_tree_path_free (path);
		return TRUE;
	}
	return FALSE;
}

void
navigation_model_add_new_channel (NavModel *model, struct session *sess)
{
	gtk_tree_model_foreach (GTK_TREE_MODEL (model->store), (GtkTreeModelForeachFunc) navigation_model_create_new_channel_entry_iterate, (gpointer) sess);
}

static gboolean
navigation_model_remove_iterate (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, struct session *data)
{
	struct session *s;
	gtk_tree_model_get (model, iter, 2, &s, -1);
	if (s == data)
	{
		gtk_tree_store_remove (GTK_TREE_STORE (model), iter);
		return TRUE;
	}
	return FALSE;
}

void
navigation_model_remove (NavModel *model, struct session *sess)
{
	//gtk_tree_model_foreach(GTK_TREE_MODEL(model->store), (GtkTreeModelForeachFunc) navigation_model_remove_iterate, (gpointer) sess);
	GtkTreeIter *iter = navigation_model_get_unsorted_iter (model, sess);
	gtk_tree_store_remove (model->store, iter);

	gtk_tree_iter_free (iter);
}

GtkTreeIter*
navigation_model_get_sorted_iter (NavModel *model, struct session *sess)
{
	GtkTreeIter *iter = NULL;
	GtkTreePath *path, *sorted;
	GtkTreeRowReference *row;

	row = (GtkTreeRowReference*) g_hash_table_lookup (model->session_rows, (gpointer) sess);
	if (row)
	{
		iter = g_new (GtkTreeIter, 1);
		path = gtk_tree_row_reference_get_path (row);
		sorted = gtk_tree_model_sort_convert_child_path_to_path (GTK_TREE_MODEL_SORT (model->sorted), path);

		gtk_tree_model_get_iter (model->sorted, iter, sorted);

		gtk_tree_path_free (sorted);
		gtk_tree_path_free (path);
	}

	return iter;
}

GtkTreeIter*
navigation_model_get_unsorted_iter (NavModel *model, struct session *sess)
{
	GtkTreeIter *iter = g_new(GtkTreeIter, 1);
	GtkTreePath *path;

	path = gtk_tree_row_reference_get_path ((GtkTreeRowReference*) g_hash_table_lookup (model->session_rows, (gpointer) sess));
	gtk_tree_model_get_iter (GTK_TREE_MODEL (model->store), iter, path);

	gtk_tree_path_free (path);
	return iter;
}

static gboolean
navigation_model_set_disconn_iterate (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	gpointer s;
	gtk_tree_model_get (model, iter, 2, &s, -1);
	if (s == data)
	{
		gtk_tree_store_set (GTK_TREE_STORE (model), iter, 4, &colors[40], -1);
		return TRUE;
	}
	return FALSE;
}

void
navigation_model_set_disconn (NavModel *model, struct session *sess)
{
	gtk_tree_model_foreach (GTK_TREE_MODEL (model->store), navigation_model_set_disconn_iterate, (gpointer) sess);
}

static gboolean
navigation_model_set_hilight_iterate (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	gpointer s;
	gint e, ref;
	gtk_tree_model_get (model, iter, 2, &s, 3, &e, 5, &ref, -1);
	if (s == data && ref == 0)
	{
		struct session *sess = s;
		if (sess->nick_said)
		{
			gtk_tree_store_set (GTK_TREE_STORE (model), iter, 0, pix_nicksaid, 3, 3, -1);
			return TRUE;
		}
		if (sess->msg_said && e < 2)
		{
			gtk_tree_store_set (GTK_TREE_STORE (model), iter, 0, pix_msgsaid, 3, 2, -1);
			return TRUE;
		}
		if (sess->new_data && e < 1)
		{
			gtk_tree_store_set (GTK_TREE_STORE (model), iter, 0, pix_newdata, 3, 1, -1);
			return TRUE;
		}
	}
	return FALSE;
}

void
navigation_model_set_hilight (NavModel *model, struct session *sess)
{
	if (sess == gui.current_session)
	{
		sess->nick_said = FALSE;
		sess->msg_said = FALSE;
		sess->new_data = FALSE;
		return;
	}
	gtk_tree_model_foreach (GTK_TREE_MODEL (model->store), navigation_model_set_hilight_iterate, (gpointer) sess);
}

void
navigation_model_path_ref (NavModel *model, GtkTreePath *path)
{
	gint ref_count;
	GtkTreeIter iter;
	GtkTreePath *unsorted = gtk_tree_model_sort_convert_path_to_child_path (GTK_TREE_MODEL_SORT (model->sorted), path);

	gtk_tree_model_get_iter (GTK_TREE_MODEL (model->store), &iter, unsorted);
	gtk_tree_model_get (GTK_TREE_MODEL (model->store), &iter, 5, &ref_count, -1);
	gtk_tree_store_set (model->store, &iter, 5, ref_count + 1, -1);

	gtk_tree_path_free (unsorted);
}

void
navigation_model_path_deref (NavModel *model, GtkTreePath *path)
{
	gint ref_count;
	GtkTreeIter iter;
	GtkTreePath *unsorted = gtk_tree_model_sort_convert_path_to_child_path (GTK_TREE_MODEL_SORT (model->sorted), path);

	gtk_tree_model_get_iter (GTK_TREE_MODEL (model->store), &iter, unsorted);
	gtk_tree_model_get (GTK_TREE_MODEL (model->store), &iter, 5, &ref_count, -1);

	if (ref_count > 0)
		gtk_tree_store_set (model->store, &iter, 5, ref_count-1, -1);

	gtk_tree_path_free (unsorted);
}

void
navigation_model_sorted_iter_ref (NavModel *model, GtkTreeIter *iter)
{
	gint ref_count;
	GtkTreeIter unsorted;
	gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model->sorted), &unsorted, iter);
	gtk_tree_model_get (model->sorted, iter, 5, &ref_count, -1);

	gtk_tree_store_set (model->store, &unsorted, 5, ref_count+1, -1);
}

void
navigation_model_sorted_iter_unref (NavModel *model, GtkTreeIter *iter)
{
	gint ref_count;
	GtkTreeIter unsorted;

	gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model->sorted), &unsorted, iter);
	gtk_tree_model_get (model->sorted, iter, 5, &ref_count, -1);

	if (ref_count > 0)
		gtk_tree_store_set (model->store, &unsorted, 5, ref_count-1, -1);
}
