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

#include <gconf/gconf-client.h>
#include "preferences_servers_page.h"
#include "preferences.h"
#include "../common/xchat.h"
#include "../common/servlist.h"

static const char *encodings[] =
{
	"UTF-8",
	"ISO-8859-15 (Western Europe)",
	"ISO-8859-2 (Central Europe)",
	"ISO-8859-7 (Greek)",
	"ISO-8859-8 (Hebrew)",
	"ISO-8859-9 (Turkish)",
	"ISO-2022-JP (Japanese)",
	"SJIS (Japanese)",
	"CP949 (Korean)",
	"CP1251 (Cyrillic)",
	"CP1256 (Arabic)",
	"GB18030 (Chinese)",
	NULL
};
static GHashTable *enctoindex;
static gboolean initialized = FALSE;

void preferences_servers_selected (GtkTreeSelection *selection, gpointer data);

static void
add_clicked (GtkWidget *button, gpointer data)
{
	g_print ("add clicked\n");
}

static void
edit_global_changed (GtkToggleButton *togglebutton, gpointer data)
{
	GtkWidget *nick, *real;

	nick = glade_xml_get_widget (gui.xml, "server config nickname");
	real = glade_xml_get_widget (gui.xml, "server config realname");
	if (gtk_toggle_button_get_active (togglebutton))
	{
		gtk_widget_set_sensitive (nick, FALSE);
		gtk_widget_set_sensitive (real, FALSE);
	}
	else
	{
		gtk_widget_set_sensitive (nick, TRUE);
		gtk_widget_set_sensitive (real, TRUE);
	}
}

static void
encoding_changed (GtkComboBox *combo, ircnet *net)
{
	guint index;
	gchar *enc;
	if (net->encoding)
		g_free (net->encoding);
	index = gtk_combo_box_get_active (combo);
	net->encoding = g_strdup (encodings[index]);
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
	if (!((servlist_net_find (text, &position) == NULL) || (strcasecmp (realname, text) == 0)))
		/* FIXME: pop up error about duplicate */
		return;
	else if (strlen (text) == 0)
		/* FIXME: pop up error about empty */
		return;
	net->name = g_strdup (text);

	/* FIXME: check validity of everything before filling the ircnet struct */

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
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
	{
		newflags |= FLAG_USE_GLOBAL;
	}
	else
	{
		widget = glade_xml_get_widget (gui.xml, "server config nickname");
		text = (char *) gtk_entry_get_text (GTK_ENTRY (widget));
		if (strlen (text) == 0)
			/* FIXME: pop up error about empty */
			return;
		net->nick = g_strdup (text);
		widget = glade_xml_get_widget (gui.xml, "server config realname");
		text = (char *) gtk_entry_get_text (GTK_ENTRY (widget));
		if (strlen (text) == 0)
			/* FIXME: pop up error about empty */
			return;
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
edit_clicked (GtkWidget *button, gpointer data)
{
	GtkWidget *dialog, *password, *nick, *real, *encoding;
	GtkWidget *treeview, *widget;
	GtkSizeGroup *group;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GConfClient *client;
	ircnet *net;

	client = gconf_client_get_default ();

	dialog = glade_xml_get_widget (gui.xml, "server configuration");

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	password = glade_xml_get_widget (gui.xml, "server config password");
	gtk_size_group_add_widget (group, password);
	nick = glade_xml_get_widget (gui.xml, "server config nickname");
	gtk_size_group_add_widget (group, nick);
	real = glade_xml_get_widget (gui.xml, "server config realname");
	gtk_size_group_add_widget (group, real);
	encoding = glade_xml_get_widget (gui.xml, "encoding combo");
	if (!initialized)
	{
		char **enc = encodings;
		guint index = 0;

		enctoindex = g_hash_table_new (g_str_hash, g_str_equal);

		do
		{
			gtk_combo_box_append_text (GTK_COMBO_BOX (encoding), *enc);
			g_hash_table_insert (enctoindex, *enc, GUINT_TO_POINTER (index));

			index++;
			enc++;
		} while (*enc);
		initialized = TRUE;
	}
	gtk_size_group_add_widget (group, encoding);
	g_object_unref (group);

	treeview = glade_xml_get_widget (gui.xml, "configure server list");
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	gtk_tree_selection_get_selected (select, &model, &iter);
	gtk_tree_model_get (model, &iter, 2, &net, -1);

	if (net->pass != NULL)
		gtk_entry_set_text (GTK_ENTRY (password), net->pass);

	widget = glade_xml_get_widget (gui.xml, "server config usedefaults");
	if (net->flags & FLAG_USE_GLOBAL)
	{
		gchar *text;
		text = gconf_client_get_string (client, "/apps/xchat/irc/nickname", NULL);
		gtk_entry_set_text (GTK_ENTRY (nick), text);
		g_free (text);
		text = gconf_client_get_string (client, "/apps/xchat/irc/realname", NULL);
		gtk_entry_set_text (GTK_ENTRY(real), text);
		g_free (text);
		gtk_widget_set_sensitive (nick, FALSE);
		gtk_widget_set_sensitive (real, FALSE);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
	}
	else
	{
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), FALSE);
		gtk_widget_set_sensitive (nick, TRUE);
		gtk_widget_set_sensitive (real, TRUE);
		if (net->nick != NULL)
			gtk_entry_set_text (GTK_ENTRY (nick), net->nick);
		if (net->real != NULL)
			gtk_entry_set_text (GTK_ENTRY (real), net->real);
	}
	g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (edit_global_changed), NULL);

	widget = glade_xml_get_widget (gui.xml, "server config autoconnect");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), (net->flags & FLAG_AUTO_CONNECT));

	widget = glade_xml_get_widget (gui.xml, "server config ssl");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), (net->flags & FLAG_USE_SSL));

	widget = glade_xml_get_widget (gui.xml, "server config cycle");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), (net->flags & FLAG_CYCLE));

	widget = glade_xml_get_widget (gui.xml, "server config network name");
	gtk_entry_set_text (GTK_ENTRY (widget), net->name);

	widget = glade_xml_get_widget (gui.xml, "server config ok");
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (edit_ok_clicked), NULL);

	widget = glade_xml_get_widget (gui.xml, "server config cancel");
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (edit_cancel_clicked), NULL);

	widget = glade_xml_get_widget (gui.xml, "encoding combo");
	{
		guint index;
		if (net->encoding == NULL)
		{
			index = 0;
			net->encoding = g_strdup (encodings[0]);
		}
		else
		{
			index = GPOINTER_TO_UINT (g_hash_table_lookup (enctoindex, net->encoding));
		}
		gtk_combo_box_set_active (GTK_COMBO_BOX (widget), index);
	}
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (encoding_changed), net);

	gtk_widget_show_all (dialog);
}

static void
remove_clicked (GtkWidget *button, gpointer data)
{
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	ircnet *net;

	treeview = glade_xml_get_widget (gui.xml, "configure server list");

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	gtk_tree_selection_get_selected (select, &model, &iter);
/*	gtk_tree_model_get(model, &iter, 2, &net, -1);*/

	gtk_tree_store_remove (GTK_LIST_STORE (model), &iter);
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
	sort = gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (store));
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (sort), 1, GTK_SORT_DESCENDING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (sort));

	text_renderer = gtk_cell_renderer_text_new ();
	text_column = gtk_tree_view_column_new_with_attributes ("name", text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), text_column);
	autoconnect_renderer = gtk_cell_renderer_toggle_new ();
	autoconnect_column = gtk_tree_view_column_new_with_attributes ("auto-connect", autoconnect_renderer, "active", 1, NULL);
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

	while (netlist)
	{
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
	if (gtk_tree_selection_get_selected (selection, NULL, NULL))
	{
		gtk_widget_set_sensitive (edit_button, TRUE);
		gtk_widget_set_sensitive (remove_button, TRUE);
	}
	else
	{
		gtk_widget_set_sensitive (edit_button, FALSE);
		gtk_widget_set_sensitive (remove_button, FALSE);
	}
}
