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

#include "navtree.h"
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
    navigation_tree_type = g_type_register_static(G_TYPE_OBJECT, "NavTree", &navigation_tree_info, 0);
  }

  return navigation_tree_type;
}

static void
navigation_tree_init (NavTree *navtree)
{
  GtkCellRenderer *icon_renderer, *text_renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection *select;

	navtree->current_path = gtk_tree_path_new();
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
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		clear_buffer(s);
	}
}

static void
disconnect_server(gpointer data, guint action, GtkWidget *widget)
{
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		s->server->disconnect(s, TRUE, -1);
	}
}

static void
show_channel_list(gpointer data, guint action, GtkWidget *widget)
{
	g_print("channel list!\n");
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
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
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model, *store;
	GtkTreeIter iter, newiter;
	session *s;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	select= gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
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
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		if(s->type == SESS_CHANNEL) {
			s->server->p_part(s->server, s->channel, "ex-chat");
			/* FIXME: part reason */
		}
		navigation_tree_remove(s);
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
	GtkTreeView *treeview, *navigation;
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
		navigation = GTK_TREE_VIEW(glade_xml_get_widget(gui.xml, "server channel list"));
		gtk_tree_path_free(NAVTREE(navigation)->current_path);
		NAVTREE(navigation)->current_path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), &iter);
	}
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

/* Channel/server selection functions. */

/***** NavModel *****/

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
