/*
 * preferences_servers_page.c - helpers for the servers preferences page
 *
 * Copyright (C) 2004-2005 xchat-gnome team
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
#include "preferences_servers_page.h"
#include "preferences.h"
#include "error_dialog.h"
#include "irc-network.h"
#include "irc-network-editor.h"
#include "../common/xchat.h"
#include "../common/servlist.h"

void preferences_servers_selected (GtkTreeSelection *selection, gpointer data);

static void
edit_clicked (GtkWidget *button, gpointer data)
{
	IrcNetwork *n;
	IrcNetworkEditor *e;
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	ircnet *net;

	treeview = glade_xml_get_widget (gui.xml, "configure server list");

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	if (!gtk_tree_selection_get_selected (select, &model, &iter))
		return;
	gtk_tree_model_get (model, &iter, 2, &net, -1);

	n = irc_network_new (net);
	e = irc_network_editor_new (n);

	irc_network_editor_run (e);
	gtk_widget_destroy (n);
}

static void
add_clicked (GtkWidget *button, gpointer data)
{
}

static void
remove_clicked (GtkWidget *button, gpointer data)
{
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkWidget *dialog;
	ircnet *net;
	gint r;

	treeview = glade_xml_get_widget (gui.xml, "configure server list");

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	if (!gtk_tree_selection_get_selected (select, &model, &iter))
		return;
	gtk_tree_model_get(model, &iter, 2, &net, -1);

	dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, _("Really remove network \"%s\" and all of its servers?"), net->name);
	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_REMOVE, GTK_RESPONSE_OK);
	r = gtk_dialog_run (GTK_DIALOG (dialog));
	if (r == GTK_RESPONSE_OK) {
		GtkTreeIter child;
		GtkListStore *store = GTK_LIST_STORE (gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model)));
		gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model), &child, &iter);
		gtk_list_store_remove (store, &child);
		servlist_net_remove (net);
		servlist_save ();
	}
	gtk_widget_destroy (dialog);
}

void
initialize_preferences_servers_page ()
{
	GtkWidget *treeview, *edit_button, *remove_button, *add_button;
	GtkListStore *store;
	GtkCellRenderer *text_renderer, *autoconnect_renderer;
	GtkTreeViewColumn *text_column, *autoconnect_column;
	GtkTreeSelection *select;
	GtkTreeModelSort *sort;

	treeview = glade_xml_get_widget (gui.xml, "configure server list");

	store = gtk_list_store_new (3, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_POINTER);
	sort = GTK_TREE_MODEL_SORT (gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (store)));
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (sort), 1, GTK_SORT_DESCENDING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (sort));

	text_renderer = gtk_cell_renderer_text_new ();
	text_column = gtk_tree_view_column_new_with_attributes ("Name", text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), text_column);
	autoconnect_renderer = gtk_cell_renderer_toggle_new ();
	autoconnect_column = gtk_tree_view_column_new_with_attributes ("Auto-Connect", autoconnect_renderer, "active", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), autoconnect_column);

	edit_button = glade_xml_get_widget (gui.xml, "servers edit");
	remove_button = glade_xml_get_widget (gui.xml, "servers remove");
	add_button = glade_xml_get_widget (gui.xml, "servers add");
	g_signal_connect (G_OBJECT (edit_button), "clicked", G_CALLBACK (edit_clicked), NULL);
	g_signal_connect (G_OBJECT (remove_button), "clicked", G_CALLBACK (remove_clicked), NULL);
	g_signal_connect (G_OBJECT (add_button), "clicked", G_CALLBACK (add_clicked), NULL);
	gtk_widget_set_sensitive (edit_button, FALSE);
	remove_button = glade_xml_get_widget (gui.xml, "servers remove");
	gtk_widget_set_sensitive (remove_button, FALSE);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (preferences_servers_selected), NULL);

	preferences_servers_page_populate (treeview, NULL);
}

void
preferences_servers_page_populate (GtkWidget *treeview, GSList *netlist)
{
	GtkListStore *store;
	GtkTreeModel *model;
	GtkTreeIter iter;
	ircnet *net;

	if (!netlist)
		netlist = network_list;
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
	store = GTK_LIST_STORE (gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model)));
	gtk_list_store_clear (store);

	while (netlist) {
		net = netlist->data;
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, net->name, 1, (net->flags & FLAG_AUTO_CONNECT), 2, net, -1);
		netlist = netlist->next;
	}
}

void
preferences_servers_selected(GtkTreeSelection *selection, gpointer data)
{
	GtkWidget *edit_button, *remove_button;

	edit_button = glade_xml_get_widget (gui.xml, "servers edit");
	remove_button = glade_xml_get_widget (gui.xml, "servers remove");
	if (gtk_tree_selection_get_selected (selection, NULL, NULL)) {
		gtk_widget_set_sensitive (edit_button, TRUE);
		gtk_widget_set_sensitive (remove_button, TRUE);
	} else {
		gtk_widget_set_sensitive (edit_button, FALSE);
		gtk_widget_set_sensitive (remove_button, FALSE);
	}
}
