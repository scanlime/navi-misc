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
#include "../common/xchat.h"
#include "../common/servlist.h"

static GHashTable *enctoindex;
static gboolean initialized = FALSE;

void preferences_servers_selected (GtkTreeSelection *selection, gpointer data);

static void
check_input (GtkWidget *widget, GtkWidget *dialog)
{
	GtkWidget *name;
	GtkWidget *globalnames, *real, *nick;

	name = glade_xml_get_widget (gui.xml, "server config network name");
	globalnames = glade_xml_get_widget (gui.xml, "server config usedefaults");
	real = glade_xml_get_widget (gui.xml, "server config realname");
	nick = glade_xml_get_widget (gui.xml, "server config nickname");

	if (strlen (gtk_entry_get_text (GTK_ENTRY (name))) == 0)
		goto fail;
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (globalnames)))
		goto success;
	if (strlen (gtk_entry_get_text (GTK_ENTRY (real))) == 0)
		goto fail;
	if (strlen (gtk_entry_get_text (GTK_ENTRY (nick))) == 0)
		goto fail;

success:
	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, TRUE);
	return;
fail:
	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, FALSE);
}

static void
globals_toggled (GtkWidget *widget, GtkWidget *dialog)
{
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
		GtkWidget *real, *nick;
		gchar *text;
		GConfClient *client = gconf_client_get_default ();
		real = glade_xml_get_widget (gui.xml, "server config realname");
		nick = glade_xml_get_widget (gui.xml, "server config nickname");
		text = gconf_client_get_string (client, "/apps/xchat/irc/nickname", NULL);
		gtk_entry_set_text (GTK_ENTRY (nick), text);
		g_free (text);
		text = gconf_client_get_string (client, "/apps/xchat/irc/realname", NULL);
		gtk_entry_set_text (GTK_ENTRY(real), text);
		g_free (text);
		gtk_widget_set_sensitive (nick, FALSE);
		gtk_widget_set_sensitive (real, FALSE);
	}
	check_input (widget, dialog);
}

static void
edit_global_changed (GtkToggleButton *togglebutton, gpointer data)
{
	GtkWidget *nick, *real;

	nick = glade_xml_get_widget (gui.xml, "server config nickname");
	real = glade_xml_get_widget (gui.xml, "server config realname");
	if (gtk_toggle_button_get_active (togglebutton)) {
		gtk_widget_set_sensitive (nick, FALSE);
		gtk_widget_set_sensitive (real, FALSE);
	} else {
		gtk_widget_set_sensitive (nick, TRUE);
		gtk_widget_set_sensitive (real, TRUE);
	}
}

static void
edit_ok_clicked (GtkWidget *button, gpointer data)
{
	GtkWidget *treeview, *widget, *dialog;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	ircnet *net;
	char *realname, *text;
	int position, newflags;

	treeview = glade_xml_get_widget (gui.xml, "configure server list");
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	if (!gtk_tree_selection_get_selected (select, &model, &iter))
		return;
	gtk_tree_model_get (model, &iter, 0, &realname, 2, &net, -1);

	widget = glade_xml_get_widget (gui.xml, "server config network name");
	text = (char *) gtk_entry_get_text (GTK_ENTRY (widget));
	if (!((servlist_net_find (text, &position) == NULL) || (strcasecmp (realname, text) == 0))) {
		error_dialog (_("Duplicate entry"), _("You already have a network of that name"));
		widget = glade_xml_get_widget (gui.xml, "server config network name");
		gtk_widget_grab_focus (widget);
		return;
	}
	net->name = g_strdup (text);

	newflags = 0;

	widget = glade_xml_get_widget (gui.xml, "server config autoconnect");
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
		newflags |= FLAG_AUTO_CONNECT;
	widget = glade_xml_get_widget (gui.xml, "server config ssl");
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
		newflags |= FLAG_USE_SSL;
	widget = glade_xml_get_widget (gui.xml, "server config cycle");
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
		newflags |= FLAG_CYCLE;
	widget = glade_xml_get_widget (gui.xml, "server config password");
	net->pass = g_strdup (gtk_entry_get_text (GTK_ENTRY (widget)));
	widget = glade_xml_get_widget (gui.xml, "server config usedefaults");
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
		newflags |= FLAG_USE_GLOBAL;
	} else {
		widget = glade_xml_get_widget (gui.xml, "server config nickname");
		text = (char *) gtk_entry_get_text (GTK_ENTRY (widget));
		net->nick = g_strdup (text);
		widget = glade_xml_get_widget (gui.xml, "server config realname");
		text = (char *) gtk_entry_get_text (GTK_ENTRY (widget));
		net->real = g_strdup (text);
	}

	net->flags = newflags;

	servlist_save ();
	preferences_servers_page_populate (treeview, network_list);

	dialog = glade_xml_get_widget (gui.xml, "server configuration");
	gtk_widget_hide_all (dialog);
}

static void
edit_cancel_clicked (GtkWidget *button, gpointer data)
{
	GtkWidget *dialog;

	dialog = glade_xml_get_widget (gui.xml, "server configuration");
	gtk_widget_hide_all (dialog);
}

static void
server_selection_changed (GtkTreeSelection *selection, gpointer data)
{	GtkWidget *button;

	button = glade_xml_get_widget (gui.xml, "server config remove server");

	if (gtk_tree_selection_get_selected (selection, NULL, NULL)) {
		gtk_widget_set_sensitive (button, TRUE);
	} else {
		gtk_widget_set_sensitive (button, FALSE);
	}
}

static void
add_server_clicked (GtkButton *button, gpointer data)
{
	GtkWidget *treeview;
	GtkListStore *store;
	GtkTreeIter iter;

	treeview = glade_xml_get_widget (gui.xml, "server config servers");
	store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
	gtk_list_store_append (store, &iter);
}

static void
remove_server_clicked (GtkButton *button, gpointer data)
{
	GtkWidget *treeview;
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	treeview = glade_xml_get_widget (gui.xml, "server config servers");
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (selection), &model, &iter)) {
		ircserver *serv;

		gtk_tree_model_get (model, &iter, 1, &serv, -1);
		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
		//servlist_server_remove (net, serv);
		servlist_save ();
	}
}

static void
populate_servers_list (GtkListStore *store, ircnet *net)
{
	GtkTreeIter iter;
	ircserver *serv;
	GSList *list = net->servlist;

	while (list) {
		serv = list->data;
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, serv->hostname, 1, serv, -1);
		list = g_slist_next (list);
	}
}

static void
autojoin_selection_changed (GtkTreeSelection *selection, gpointer data)
{
	GtkWidget *button;

	button = glade_xml_get_widget (gui.xml, "remove autojoin channel");

	gtk_widget_set_sensitive (button, gtk_tree_selection_get_selected (selection, NULL, NULL));
}

static void
add_autojoin_clicked (GtkButton *button, gpointer data)
{
	GtkWidget *treeview;
	GtkListStore *store;
	GtkTreeIter iter;

	treeview = glade_xml_get_widget (gui.xml, "server config channels");
	store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
	gtk_list_store_append (store, &iter);
}

static void
remove_autojoin_clicked (GtkButton *button, gpointer data)
{
	GtkWidget *treeview;
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	treeview = glade_xml_get_widget (gui.xml, "server config channels");
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
	}
}

static void
populate_channels_list (GtkListStore *store, ircnet *net)
{
}

static void
server_entry_edited (GtkCellRendererText *renderer, gchar *arg1, gchar *arg2, GtkTreeView *view)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (view);
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, arg2, -1);
}

static void
edit_clicked (GtkWidget *button, gpointer data)
{
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
