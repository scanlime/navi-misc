/*
 * navigation_tree.c - functions to create and maintain the navigation tree
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
#include "textgui.h"
#include "userlist.h"
#include "pixmaps.h"
#include "palette.h"
#include "channel_list.h"
#include "main_window.h"

void navigation_selection_changed(GtkTreeSelection *selection, gpointer data);
gboolean navigation_click(GtkWidget *treeview, GdkEventButton *event, gpointer data);
void navigation_context(GtkWidget *treeview, session *selected);
void server_context(GtkWidget *treeview, session *selected);
void channel_context(GtkWidget *treeview, session *selected);
void dialog_context(GtkWidget *treeview, session *selected);

void initialize_navigation_tree() {
	GtkWidget *navigation_view;
	GtkTreeStore *store;
	GtkTreeModel *model;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;

	navigation_view = glade_xml_get_widget(gui.xml, "server channel list");

	store = gtk_tree_store_new(5, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_INT, GDK_TYPE_COLOR);
	model = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(model), 1, GTK_SORT_ASCENDING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(navigation_view), GTK_TREE_MODEL(model));

	column = gtk_tree_view_column_new();
	icon_renderer = gtk_cell_renderer_pixbuf_new();
	text_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, icon_renderer, FALSE);
	gtk_tree_view_column_set_attributes(column, icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_column_pack_start(column, text_renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, text_renderer, "text", 1, "foreground-gdk", 4, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(navigation_view), column);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(navigation_view));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(navigation_selection_changed), NULL);
	g_signal_connect(G_OBJECT(navigation_view), "button_press_event", G_CALLBACK(navigation_click), NULL);
}

void navigation_tree_create_new_network_entry(struct session *sess) {
	GtkTreeStore *store;
	GtkTreeModel *model;
	GtkWidget *treeview;
	GtkTreeIter iter;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	store = GTK_TREE_STORE(gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model)));

	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 1, "<none>", 2, sess, 3, 0, 4, NULL, -1);
}

static gboolean navigation_tree_create_new_channel_entry_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, struct session *data) {
	struct session *s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s->type == SESS_SERVER && s->server == data->server) {
		GtkTreeIter child;
		GtkWidget *treeview;

		treeview = glade_xml_get_widget(gui.xml, "server channel list");

		gtk_tree_store_append(GTK_TREE_STORE(model), &child, iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child, 1, "<none>", 2, data, 3, 0, 4, NULL, -1);
		/* make sure the tree expands to show the new channel */
		gtk_tree_view_expand_row(GTK_TREE_VIEW(treeview), path, TRUE);
		return TRUE;
	}
	return FALSE;
}

void navigation_tree_create_new_channel_entry(struct session *sess) {
	GtkTreeModel *store, *model;
	GtkWidget *treeview;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	store = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));

	gtk_tree_model_foreach(store, (GtkTreeModelForeachFunc) navigation_tree_create_new_channel_entry_iterate, (gpointer) sess);
}

static gboolean navigation_tree_remove_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, struct session *data) {
	struct session *s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s == data) {
		gtk_tree_store_remove(GTK_TREE_STORE(model), iter);
		if(s == gui.current_session) {
			GtkWidget *treeview;
			GtkTreeSelection *select;

			treeview = glade_xml_get_widget(gui.xml, "server channel list");
			select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
			if(!gtk_tree_path_up(path)) {
				path = gtk_tree_path_new_first();
			}
			gtk_tree_selection_select_path(select, path);
		}
		return TRUE;
	}
	return FALSE;
}

void navigation_tree_remove(struct session *sess) {
	GtkTreeModel *store, *model;
	GtkWidget *treeview;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	store = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));

	gtk_tree_model_foreach(store, (GtkTreeModelForeachFunc) navigation_tree_remove_iterate, (gpointer) sess);
}

static gboolean navigation_tree_set_channel_name_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {
	gpointer s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s == data) {
		struct session *sess = s;
		gtk_tree_store_set(GTK_TREE_STORE(model), iter, 1, (sess->channel), 4, NULL, -1);
		return TRUE;
	}
	return FALSE;
}

void navigation_tree_set_channel_name(struct session *sess) {
	GtkTreeModel *store, *model;
	GtkWidget *treeview;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	store = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));

	gtk_tree_model_foreach(store, navigation_tree_set_channel_name_iterate, sess);
}

void navigation_selection_changed(GtkTreeSelection *selection, gpointer data) {
	GtkTreeIter iter, newiter;
	GtkTreeModel *model, *store;
	gpointer *s;
	session *sess;
	session_gui *tgui;

	if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
		GtkWidget *topic, *entry;

		/* back up existing entry */
		tgui = gui.current_session->gui;
		g_free(tgui->entry);
		entry = glade_xml_get_widget(gui.xml, "text entry");
		tgui->entry = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));

		gtk_tree_model_get(model, &iter, 2, &s, -1);
		sess = (session *) s;
		tgui = (session_gui *) sess->gui;
		gtk_xtext_buffer_show(gui.xtext, tgui->buffer, TRUE);
		topic = glade_xml_get_widget(gui.xml, "topic entry");
		gtk_entry_set_text(GTK_ENTRY(topic), tgui->topic);
		entry = glade_xml_get_widget(gui.xml, "text entry");
		gtk_entry_set_text(GTK_ENTRY(entry), tgui->entry);
		gui.current_session = sess;
		userlist_display(tgui);
		set_nickname(sess->server, NULL);
		rename_main_window(sess->server->networkname, sess->channel);
		/* remove any icon that exists */
		store = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));
		gtk_tree_model_sort_convert_iter_to_child_iter(GTK_TREE_MODEL_SORT(model), &newiter, &iter);
		gtk_tree_store_set(GTK_TREE_STORE(store), &newiter, 0, NULL, 3, 0, -1);
	}
}

static gboolean navigation_tree_set_hilight_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {
	gpointer s;
	gint e;
	gtk_tree_model_get(model, iter, 2, &s, 3, &e, -1);
	if(s == data) {
		struct session *sess = s;
		if(sess->nick_said) {
			gtk_tree_store_set(GTK_TREE_STORE(model), iter, 0, pix_nicksaid, 3, 3, -1);
			return TRUE;
		}
		if(sess->msg_said && e < 2) {
			gtk_tree_store_set(GTK_TREE_STORE(model), iter, 0, pix_msgsaid, 3, 2, -1);
			return TRUE;
		}
		if(sess->new_data && e < 1) {
			gtk_tree_store_set(GTK_TREE_STORE(model), iter, 0, pix_newdata, 3, 1, -1);
			return TRUE;
		}
	}
	return FALSE;
}

void navigation_tree_set_hilight(struct session *sess) {
	GtkTreeModel *store, *model;
	GtkWidget *treeview;

	if(sess == gui.current_session)
		return;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	store = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));

	gtk_tree_model_foreach(store, navigation_tree_set_hilight_iterate, (gpointer) sess);
}

static gboolean navigation_tree_set_disconn_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {
	gpointer s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s == data) {
		gtk_tree_store_set(GTK_TREE_STORE(model), iter, 4, &colors[23], -1);
		return TRUE;
	}
	return FALSE;
}

void navigation_tree_set_disconn(struct session *sess) {
	GtkTreeModel *store, *model;
	GtkWidget *treeview;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	store = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));

	gtk_tree_model_foreach(store, navigation_tree_set_disconn_iterate, (gpointer) sess);
}

static session *navigation_get_selected() {
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		return s;
	}
	return NULL;
}

gboolean navigation_click(GtkWidget *treeview, GdkEventButton *event, gpointer data) {
	GtkTreePath *path;
	GtkTreeSelection *select;
	if(!event)
		return FALSE;
	if(event->button == 3) {
		if(gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), event->x, event->y, &path, 0, 0, 0)) {
			select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
			gtk_tree_selection_unselect_all(select);
			gtk_tree_selection_select_path(select, path);
			gtk_tree_path_free(path);
		}
		session *s = navigation_get_selected();
		if(s != NULL)
			navigation_context(treeview, s);
		return TRUE;
	}
	return FALSE;
}

void navigation_context(GtkWidget *treeview, session *selected) {
	switch(selected->type) {
	case SESS_SERVER:	server_context(treeview, selected); return;
	case SESS_CHANNEL:	channel_context(treeview, selected); return;
	case SESS_DIALOG:	dialog_context(treeview, selected); return;
	}
}

static void clear_dialog(gpointer data, guint action, GtkWidget *widget) {
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

static void disconnect_server(gpointer data, guint action, GtkWidget *widget) {
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

static void show_channel_list(gpointer data, guint action, GtkWidget *widget) {
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

void server_context(GtkWidget *treeview, session *selected) {
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

static void leave_dialog(gpointer data, guint action, GtkWidget *widget) {
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

static void close_dialog(gpointer data, guint action, GtkWidget *widget) {
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

void channel_context(GtkWidget *treeview, session *selected) {
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

void dialog_context(GtkWidget *treeview, session *selected) {
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
