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
connect_dialog_finalize (GObject *object)
{
	ConnectDialog *dialog = (ConnectDialog *) object;

	g_object_unref (dialog->server_store);

	((GObjectClass *) parent_class)->finalize (object);
}

static void
connect_dialog_class_init (ConnectDialogClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;
	object_class->finalize = connect_dialog_finalize;
}

static void
selection_changed (GtkTreeSelection *select, ConnectDialog *dialog)
{
	if (gtk_tree_selection_get_selected (select, NULL, NULL))
		gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, TRUE);
	else
		gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, FALSE);
}

static void
dialog_response (ConnectDialog *dialog, gint response, gpointer data)
{
	if (response == GTK_RESPONSE_OK) {
		GtkTreeModel *model;
		GtkTreeIter iter;
		GtkTreeSelection *select;
		gchar *network;

		select = gtk_tree_view_get_selection (GTK_TREE_VIEW (dialog->server_list));
		if (gtk_tree_selection_get_selected (select, &model, &iter)) {
			gtk_tree_model_get (model, &iter, 0, &network, -1);
			servlist_connect_by_netname (NULL, network);
			g_free (network);
			gtk_widget_destroy (dialog);
		}
	} else {
		gtk_widget_destroy (GTK_WIDGET (dialog));
	}
}

static void
connect_dialog_init (ConnectDialog *dialog)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GladeXML *xml;
	GtkWidget *button;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GSList *netlist;
	ircnet *net;

	dialog->toplevel = NULL;

	xml = NULL;
	if (g_file_test ("connect-dialog.glade", G_FILE_TEST_EXISTS))
		xml = glade_xml_new ("connect-dialog.glade", "toplevel", NULL);
	if (!xml)
		xml = glade_xml_new (XCHATSHAREDIR "/connect-dialog.glade", "toplevel", NULL);
	if (!xml)
		return;

#define GW(name) ((dialog->name) = glade_xml_get_widget (xml, #name))
	GW(toplevel);
	GW(server_list);
#undef GW

	g_object_unref (xml);

	dialog->server_store = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (dialog->server_list), GTK_TREE_MODEL (dialog->server_store));

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("name", renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (dialog->server_list), column);

	button = gtk_button_new_with_mnemonic (_("_Connect"));
	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
	gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button, GTK_RESPONSE_OK);
	gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), dialog->toplevel);
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (dialog->server_list));
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (selection_changed), dialog);

	netlist = network_list;
	while (netlist) {
		net = netlist->data;
		gtk_list_store_append (dialog->server_store, &iter);
		gtk_list_store_set (dialog->server_store, &iter, 0, net->name, -1);
		netlist = g_slist_next (netlist);
	}

	gtk_window_set_default_size (GTK_WINDOW (dialog), 320, 240);
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, FALSE);
	g_signal_connect (G_OBJECT (dialog), "response", G_CALLBACK (dialog_response), NULL);
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
