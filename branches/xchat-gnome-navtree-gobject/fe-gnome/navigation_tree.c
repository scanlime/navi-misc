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

#include "navigation_tree.h"
#include "userlist_gui.h"
#include "userlist.h"
#include "textgui.h"
#include "pixmaps.h"
#include "palette.h"
#include "channel_list.h"
#include "main_window.h"

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

	navtree->current_path = NULL;
	navtree->model = NULL;

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
  g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(navigation_selection_changed), NULL);
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
	navigation_model_add_new_network(navtree->model, sess);
	navigation_tree_select_session(navtree, sess);
}

void
navigation_tree_create_new_channel_entry (NavTree *navtree, struct session *sess)
{
	navigation_model_add_new_channel(navtree->model, sess);
	navigation_tree_select_session(navtree, sess);
}

void
navigation_tree_remove (NavTree *navtree, struct session *sess)
{
	navigation_model_remove (navtree->model, sess);
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

	view = GTK_TREE_VIEW(navtree);
	selection = gtk_tree_view_get_selection(view);
	model = gtk_tree_view_get_model(view);
	/* Make sure we get the an iter in the tree. */
	if(model != NULL && gtk_tree_model_get_iter_first(model, &server)) {
		/* Loop until we run out of channels or until we find the one
		 * we're looking for.
		 */
		do {
			/* Get path to current server. */
			path = gtk_tree_model_get_path(model, &server);

			/* Only count the channels in the server if the list is expanded. */
			if(gtk_tree_view_row_expanded(view, path)) {
				/* Get the number of channels on the current server starting
			 	 * with the first.
			 	 */
				kids = gtk_tree_model_iter_n_children(model, &server);
				/* If the server has enough channels to contain the one we're looking
			 	 * for select it and return.
			 	 */
				if(chan_num < kids) {
					GtkTreeIter new_iter;
					gtk_tree_model_iter_nth_child(model, &new_iter, &server, chan_num);
					gtk_tree_selection_select_iter(selection, &new_iter);
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
		}while (gtk_tree_model_iter_next(model, &server));
	}
}

void
navigation_tree_select_session (NavTree *navtree, struct session *sess)
{ /* FIXME: Impelement. */
}

void
navigation_tree_select_next_channel (NavTree *navtree)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;

	/* Grab our treeview and get a gtk selection. */
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(navtree));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(navtree));

	if (!navtree->current_path) {
		gtk_tree_model_get_iter_first(model, &iter);
		gtk_tree_path_free(navtree->current_path);
		navtree->current_path = gtk_tree_model_get_path(model, &iter);
	}

	gtk_tree_model_get_iter(model, &iter, navtree->current_path);
	/* If our iter has a child we know it's a server and we need
	 * to move down one level to get to a channel.
	 */
	if(gtk_tree_model_iter_has_child(model, &iter))
		gtk_tree_path_down(navtree->current_path);

	/* As long as our path depth is greater than one we know we have
	 * a channel selected, and not a server without any channels.
	 */
	else if(gtk_tree_path_get_depth(navtree->current_path) > 1) {
		GtkTreePath *path_to_bottom;
		GtkTreeIter parent;

		/* Get the path to the last node for comparison later. */
		gtk_tree_model_iter_parent(model, &parent, &iter);
		gtk_tree_model_iter_nth_child(model, &iter, &parent,
				gtk_tree_model_iter_n_children(model, &parent) - 1);
		path_to_bottom = gtk_tree_model_get_path(model, &iter);

		/* Advance the path one node. */
		gtk_tree_path_next(navtree->current_path);

		/* If our new path goes beyond the path to the last node
		 * we need to wrap around to the top.
		 */
		if(gtk_tree_path_compare(path_to_bottom, navtree->current_path) == -1) {
			gtk_tree_path_up(navtree->current_path);
			gtk_tree_path_down(navtree->current_path);
		}
	}

	/* If we don't have a valid path, return, otherwise move
	 * the selection to our new path.
	 */
	if(navtree->current_path == NULL)
		return;
	gtk_tree_selection_select_path(selection, navtree->current_path);
}

void
navigation_tree_select_prev_channel (NavTree *navtree)
{
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(navtree));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(navtree));

	if (!navtree->current_path) {
		gtk_tree_model_get_iter_first(model, &iter);
		gtk_tree_path_free(navtree->current_path);
		navtree->current_path = gtk_tree_model_get_path(model, &iter);
	}

	gtk_tree_model_get_iter(model, &iter, navtree->current_path);
	if (gtk_tree_model_iter_has_child(model, &iter))
		gtk_tree_path_down(navtree->current_path);

	if (gtk_tree_path_get_depth(navtree->current_path) > 1 &&
			!gtk_tree_path_prev(navtree->current_path)) {
		GtkTreeIter parent;
		gint children;

		/* Get the parent (server) of our current channel. */
		gtk_tree_model_iter_parent(model, &parent, &iter);

		/* Move iter to the last child of parent. */
		children = gtk_tree_model_iter_n_children(model, &parent);
		gtk_tree_model_iter_nth_child(model, &iter, &parent, children-1);

		/* Set path to the last child. */
		gtk_tree_path_free(navtree->current_path);
		navtree->current_path = gtk_tree_model_get_path(model, &iter);
	}

	if (!gtk_tree_view_row_expanded(GTK_TREE_VIEW(navtree), navtree->current_path))
		gtk_tree_view_expand_row(GTK_TREE_VIEW(navtree), navtree->current_path, TRUE);

	if (navtree->current_path == NULL)
		return;

	gtk_tree_selection_select_path(selection, navtree->current_path);
}

void
navigation_tree_select_next_network (NavTree *navtree)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;

	/* Get our tree view and selection. */
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(navtree));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(navtree));

	/* Make sure we get our iterator and model. */
	if(navtree->current_path) {
		/* Test the depth of the path to make sure we have a server selected
	 	 * (depth of 1). If we have a channel selected (depth > 1) we need
	 	 * to move up to the parent (server).
	 	 */
		if(gtk_tree_path_get_depth(navtree->current_path) > 1) {
			GtkTreeIter parent;
			gtk_tree_model_iter_parent(model, &parent, &iter);
			iter = parent;
		}

		/* Try and move the iter forward, if that fails we're probably at the end
	 	 * of the list and should wrap around to the first server.
	 	 */
		if(!gtk_tree_model_iter_next(model, &iter))
			gtk_tree_model_get_iter_first(model, &iter);

	  /* Move the selection to the new position. */
	  gtk_tree_selection_select_iter(selection, &iter);
	}
}

/* Misc. Functions. */
static gboolean
navigation_tree_set_channel_name_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	gpointer s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s == data) {
		struct session *sess = s;
		gtk_tree_store_set(GTK_TREE_STORE(model), iter, 1, (sess->channel), 4, NULL, -1);
		return TRUE;
	}
	return FALSE;
}

void
navigation_tree_set_channel_name(NavTree *navtree, struct session *sess)
{
	GtkTreeModel *store, *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(navtree));
	store = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));

	gtk_tree_model_foreach(store, navigation_tree_set_channel_name_iterate, sess);
}

void
navigation_tree_set_disconn (NavTree *navtree, struct session *sess)
{ /* FIXME: Implement. */
}

void
navigation_tree_set_hilight (NavTree *navtree, struct session *sess)
{ /* FIXME: Implement. */
}

/***** Context Menus *****/
static void
navigation_context(GtkWidget *treeview, session *selected)
{
	switch(selected->type) {
	case SESS_SERVER:	server_context(treeview, selected); return;
	case SESS_CHANNEL:	channel_context(treeview, selected); return;
	case SESS_DIALOG:	dialog_context(treeview, selected); return;
	}
}

/*** Server Context Menu ***/
static void
clear_dialog(gpointer data, guint action, GtkWidget *widget)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = GTK_TREE_VIEW(gui.server_tree);
	select = gtk_tree_view_get_selection(treeview);
	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		clear_buffer(s);
	}
}

static void
disconnect_server(gpointer data, guint action, GtkWidget *widget)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = GTK_TREE_VIEW(gui.server_tree);
	select = gtk_tree_view_get_selection(treeview);
	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		s->server->disconnect(s, TRUE, -1);
	}
}

static void
show_channel_list(gpointer data, guint action, GtkWidget *widget)
{
	g_print("channel list!\n");
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = GTK_TREE_VIEW(gui.server_tree);
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		create_channel_list(s);
	}
}

static void
server_context(GtkWidget *treeview, session *selected)
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

	menu = gnome_popup_menu_new(server_context);
	gnome_popup_menu_do_popup(menu, NULL, NULL, NULL, NULL, treeview);
}

/*** Channel Context Menu ***/
static void
leave_dialog(gpointer data, guint action, GtkWidget *widget)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model, *store;
	GtkTreeIter iter, newiter;
	session *s;

	treeview = GTK_TREE_VIEW(gui.server_tree);
	select= gtk_tree_view_get_selection(treeview);
	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		store = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));
		if(s->type == SESS_CHANNEL) {
			s->server->p_part(s->server, s->channel, "ex-chat");
			/* FIXME: part reason */
		}
		gtk_tree_model_sort_convert_iter_to_child_iter(GTK_TREE_MODEL_SORT(model), &newiter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(store), &newiter, 4, &colors[23], -1);
	}
}

static void
close_dialog(gpointer data, guint action, GtkWidget *widget)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = GTK_TREE_VIEW(gui.server_tree);
	select = gtk_tree_view_get_selection(treeview);
	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		if(s->type == SESS_CHANNEL) {
			s->server->p_part(s->server, s->channel, "ex-chat");
			/* FIXME: part reason */
		}
		/*FIXME: navigation_tree_remove(s); */
		text_gui_remove_text_buffer(s);
	}
}

static void
channel_context(GtkWidget *treeview, session *selected)
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

	menu = gnome_popup_menu_new(channel_context);
	gnome_popup_menu_do_popup(menu, NULL, NULL, NULL, NULL, treeview);
}

/*** Dialog Context Menu ***/
static void
dialog_context(GtkWidget *treeview, session *selected)
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

	menu = gnome_popup_menu_new(dialog_context);
	gnome_popup_menu_do_popup(menu, NULL, NULL, NULL, NULL, treeview);
}

/***** Callbacks *****/
static gboolean
click(GtkWidget *treeview, GdkEventButton *event, gpointer data)
{
	GtkTreePath *path;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	struct session *s;

	if(!event)
		return FALSE;

	if(event->button == 3) {
		if(gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), event->x, event->y, &path, 0, 0, 0)) {
			select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
			gtk_tree_selection_unselect_all(select);
			gtk_tree_selection_select_path(select, path);
			gtk_tree_path_free(path);
		}

		model = GTK_TREE_MODEL(NAVTREE(treeview)->model->sorted);
		gtk_tree_model_get_iter(model, &iter, NAVTREE(treeview)->current_path);
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		if(s != NULL)
			/* FIXME */
			navigation_context(treeview, s);
		return TRUE;
	}

	g_object_set(G_OBJECT(treeview), "can-focus", FALSE, NULL);
	return FALSE;
}

static gboolean declick(GtkWidget *treeview, GdkEventButton *e, gpointer data) {
	GtkWidget *entry;

	entry = glade_xml_get_widget(gui.xml, "text entry");
	gtk_widget_grab_focus(entry);
	gtk_editable_set_position(GTK_EDITABLE(entry), -1);
	g_object_set(G_OBJECT(treeview), "can-focus", TRUE, NULL);
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

	if(gtk_tree_selection_get_selected(treeselection, &model, &iter) && gui.current_session) {
		GtkWidget *topic, *entry;

		/* back up existing entry */
		tgui = (session_gui *) gui.current_session->gui;
		if(tgui == NULL)
			return;
		g_free(tgui->entry);
		entry = glade_xml_get_widget(gui.xml, "text entry");
		tgui->entry = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));

		/* Get the session for the new selection. */
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		sess = (session *) s;
		/* Clear the icons. */
		sess->nick_said = FALSE;
		sess->msg_said = FALSE;
		sess->new_data = FALSE;
		/* Set tgui to the gui of the new session. */
		tgui = (session_gui *) sess->gui;
		if(tgui == NULL)
			return;
		/* Show the xtext buffer for the session. */
		gtk_xtext_buffer_show(gui.xtext, tgui->buffer, TRUE);
		/* Set the topic. */
		topic = glade_xml_get_widget(gui.xml, "topic entry");
		gtk_entry_set_text(GTK_ENTRY(topic), tgui->topic);
		/* Set the text entry field to whatever is in the text entry of this session. */
		entry = glade_xml_get_widget(gui.xml, "text entry");
		gtk_entry_set_text(GTK_ENTRY(entry), tgui->entry);
		/* Make this our current session. */
		gui.current_session = sess;
		/* Change the model in the userlist to the one for this session. */
		gtk_tree_view_set_model (treeview, GTK_TREE_MODEL (userlist_get_store (u, sess)));
		/* Set our nick. */
		set_nickname(sess->server, NULL);
		/* Change the window name. */
		rename_main_window(sess->server->networkname, sess->channel);
		/* remove any icon that exists */
		store = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));
		gtk_tree_model_sort_convert_iter_to_child_iter(GTK_TREE_MODEL_SORT(model), &newiter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(store), &newiter, 0, NULL, 3, 0, -1);

		/* Update current_path. */
		if(gui.server_tree->current_path)
		  gtk_tree_path_free(gui.server_tree->current_path);
	  gui.server_tree->current_path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), &iter);
	}
}

/********** NavModel **********/

static void navigation_model_init       (NavModel *navmodel);
static void navigation_model_class_init (NavModelClass *klass);
static void navigation_model_dispose    (GObject *object);

GType
navigation_model_get_type (void)
{
  static GType navigation_model_type = 0;
	/* If we haven't registered our type yet. */
  if (!navigation_model_type) {
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

		/* Register the type. */
    navigation_model_type = g_type_register_static(G_TYPE_OBJECT, "NavModel", &navigation_model_info, 0);
  }

  return navigation_model_type;
}

static void
navigation_model_init (NavModel *navmodel)
{
  navmodel->store = gtk_tree_store_new(5, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_INT, GDK_TYPE_COLOR);
	navmodel->sorted = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(navmodel->store));
  gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(navmodel->sorted), 1, GTK_SORT_ASCENDING);
}

static void
navigation_model_class_init (NavModelClass *klass)
{
	GObjectClass *object_class = (GObjectClass*) klass;
	object_class->dispose = navigation_model_dispose;
}

static void
navigation_model_dispose (GObject *object)
{
  NavModel *navmodel = NAVMODEL(object);
  g_object_unref((gpointer)navmodel->store);
	g_object_unref((gpointer)navmodel->sorted);
}

/* New NavModel. */
NavModel*
navigation_model_new ()
{
	return NAVMODEL(g_object_new(navigation_model_get_type(), NULL));
}

void
navigation_model_add_new_network (NavModel *model, struct session *sess)
{
	GtkTreeIter iter;

	gtk_tree_store_append(GTK_TREE_STORE(model->store), &iter, NULL);
	gtk_tree_store_set(model->store, &iter, 1, "<none>", 2, sess, 3, 0, 4, NULL, -1);
}

static gboolean
navigation_model_create_new_channel_entry_iterate (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, struct session *data)
{
	struct session *s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s->type == SESS_SERVER && s->server == data->server) {
		GtkTreeIter child, sorted;
		GtkWidget *entry;
		GtkTreeView *treeview;
		GtkTreeModelSort *sort;

		treeview = GTK_TREE_VIEW(gui.server_tree);
		sort = GTK_TREE_MODEL_SORT(gtk_tree_view_get_model(treeview));

		gtk_tree_store_append(GTK_TREE_STORE(model), &child, iter);

		gtk_tree_store_set(GTK_TREE_STORE(model), &child, 1, data->channel, 2, data, 3, 0, 4, NULL, -1);

		gtk_tree_model_sort_convert_child_iter_to_iter(sort, &sorted, iter);

		entry = glade_xml_get_widget(gui.xml, "text entry");
		gtk_widget_grab_focus(entry);
		return TRUE;
	}
	return FALSE;
}

void
navigation_model_add_new_channel (NavModel *model, struct session *sess)
{
	gtk_tree_model_foreach(GTK_TREE_MODEL(model->store), (GtkTreeModelForeachFunc) navigation_model_create_new_channel_entry_iterate, (gpointer) sess);
}

static gboolean
navigation_model_remove_iterate (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, struct session *data)
{
	struct session *s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s == data) {
		gtk_tree_store_remove(GTK_TREE_STORE(model), iter);
		return TRUE;
	}
	return FALSE;
}

void
navigation_model_remove (NavModel *model, struct session *sess)
{
	gtk_tree_model_foreach(GTK_TREE_MODEL(model->store), (GtkTreeModelForeachFunc) navigation_model_remove_iterate, (gpointer) sess);
}
