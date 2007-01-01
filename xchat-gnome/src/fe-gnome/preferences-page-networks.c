/*
 * preferences-page-networks.c - helpers for the servers preferences page
 *
 * Copyright (C) 2004-2007 xchat-gnome team
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

#include <config.h>
#include <glib/gi18n.h>
#include <gconf/gconf-client.h>
#include "preferences-page-networks.h"
#include "preferences-dialog.h"
#include "irc-network.h"
#include "irc-network-editor.h"
#include "util.h"
#include "../common/xchat.h"
#include "../common/servlist.h"

G_DEFINE_TYPE(PreferencesPageNetworks, preferences_page_networks, PREFERENCES_PAGE_TYPE)

/*******************************************************************************
 * CRUFT BARRIER ***************************************************************
 *******************************************************************************/

static void
populate (PreferencesPageNetworks *page)
{
	GtkTreeIter iter;
	ircnet *net;
	GSList *netlist;

	netlist = network_list;
	gtk_list_store_clear (page->network_store);

	while (netlist) {
		net = netlist->data;
		gtk_list_store_append (page->network_store, &iter);
		gtk_list_store_set (page->network_store, &iter, 0, net->name, 1, net, -1);

		netlist = g_slist_next (netlist);
	}
}

static void
add_clicked (GtkWidget *button, PreferencesPageNetworks *page)
{
	IrcNetwork *n;
	IrcNetworkEditor *e;

	n = irc_network_new (NULL);
	e = irc_network_editor_new (n);

	g_object_weak_ref (G_OBJECT (e), (GWeakNotify) populate, page);
	irc_network_editor_run (e);
	g_object_unref (n);
}

static void
edit_clicked (GtkWidget *button, PreferencesPageNetworks *page)
{
	IrcNetwork *n;
	IrcNetworkEditor *e;
	ircnet *net;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->network_list));
	if (!gtk_tree_selection_get_selected (select, &model, &iter)) {
		return;
	}
	gtk_tree_model_get (model, &iter, 1, &net, -1);

	n = irc_network_new (net);
	e = irc_network_editor_new (n);

	g_object_weak_ref (G_OBJECT (e), (GWeakNotify) populate, page);
	irc_network_editor_run (e);
	g_object_unref (n);
}

static void
edit_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, PreferencesPageNetworks *page)
{
	GtkTreeIter iter;
	if (gtk_tree_model_get_iter (GTK_TREE_MODEL (page->sort_model), &iter, path)) {
		edit_clicked (NULL, page);
	}
}

static void
remove_clicked (GtkWidget *button, PreferencesPageNetworks *page)
{
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkWidget *dialog;
	ircnet *net;
	gint r;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->network_list));
	if (!gtk_tree_selection_get_selected (select, &model, &iter)) {
		return;
	}
	gtk_tree_model_get(model, &iter, 1, &net, -1);

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
	populate (page);
}

static void
selection_changed (GtkTreeSelection *selection, PreferencesPageNetworks *page)
{
	if (gtk_tree_selection_get_selected (selection, NULL, NULL)) {
		gtk_widget_set_sensitive (page->network_edit, TRUE);
		gtk_widget_set_sensitive (page->network_remove, TRUE);
	} else {
		gtk_widget_set_sensitive (page->network_edit, FALSE);
		gtk_widget_set_sensitive (page->network_remove, FALSE);
	}
}

PreferencesPageNetworks *
preferences_page_networks_new (gpointer prefs_dialog, GladeXML *xml)
{
	PreferencesPageNetworks *page = g_object_new (PREFERENCES_PAGE_NETWORKS_TYPE, NULL);
	PreferencesDialog *p = (PreferencesDialog *) prefs_dialog;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;

#define GW(name) ((page->name) = glade_xml_get_widget (xml, #name))
	GW(network_list);
	GW(network_add);
	GW(network_edit);
	GW(network_remove);
#undef GW

	gtk_widget_set_sensitive (page->network_edit, FALSE);
	gtk_widget_set_sensitive (page->network_remove, FALSE);

	PREFERENCES_PAGE (page)->icon = gtk_widget_render_icon (page->network_edit, GTK_STOCK_NETWORK, GTK_ICON_SIZE_MENU, NULL);

	gtk_list_store_append (p->page_store, &iter);
	gtk_list_store_set (p->page_store, &iter, 0, PREFERENCES_PAGE (page)->icon, 1, _("Networks"), 2, 4, -1);

	page->network_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);
	page->sort_model = GTK_TREE_MODEL_SORT (gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (page->network_store)));
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (page->sort_model), 0, GTK_SORT_ASCENDING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (page->network_list), GTK_TREE_MODEL (page->sort_model));

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Name"), renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (page->network_list), column);

	g_signal_connect (G_OBJECT (page->network_list),   "row-activated", G_CALLBACK (edit_activated), page);
	g_signal_connect (G_OBJECT (page->network_add),    "clicked", G_CALLBACK (add_clicked),          page);
	g_signal_connect (G_OBJECT (page->network_edit),   "clicked", G_CALLBACK (edit_clicked),         page);
	g_signal_connect (G_OBJECT (page->network_remove), "clicked", G_CALLBACK (remove_clicked),       page);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->network_list));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (selection_changed), page);

	populate (page);

	return page;
}

static void
preferences_page_networks_init (PreferencesPageNetworks *page)
{
}

static void
preferences_page_networks_dispose (GObject *object)
{
	PreferencesPageNetworks *page = (PreferencesPageNetworks *) object;

	if (page->sort_model) {
		g_object_unref (page->sort_model);
		page->sort_model = NULL;
	}

	if (page->network_store) {
		g_object_unref (page->network_store);
		page->network_store = NULL;
	}
 
	G_OBJECT_CLASS (preferences_page_networks_parent_class)->dispose (object);
}	

static void
preferences_page_networks_class_init (PreferencesPageNetworksClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;

	object_class->dispose = preferences_page_networks_dispose;
}
