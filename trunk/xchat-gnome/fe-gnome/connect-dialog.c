/*
 * connect-dialog.c - utilities for displaying the connect dialog
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

#include "connect-dialog.h"
#include "../common/xchat.h"
#include "../common/servlist.h"

static GtkDialogClass *parent_class;

static void
connect_dialog_class_init (ConnectDialogClass *klass)
{
}

static void
connect_dialog_init (ConnectDialog *dialog)
{
}

GType
connect_dialog_get_type (void)
{
	static GType connect_dialog_type = 0;
	if (!connect_dialog_type) {
		static const GTypeInfo connect_dialog_info = {
			sizeof (ConnectDialogClass),
			NULL, NULL,
			(GClassInitFunc) connect_dialog_class_init,
			NULL, NULL,
			sizeof (ConnectDialog),
			0,
			(GInstanceInitFunc) connect_dialog_init,
		};
		connect_dialog_type = g_type_register_static (GTK_TYPE_DIALOG, "ConnectDialog", &connect_dialog_info, 0);

		parent_class = g_type_class_ref (GTK_TYPE_DIALOG);
	}

	return connect_dialog_type;
}

ConnectDialog *
connect_dialog_new ()
{
	ConnectDialog *dialog = g_object_new (connect_dialog_get_type(), 0);
	if (dialog->toplevel == NULL) {
		g_object_unref (dialog);
		return NULL;
	}
	return dialog;
}

/**********************************************************************
 * CRUFT BARRIER ******************************************************
 **********************************************************************/

static void connection_dialog_close (GtkWidget *widget, gpointer data);
static void connection_dialog_connect (GtkWidget *widget, gpointer data);

static void close_window (GtkWidget *widget, GdkEvent *event, gpointer data);

void
initialize_connection_dialog ()
{
	GtkWidget *treeview, *close_button, *connect_button, *window;
	GtkListStore *store;
	GtkCellRenderer *text_renderer;
	GtkTreeViewColumn *text_column;

	GtkTreeSelection *select;

	treeview = glade_xml_get_widget (gui.xml, "connect server list");

	store = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));

	text_renderer = gtk_cell_renderer_text_new ();
	text_column = gtk_tree_view_column_new_with_attributes ("name", text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), text_column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	close_button = glade_xml_get_widget (gui.xml, "close server connect");
	g_signal_connect (G_OBJECT (close_button), "clicked", G_CALLBACK (connection_dialog_close), NULL);
	connect_button = glade_xml_get_widget(gui.xml, "connect to server");
	g_signal_connect (G_OBJECT (connect_button), "clicked", G_CALLBACK (connection_dialog_connect), NULL);
	window = glade_xml_get_widget (gui.xml, "connect to network");
	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (close_window), NULL);
}

void
display_connection_dialog ()
{
	GtkWidget *dialog, *treeview;
	GtkListStore *store;
	GtkTreeIter iter;
	ircnet *net;
	GSList *netlist;

	treeview = glade_xml_get_widget (gui.xml, "connect server list");

	/* FIXME: get a custom list? */
	netlist = network_list;
	store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
	gtk_list_store_clear (store);

	while (netlist) {
		net = netlist->data;
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, net->name, -1);
		netlist = netlist->next;
	}

	dialog = glade_xml_get_widget (gui.xml, "connect to network");
	gtk_widget_show_all (dialog);
}

static void
connection_dialog_close (GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog = glade_xml_get_widget (gui.xml, "connect to network");
	gtk_widget_hide_all (dialog);
}

static void
connection_dialog_connect (GtkWidget *widget, gpointer data)
{
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *network;

	treeview = glade_xml_get_widget (gui.xml, "connect server list");
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 0, &network, -1);
		servlist_connect_by_netname (NULL, network);
		connection_dialog_close (widget, data);
		g_free (network);
	}
}

static void
close_window (GtkWidget *widget, GdkEvent *Event, gpointer data)
{
	connection_dialog_close (widget, data);
}
