/*
 * preferences_servers_page.c - helpers for the servers preferences page
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

#include "preferences_servers_page.h"
#include "preferences.h"
#include "../common/xchat.h"
#include "../common/servlist.h"

void preferences_servers_selected(GtkTreeSelection *selection, gpointer data);

static void edit_global_changed(GtkToggleButton *togglebutton, gpointer data) {
	GtkWidget *nick, *real;

	nick = glade_xml_get_widget(gui.xml, "server config nickname");
	real = glade_xml_get_widget(gui.xml, "server config realname");
	if(gtk_toggle_button_get_active(togglebutton)) {
		gtk_widget_set_sensitive(nick, FALSE);
		gtk_widget_set_sensitive(real, FALSE);
	} else {
		gtk_widget_set_sensitive(nick, TRUE);
		gtk_widget_set_sensitive(real, TRUE);
	}
}

static void edit_clicked(GtkWidget *button, gpointer data) {
	GtkWidget *dialog, *password, *nick, *real;
	GtkWidget *treeview, *widget;
	GtkSizeGroup *group;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	ircnet *net;

	dialog = glade_xml_get_widget(gui.xml, "server configuration");

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	password = glade_xml_get_widget(gui.xml, "server config password");
	gtk_size_group_add_widget(group, password);
	nick = glade_xml_get_widget(gui.xml, "server config nickname");
	gtk_size_group_add_widget(group, nick);
	real = glade_xml_get_widget(gui.xml, "server config realname");
	gtk_size_group_add_widget(group, real);
	g_object_unref(group);

	treeview = glade_xml_get_widget(gui.xml, "configure server list");
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

	gtk_tree_selection_get_selected(select, &model, &iter);
	gtk_tree_model_get(model, &iter, 2, &net, -1);

	if(net->pass != NULL)
		gtk_entry_set_text(GTK_ENTRY(password), net->pass);

	widget = glade_xml_get_widget(gui.xml, "server config usedefaults");
	if(net->flags & FLAG_USE_GLOBAL) {
		gtk_entry_set_text(GTK_ENTRY(nick), preferences_nickname());
		gtk_entry_set_text(GTK_ENTRY(real), preferences_realname());
		gtk_widget_set_sensitive(nick, FALSE);
		gtk_widget_set_sensitive(real, FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), FALSE);
		gtk_widget_set_sensitive(nick, TRUE);
		gtk_widget_set_sensitive(real, TRUE);
		if(net->nick != NULL)
			gtk_entry_set_text(GTK_ENTRY(nick), net->nick);
		if(net->real != NULL)
			gtk_entry_set_text(GTK_ENTRY(real), net->real);
	}
	g_signal_connect(G_OBJECT(widget), "toggled", G_CALLBACK(edit_global_changed), NULL);

	widget = glade_xml_get_widget(gui.xml, "server config autoconnect");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), (net->flags & FLAG_AUTO_CONNECT));

	widget = glade_xml_get_widget(gui.xml, "server config ssl");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), (net->flags & FLAG_USE_SSL));

	widget = glade_xml_get_widget(gui.xml, "server config cycle");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), (net->flags & FLAG_CYCLE));

	widget = glade_xml_get_widget(gui.xml, "server config network name");
	char *name = g_strdup_printf("<span weight=\"bold\">%s</span>", net->name);
	gtk_label_set_text(GTK_LABEL(widget), name);
	gtk_label_set_use_markup(GTK_LABEL(widget), TRUE);
	g_free(name);

	gtk_widget_show_all(dialog);
}

void initialize_preferences_servers_page() {
	GtkWidget *treeview, *edit_button, *remove_button;
	GtkListStore *store;
	GtkCellRenderer *text_renderer, *autoconnect_renderer;
	GtkTreeViewColumn *text_column, *autoconnect_column;
	GtkTreeSelection *select;

	treeview = glade_xml_get_widget(gui.xml, "configure server list");

	store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_POINTER);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));

	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), text_column);
	autoconnect_renderer = gtk_cell_renderer_toggle_new();
	autoconnect_column = gtk_tree_view_column_new_with_attributes("auto-connect", autoconnect_renderer, "active", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), autoconnect_column);

	edit_button = glade_xml_get_widget(gui.xml, "servers edit");
	g_signal_connect(G_OBJECT(edit_button), "clicked", G_CALLBACK(edit_clicked), NULL);
	gtk_widget_set_sensitive(edit_button, FALSE);
	remove_button = glade_xml_get_widget(gui.xml, "servers remove");
	gtk_widget_set_sensitive(remove_button, FALSE);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(preferences_servers_selected), NULL);

	preferences_servers_page_populate(treeview, NULL);
}

void preferences_servers_page_populate(GtkWidget *treeview, GSList *netlist) {
	GtkListStore *store;
	GtkTreeIter iter;
	ircnet *net;

	/* FIXME: get a custom list? */
	if(!netlist) {
//		net = servlist_net_add(_("New Network"), "");
//		servlist_server_add(net, "newserver/6667");
		netlist = network_list;
	}
	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
	gtk_list_store_clear(store);

	while(netlist) {
		net = netlist->data;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, 0, net->name, 1, (net->flags & FLAG_AUTO_CONNECT), 2, net, -1);
		netlist = netlist->next;
	}
}

void preferences_servers_selected(GtkTreeSelection *selection, gpointer data) {
	GtkWidget *edit_button, *remove_button;

	edit_button = glade_xml_get_widget(gui.xml, "servers edit");
	remove_button = glade_xml_get_widget(gui.xml, "servers remove");
	if(gtk_tree_selection_get_selected(selection, NULL, NULL)) {
		gtk_widget_set_sensitive(edit_button, TRUE);
		gtk_widget_set_sensitive(remove_button, TRUE);
	} else {
		gtk_widget_set_sensitive(edit_button, FALSE);
		gtk_widget_set_sensitive(remove_button, FALSE);
	}
}
