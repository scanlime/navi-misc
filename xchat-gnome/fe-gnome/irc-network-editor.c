/*
 * irc-network-editor.h - GtkDialog subclass for editing an IrcNetwork
 *
 * Copyright (C) 2005 David Trowbridge
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

#include "irc-network-editor.h"
#include <gtk/gtk.h>

static void
irc_network_editor_dispose (GObject *object)
{
	IrcNetworkEditor *e = (IrcNetworkEditor *) object;

	if (e->gconf) {
		g_object_unref (e->gconf);
		e->gconf = NULL;
	}
	if (e->network) {
		g_object_unref (e->network);
		e->network = NULL;
	}
}

static void
irc_network_editor_class_init (IrcNetworkEditorClass *klass)
{
	GObjectClass *parent_class = (GObjectClass *) klass;
	parent_class->dispose = irc_network_editor_dispose;
}

static void
use_globals_set (GtkRadioButton *button, IrcNetworkEditor *e)
{
	gchar *text;

	text = gconf_client_get_string (e->gconf, "/apps/xchat/irc/nickname", NULL);
	gtk_entry_set_text (GTK_ENTRY (e->nickname), text);
	g_free (text);

	text = gconf_client_get_string (e->gconf, "/apps/xchat/irc/realname", NULL);
	gtk_entry_set_text (GTK_ENTRY (e->realname), text);
	g_free (text);

	gtk_widget_set_sensitive (GTK_WIDGET (e->nickname), FALSE);
	gtk_widget_set_sensitive (GTK_WIDGET (e->realname), FALSE);
}

static void
use_custom_set (GtkRadioButton *button, IrcNetworkEditor *e)
{
	gtk_widget_set_sensitive (e->nickname, TRUE);
	gtk_widget_set_sensitive (e->realname, TRUE);
}

static void
edit_server_clicked (GtkButton *button, IrcNetworkEditor *e)
{
	gtk_cell_editable_start_editing (GTK_CELL_EDITABLE (e->server_renderer), NULL);
}

static void
server_selection_changed (GtkTreeSelection *selection, IrcNetworkEditor *e)
{
	if (gtk_tree_selection_get_selected (selection, NULL, NULL)) {
		gtk_widget_set_sensitive (e->edit_server, TRUE);
		gtk_widget_set_sensitive (e->remove_server, TRUE);
	} else {
		gtk_widget_set_sensitive (e->edit_server, FALSE);
		gtk_widget_set_sensitive (e->remove_server, FALSE);
	}
}

static void
server_edited (GtkCellRendererText *renderer, gchar *arg1, gchar *newtext, IrcNetworkEditor *e)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (e->servers));
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, newtext, -1);
}

static void
irc_network_editor_init (IrcNetworkEditor *dialog)
{
	GtkSizeGroup *group;
	gchar **enc;
	GtkTreeSelection *selection;

	dialog->gconf = NULL;
	dialog->network = NULL;

	dialog->xml = NULL;
	if (g_file_test ("irc-network-editor.glade", G_FILE_TEST_EXISTS))
		dialog->xml = glade_xml_new ("irc-network-editor.glade", "toplevel", NULL);
	if (!dialog->xml)
		dialog->xml = glade_xml_new (XCHATSHAREDIR "/irc-network-editor.glade", "toplevel", NULL);
	if (!dialog->xml)
		return;

#define GW(name) ((dialog->name) = glade_xml_get_widget (dialog->xml, #name))
	GW(network_name);

	GW(autoconnect);
	GW(use_ssl);
	GW(cycle);
	GW(autoreconnect);
	GW(giveup_reconnect);

	GW(password);
	GW(encoding_hbox);

	GW(servers);
	GW(add_server);
	GW(edit_server);
	GW(remove_server);

	GW(use_globals);
	GW(use_custom);
	GW(nickname);
	GW(realname);

	GW(autojoin_channels);
	GW(add_autojoin);
	GW(edit_autojoin);
	GW(remove_autojoin);

	GW(toplevel);
#undef GW

	dialog->server_store = gtk_list_store_new (1, G_TYPE_STRING);
	dialog->autojoin_store = gtk_list_store_new (1, G_TYPE_STRING);
	dialog->server_renderer = gtk_cell_renderer_text_new ();
	dialog->autojoin_renderer = gtk_cell_renderer_text_new ();

	gtk_tree_view_set_model (GTK_TREE_VIEW (dialog->servers), GTK_TREE_MODEL (dialog->server_store));
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (dialog->servers), 0, "Server", dialog->server_renderer, "text", 0, NULL);
	g_object_set (G_OBJECT (dialog->server_renderer), "editable", TRUE, NULL);

	gtk_tree_view_set_model (GTK_TREE_VIEW (dialog->autojoin_channels), GTK_TREE_MODEL (dialog->autojoin_store));
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (dialog->autojoin_channels), 0, "Channel", dialog->autojoin_renderer, "text", 0, NULL);
	g_object_set (G_OBJECT (dialog->autojoin_renderer), "editable", TRUE, NULL);

	dialog->encoding = gtk_combo_box_new_text ();
	gtk_widget_show (dialog->encoding);
	gtk_box_pack_start (GTK_BOX (dialog->encoding_hbox), dialog->encoding, FALSE, TRUE, 0);

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	gtk_size_group_add_widget (group, dialog->network_name);
	gtk_size_group_add_widget (group, dialog->encoding);
	gtk_size_group_add_widget (group, dialog->password);
	g_object_unref (group);

	enc = (gchar **) encodings;
	do {
		gtk_combo_box_append_text (GTK_COMBO_BOX (dialog->encoding), _(*enc));
		enc++;
	} while (*enc);

	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_APPLY,  GTK_RESPONSE_APPLY);
	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_OK,     GTK_RESPONSE_OK);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

	gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), dialog->toplevel);
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (dialog->servers));
	g_signal_connect (G_OBJECT (dialog->use_globals),     "toggled", G_CALLBACK (use_globals_set),          dialog);
	g_signal_connect (G_OBJECT (dialog->use_custom),      "toggled", G_CALLBACK (use_custom_set),           dialog);
	g_signal_connect (G_OBJECT (dialog->edit_server),     "clicked", G_CALLBACK (edit_server_clicked),      dialog);
	g_signal_connect (G_OBJECT (selection),               "changed", G_CALLBACK (server_selection_changed), dialog);
	g_signal_connect (G_OBJECT (dialog->server_renderer), "edited",  G_CALLBACK (server_edited),            dialog);
}

GType
irc_network_editor_get_type (void)
{
	static GType irc_network_editor_type = 0;
	if (!irc_network_editor_type) {
		static const GTypeInfo irc_network_editor_info = {
			sizeof (IrcNetworkEditorClass),
			NULL, NULL,
			(GClassInitFunc) irc_network_editor_class_init,
			NULL, NULL,
			sizeof (IrcNetworkEditor),
			0,
			(GInstanceInitFunc) irc_network_editor_init,
		};

		irc_network_editor_type = g_type_register_static (GTK_TYPE_DIALOG, "IrcNetworkEditor", &irc_network_editor_info, 0);
	}

	return irc_network_editor_type;
}

static void
populate_server_list (IrcNetworkEditor *e)
{
	GSList *s;
	GtkTreeIter iter;
	ircserver *serv;

	for (s = e->network->servers; s; s = g_slist_next (s)) {
		serv = s->data;

		gtk_list_store_append (e->server_store, &iter);
		gtk_list_store_set (e->server_store, &iter, 0, serv->hostname, -1);
	}
}

static void
populate_autojoin_list (IrcNetworkEditor *e)
{
	GtkTreeIter iter;
	gchar **channels;
	gint i;

	channels = g_strsplit (e->network->autojoin, ",", 0);
	for (i = 0; channels[i]; i++) {
		gtk_list_store_append (e->autojoin_store, &iter);
		gtk_list_store_set (e->autojoin_store, &iter, 0, channels[i]);
	}

	g_strfreev (channels);
}

static void
irc_network_editor_populate (IrcNetworkEditor *e)
{
	gchar *title;

	e->gconf = gconf_client_get_default ();

	title = g_strdup_printf ("%s Network Properties", e->network->name);
	gtk_window_set_title (GTK_WINDOW (e), title);
	g_free (title);

	gtk_entry_set_text (GTK_ENTRY (e->network_name), e->network->name);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->autoconnect),      e->network->autoconnect);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->use_ssl),          e->network->use_ssl);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->cycle),            e->network->cycle);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->autoreconnect),    e->network->reconnect);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->giveup_reconnect), e->network->nogiveup_reconnect);

	if (e->network->use_global) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->use_globals), TRUE);
		use_globals_set (GTK_RADIO_BUTTON (e->use_globals), e);
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->use_custom), TRUE);
		use_custom_set (GTK_RADIO_BUTTON (e->use_custom), e);

		gtk_entry_set_text (GTK_ENTRY (e->nickname), e->network->nick);
		gtk_entry_set_text (GTK_ENTRY (e->realname), e->network->real);
	}

	gtk_widget_set_sensitive (e->edit_server, FALSE);
	gtk_widget_set_sensitive (e->remove_server, FALSE);
	gtk_widget_set_sensitive (e->edit_autojoin, FALSE);
	gtk_widget_set_sensitive (e->remove_autojoin, FALSE);

	gtk_entry_set_text (GTK_ENTRY (e->password), e->network->password);

	gtk_combo_box_set_active (GTK_COMBO_BOX (e->encoding), e->network->encoding);

	populate_server_list (e);
	populate_autojoin_list (e);
}

IrcNetworkEditor *
irc_network_editor_new (IrcNetwork *network)
{
	IrcNetworkEditor *e = g_object_new (irc_network_editor_get_type (), 0);
	if (e->xml == NULL) {
		g_object_unref (e);
		return NULL;
	}

	e->network = g_object_ref (network);
	irc_network_editor_populate (e);

	return e;
}

static void
apply_changes (IrcNetworkEditor *e)
{
	IrcNetwork *net;
	GSList *s;
	GtkTreeIter iter;

	net = e->network;

	if (net->name)     g_free (net->name);
	if (net->password) g_free (net->password);
	if (net->nick)     g_free (net->nick);
	if (net->real)     g_free (net->real);
	if (net->autojoin) g_free (net->autojoin);

	net->name     = g_strdup (gtk_entry_get_text (GTK_ENTRY (e->network_name)));
	net->password = g_strdup (gtk_entry_get_text (GTK_ENTRY (e->password)));
	net->nick     = g_strdup (gtk_entry_get_text (GTK_ENTRY (e->nickname)));
	net->real     = g_strdup (gtk_entry_get_text (GTK_ENTRY (e->realname)));
	net->autojoin = g_strdup ("");

	net->autoconnect        = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->autoconnect));
	net->use_ssl            = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->use_ssl));
	net->cycle              = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->cycle));
	net->reconnect          = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->autoreconnect));
	net->nogiveup_reconnect = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->giveup_reconnect));
	net->use_global         = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->use_globals));

	for (s = net->servers; s; s = g_slist_next (s)) {
		ircserver *serv = s->data;
		g_free (serv->hostname);
		g_free (serv);
	}
	g_slist_free (s);
	s = NULL;
	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (e->server_store), &iter)) {
		do {
			char *text;
			ircserver *serv = g_new0 (ircserver, 1);
			gtk_tree_model_get (GTK_TREE_MODEL (e->server_store), &iter, 0, &text, -1);
			serv->hostname = g_strdup (text);
		} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (e->server_store), &iter));
	}

	irc_network_save (net);
}

void
irc_network_editor_run (IrcNetworkEditor *editor)
{
	gint response;

	response = gtk_dialog_run (GTK_DIALOG (editor));
	while (response == GTK_RESPONSE_APPLY) {
		apply_changes (editor);
		response = gtk_dialog_run (GTK_DIALOG (editor));
	}
	if (response == GTK_RESPONSE_OK)
		apply_changes (editor);
	gtk_widget_hide (GTK_WIDGET (editor));
}
