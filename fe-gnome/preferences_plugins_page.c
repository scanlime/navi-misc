/*
 * preferences_plugins_page.h - helpers for the plugins preferences page
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

#include "preferences_plugins_page.h"
#include "../common/xchat.h"
// we have to define PLUGIN_C for no particular reason at all to get the xchat_plugin
// structure in all its glory. i dunno why, something to ask zed i suppose
#define PLUGIN_C
typedef struct session xchat_context;
#include "../common/xchat-plugin.h"
#include "../common/plugin.h"

extern GSList *plugin_list;

void initialize_preferences_plugins_page() {
	GtkWidget *treeview;
	GtkListStore *store;
	GtkCellRenderer *text_renderer, *load_renderer;
	GtkTreeViewColumn *text_column, *load_column;
	GtkTreeSelection *select;

	treeview = glade_xml_get_widget(gui.xml, "plugins list");

	store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));

	load_renderer = gtk_cell_renderer_toggle_new();
	load_column = gtk_tree_view_column_new_with_attributes("Load", load_renderer, "active", 3, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), load_column);
	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("Plugin", text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), text_column);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	preferences_plugins_page_populate();
}

void preferences_plugins_page_populate() {
	GtkWidget *treeview;
	GtkTreeIter iter;
	GtkListStore *store;
	GSList *list;
	xchat_plugin *plugin;

	treeview = glade_xml_get_widget(gui.xml, "plugins list");
	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));

	gtk_list_store_clear(store);

	list = plugin_list;
	while(list) {
		plugin = list->data;
		if(plugin->version[0] != 0) {
			gtk_list_store_append(store, &iter);
			gtk_list_store_set(store, &iter, 0, plugin->name, 1, plugin->version, 2, plugin->desc, -1);
		}
		list = list->next;
	}
}
