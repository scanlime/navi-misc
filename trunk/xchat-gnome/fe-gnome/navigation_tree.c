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

void navigation_selection_changed(GtkTreeSelection *selection, gpointer data);
gboolean navigation_click(GtkWidget *treeview, GdkEventButton *event, gpointer data);
void navigation_context(GtkWidget *treeview, session *selected);
void server_context(GtkWidget *treeview, session *selected);
void channel_context(GtkWidget *treeview, session *selected);
void dialog_context(GtkWidget *treeview, session *selected);

void initialize_navigation_tree() {
	GtkWidget *navigation_view;
	GtkTreeStore *store;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *icon_column, *text_column;
	GtkTreeSelection *select;

	navigation_view = glade_xml_get_widget(gui.xml, "server channel list");

	store = gtk_tree_store_new(4, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_INT);
	gtk_tree_view_set_model(GTK_TREE_VIEW(navigation_view), GTK_TREE_MODEL(store));

	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(navigation_view), text_column);
	icon_renderer = gtk_cell_renderer_pixbuf_new();
	icon_column = gtk_tree_view_column_new_with_attributes("icon", icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(navigation_view), icon_column);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(navigation_view));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(navigation_selection_changed), NULL);
	g_signal_connect(G_OBJECT(navigation_view), "button_press_event", G_CALLBACK(navigation_click), NULL);
}

void navigation_tree_create_new_network_entry(struct session *sess) {
	GtkTreeStore *store;
	GtkWidget *treeview;
	GtkTreeIter iter;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));

	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 1, "<none>", 2, sess, 3, 0, -1);
}

static gboolean navigation_tree_create_new_channel_entry_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, struct session *data) {
	struct session *s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s->type == SESS_SERVER && s->server == data->server) {
		GtkTreeIter child;
		GtkWidget *treeview;

		treeview = glade_xml_get_widget(gui.xml, "server channel list");

		gtk_tree_store_append(GTK_TREE_STORE(model), &child, iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child, 1, "<none>", 2, data, 3, 0, -1);
		/* make sure the tree expands to show the new channel */
		gtk_tree_view_expand_row(GTK_TREE_VIEW(treeview), path, TRUE);
		return TRUE;
	}
	return FALSE;
}

void navigation_tree_create_new_channel_entry(struct session *sess) {
	GtkTreeStore *store;
	GtkWidget *treeview;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));

	gtk_tree_model_foreach(store, navigation_tree_create_new_channel_entry_iterate, (gpointer) sess);
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
	GtkTreeStore *store;
	GtkWidget *treeview;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));

	gtk_tree_model_foreach(store, navigation_tree_remove_iterate, (gpointer) sess);
}

static gboolean navigation_tree_set_channel_name_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {
	gpointer s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s == data) {
		struct session *sess = s;
		gtk_tree_store_set(GTK_TREE_STORE(model), iter, 1, (sess->channel), -1);
		return TRUE;
	}
	return FALSE;
}

void navigation_tree_set_channel_name(struct session *sess) {
	GtkTreeModel *store;
	GtkWidget *treeview;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	store = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));

	gtk_tree_model_foreach(store, navigation_tree_set_channel_name_iterate, sess);
}

void navigation_selection_changed(GtkTreeSelection *selection, gpointer data) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	gpointer *s;
	session *sess;
	session_gui *tgui;

	if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
		GtkWidget *topic, *entry;

		gtk_tree_model_get(model, &iter, 2, &s, -1);
		sess = s;
		tgui = sess->gui;
		gtk_xtext_buffer_show(gui.xtext, tgui->buffer, TRUE);
		topic = glade_xml_get_widget(gui.xml, "topic entry");
		gtk_text_view_set_buffer(GTK_TEXT_VIEW(topic), tgui->topic_buffer);
		entry = glade_xml_get_widget(gui.xml, "text entry");
		gtk_text_view_set_buffer(GTK_TEXT_VIEW(entry), tgui->entry_buffer);
		gui.current_session = sess;
		userlist_display(tgui);
		set_nickname(sess->server, NULL);
		/* remove any icon that exists */
		gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, NULL, 3, 0, -1);
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
	GtkTreeModel *store;
	GtkWidget *treeview;

	if(sess == gui.current_session)
		return;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	store = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));

	gtk_tree_model_foreach(store, navigation_tree_set_hilight_iterate, (gpointer) sess);
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

void server_context(GtkWidget *treeview, session *selected) {
	static GtkItemFactoryEntry entries[] = {
		{"/Server",			NULL, NULL,			0, "<Branch>"},
		{"/Server/_Information",	NULL, NULL,			0, "<StockItem>", GTK_STOCK_DIALOG_INFO},
		{"/Server/Separator1",		NULL, NULL,			0, "<Separator>"},
		{"/Server/_Reconnect",		NULL, NULL,			0, "<StockItem>", GTK_STOCK_REFRESH},
		{"/Server/_Disconnect",		NULL, disconnect_server,	0, "<StockItem>", GTK_STOCK_STOP},
		{"/Server/Separator2",		NULL, NULL,			0, "<Separator>"},
		{"/Server/_Channels",		NULL, NULL,			0, "<StockItem>", GNOME_STOCK_TEXT_BULLETED_LIST}
	};
	GtkItemFactory *factory;
	GtkWidget *menu;

	factory = gtk_item_factory_new(GTK_TYPE_MENU, "<XChatGnomeNavigationServerContext>", NULL);
	gtk_item_factory_create_items(factory, 7, entries, NULL);
	menu = gtk_item_factory_get_widget(factory, "/Server");

	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 0, 0);
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
	static GtkItemFactoryEntry entries[] = {
		{"/Channel",			NULL, NULL,		0, "<Branch>"},
		{"/Channel/_Save",		NULL, NULL,		0, "<StockItem>", GTK_STOCK_SAVE},
		{"/Channel/Save _As...",	NULL, NULL,		0, "<StockItem>", GTK_STOCK_SAVE_AS},
		{"/Channel/Separator1",		NULL, NULL,		0, "<Separator>"},
		{"/Channel/Leave",		NULL, close_dialog,	0, "<StockItem>", GTK_STOCK_CLOSE},
		{"/Channel/Separator2",		NULL, NULL,		0, "<Separator>"},
		{"/Channel/_Find...",		NULL, NULL,		0, "<StockItem>", GTK_STOCK_FIND},
		{"/Channel/Find Ne_xt",		NULL, NULL,		0, "<StockItem>", GTK_STOCK_FIND},
		{"/Channel/_Clear Window",	NULL, clear_dialog,	0, "<StockItem>", GTK_STOCK_CLEAR},
		{"/Channel/Separator3",		NULL, NULL,		0, "<Separator>"},
		{"/Channel/_Bans",		NULL, NULL,		0, "<StockItem>", GTK_STOCK_DIALOG_WARNING}
	};
	GtkItemFactory *factory;
	GtkWidget *menu;

	factory = gtk_item_factory_new(GTK_TYPE_MENU, "<XChatGnomeNavigationChannelContext>", NULL);
	gtk_item_factory_create_items(factory, 11, entries, NULL);
	menu = gtk_item_factory_get_widget(factory, "/Channel");

	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 0, 0);
}

void dialog_context(GtkWidget *treeview, session *selected) {
	static GtkItemFactoryEntry entries[] = {
		{"/Dialog",			NULL, NULL,		0, "<Branch>"},
		{"/Dialog/_Save",		NULL, NULL,		0, "<StockItem>", GTK_STOCK_SAVE},
		{"/Dialog/Save _As...",		NULL, NULL,		0, "<StockItem>", GTK_STOCK_SAVE_AS},
		{"/Dialog/Separator1",		NULL, NULL,		0, "<Separator>"},
		{"/Dialog/Close",		NULL, close_dialog,	0, "<StockItem>", GTK_STOCK_CLOSE},
		{"/Dialog/Separator2",		NULL, NULL,		0, "<Separator>"},
		{"/Dialog/_Find...",		NULL, NULL,		0, "<StockItem>", GTK_STOCK_FIND},
		{"/Dialog/Find Ne_xt",		NULL, NULL,		0, "<StockItem>", GTK_STOCK_FIND},
		{"/Dialog/_Clear Window",	NULL, clear_dialog,	0, "<StockItem>", GTK_STOCK_CLEAR}
	};
	GtkItemFactory *factory;
	GtkWidget *menu;

	factory = gtk_item_factory_new(GTK_TYPE_MENU, "<XChatGnomeNavigationDialogContext>", NULL);
	gtk_item_factory_create_items(factory, 9, entries, NULL);
	menu = gtk_item_factory_get_widget(factory, "/Dialog");

	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 0, 0);
}
