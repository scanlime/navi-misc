/*
 * preferences_dialog.c - helpers for the preference dialog
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

#include "preferences_dialog.h"
#include "preferences_servers_page.h"
#include "pixmaps.h"

void initialize_pages_list();
void hide_preferences_dialog(GtkWidget *widget, gpointer data);
void initialize_irc_preferences_page();
void initialize_file_transfers_page();
void initialize_servers_page();
void settings_page_changed(GtkTreeSelection *selection, gpointer data);

void initialize_preferences_dialog() {
	GtkWidget *close_button;

	gui.preferences_dialog = GTK_DIALOG(glade_xml_get_widget(gui.xml, "preferences"));
	gtk_window_set_transient_for(GTK_WINDOW(gui.preferences_dialog), GTK_WINDOW(gui.main_window));
	gtk_widget_hide_all(GTK_WIDGET(gui.preferences_dialog));
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(glade_xml_get_widget(gui.xml, "settings notebook")), FALSE);
	initialize_pages_list();
	initialize_irc_preferences_page();
	initialize_file_transfers_page();
	initialize_preferences_servers_page();

	close_button = glade_xml_get_widget(gui.xml, "close preferences");
	g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(hide_preferences_dialog), NULL);
}

void hide_preferences_dialog(GtkWidget *widget, gpointer data) {
	gtk_widget_hide_all(GTK_WIDGET(gui.preferences_dialog));
}

void initialize_pages_list() {
	GtkWidget *options_list;
	GtkListStore *store;
	GtkTreeIter iter;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;

	options_list = glade_xml_get_widget(gui.xml, "settings page list");

	store = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
	gtk_tree_view_set_model(GTK_TREE_VIEW(options_list), GTK_TREE_MODEL(store));

	column = gtk_tree_view_column_new();
	icon_renderer = gtk_cell_renderer_pixbuf_new();
	text_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, icon_renderer, FALSE);
	gtk_tree_view_column_set_attributes(column, icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_column_pack_start(column, text_renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, text_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(options_list), column);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(options_list));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(settings_page_changed), NULL);

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, pix_prefs_irc, 1, "IRC Preferences", 2, 0, -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, pix_prefs_colors, 1, "Colors", 2, 1, -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, pix_prefs_dcc, 1, "File Transfers & DCC", 2, 2, -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, pix_prefs_networks, 1, "Networks", 2, 3, -1);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, pix_prefs_plugins, 1, "Scripts and Plugins", 2, 4, -1);
}

void initialize_irc_preferences_page() {
	GtkWidget *widget;
	GtkSizeGroup *group;

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "nick name");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "real name");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "quit message");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "part message");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "away message");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "highlight list container");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);
}

void initialize_file_transfers_page() {
	GtkWidget *widget;
	GtkSizeGroup *group;

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "dcc transfer destination");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "completed dcc destination");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "download label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "completed label");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "dcc ip address");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "individual send throttle");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "global send throttle");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "individual receive throttle");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "global receive throttle");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);
}

void initialize_servers_page() {
}

void settings_page_changed(GtkTreeSelection *selection, gpointer data) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkWidget *notebook;
	gint page;

	if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
		notebook = glade_xml_get_widget(gui.xml, "settings notebook");
		gtk_tree_model_get(model, &iter, 2, &page, -1);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page);
	}
}
