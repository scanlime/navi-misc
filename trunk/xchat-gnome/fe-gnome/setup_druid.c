/*
 * setup_druid.c - helpers for the initial setup druid
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

#include "setup_druid.h"
#include "gui.h"
#include "../common/servlist.h"

static gboolean druid_finished;
void setup_druid_servers_populate(GtkWidget *treeview);
void setup_druid_nicknames_prepare(GnomeDruidPage *p, GtkWidget *w, gpointer d);
void setup_druid_finish_prepare(GnomeDruidPage *p, GtkWidget *w, gpointer d);
void setup_druid_finish(GnomeDruidPage *p, GtkWidget *w, gpointer d);
void setup_druid_nickname_changed(GtkEditable *entry, gpointer d);
void setup_druid_realname_changed(GtkEditable *entry, gpointer d);

void initialize_setup_druid() {
	GtkWidget *widget, *treeview;
	GtkSizeGroup *group;
	GtkWidget *nickname_page, *finish_page;
	GtkWidget *nickname_entry, *realname_entry;
	GtkListStore *store;
	GtkCellRenderer *text_renderer, *autoconnect_renderer;
	GtkTreeViewColumn *text_column, *autoconnect_column;
	GtkTreeSelection *select;

	gui.setup_druid = GNOME_DRUID(glade_xml_get_widget(gui.xml, "setup druid"));

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "setup druid nickname");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "setup druid realname");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);

	nickname_page = glade_xml_get_widget(gui.xml, "setup druid nicknames");
	g_signal_connect(G_OBJECT(nickname_page), "prepare", G_CALLBACK(setup_druid_nicknames_prepare), NULL);
	finish_page = glade_xml_get_widget(gui.xml, "setup druid finish");
	g_signal_connect(G_OBJECT(finish_page), "prepare", G_CALLBACK(setup_druid_finish_prepare), NULL);
	g_signal_connect(G_OBJECT(finish_page), "finish", G_CALLBACK(setup_druid_finish), NULL);

	nickname_entry = glade_xml_get_widget(gui.xml, "setup druid nickname");
	g_signal_connect(G_OBJECT(nickname_entry), "changed", G_CALLBACK(setup_druid_nickname_changed), NULL);
	g_signal_connect(G_OBJECT(nickname_entry), "realize", G_CALLBACK(setup_druid_nickname_changed), NULL);
	realname_entry = glade_xml_get_widget(gui.xml, "setup druid realname");
	g_signal_connect(G_OBJECT(realname_entry), "changed", G_CALLBACK(setup_druid_realname_changed), NULL);

	treeview = glade_xml_get_widget(gui.xml, "setup druid server list");

	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_BOOLEAN);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));

	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), text_column);
	autoconnect_renderer = gtk_cell_renderer_toggle_new();
	autoconnect_column = gtk_tree_view_column_new_with_attributes("auto-connect", autoconnect_renderer, "active", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), autoconnect_column);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_NONE);

	setup_druid_servers_populate(treeview);
}

void run_setup_druid() {
	druid_finished = FALSE;
	gtk_widget_show_all(GTK_WIDGET(gui.setup_druid));
	while(!druid_finished) {
		g_main_context_iteration(NULL, TRUE);
	}
}

void setup_druid_servers_populate(GtkWidget *treeview) {
	GtkListStore *store;
	GtkTreeIter iter;
	ircnet *net;
	GSList *netlist;

	netlist = network_list;
	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
	gtk_list_store_clear(store);

	while(netlist) {
		net = netlist->data;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, 0, net->name, 1, FALSE, -1);
		netlist = netlist->next;
	}
}

void setup_druid_nicknames_prepare(GnomeDruidPage *p, GtkWidget *w, gpointer d) {
	GtkWidget *nick, *real;

	real = glade_xml_get_widget(gui.xml, "setup druid realname");
	nick = glade_xml_get_widget(gui.xml, "setup druid nickname");
	gtk_entry_set_text(GTK_ENTRY(real), g_get_real_name());
	gtk_widget_grab_focus(nick);
}

void setup_druid_finish_prepare(GnomeDruidPage *p, GtkWidget *w, gpointer d) {
	gnome_druid_set_show_finish(gui.setup_druid, TRUE);
}

void setup_druid_finish(GnomeDruidPage *p, GtkWidget *w, gpointer d) {
	GtkWidget *druid_window;

	druid_window = glade_xml_get_widget(gui.xml, "setup druid window");
	gtk_widget_hide_all(druid_window);
	druid_finished = TRUE;
}

void setup_druid_nickname_changed(GtkEditable *entry, gpointer d) {
	GtkWidget *nick, *real;

	nick = glade_xml_get_widget(gui.xml, "setup druid nickname");
	real = glade_xml_get_widget(gui.xml, "setup druid realname");
	if(strlen(gtk_entry_get_text(GTK_ENTRY(nick))) == 0 ||
	   strlen(gtk_entry_get_text(GTK_ENTRY(real))) == 0)
		gnome_druid_set_buttons_sensitive(gui.setup_druid, TRUE, FALSE, TRUE, TRUE);
	else
		gnome_druid_set_buttons_sensitive(gui.setup_druid, TRUE, TRUE, TRUE, TRUE);
}

void setup_druid_realname_changed(GtkEditable *entry, gpointer d) {
	GtkWidget *nick, *real;

	nick = glade_xml_get_widget(gui.xml, "setup druid nickname");
	real = glade_xml_get_widget(gui.xml, "setup druid realname");
	if(strlen(gtk_entry_get_text(GTK_ENTRY(nick))) == 0 ||
	   strlen(gtk_entry_get_text(GTK_ENTRY(real))) == 0)
		gnome_druid_set_buttons_sensitive(gui.setup_druid, TRUE, FALSE, TRUE, TRUE);
	else
		gnome_druid_set_buttons_sensitive(gui.setup_druid, TRUE, TRUE, TRUE, TRUE);
}
