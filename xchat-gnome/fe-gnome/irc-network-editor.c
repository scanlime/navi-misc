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
irc_network_editor_init (IrcNetworkEditor *dialog)
{
	GtkCellRenderer *renderer;
	GtkSizeGroup *group;

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
	GW(remove_server);

	GW(use_globals);
	GW(use_custom);
	GW(nickname);
	GW(realname);

	GW(autojoin_channels);
	GW(add_autojoin);
	GW(remove_autojoin);

	GW(toplevel);
#undef GW

	dialog->store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);
	renderer = gtk_cell_renderer_text_new ();

	gtk_tree_view_set_model (GTK_TREE_VIEW (dialog->servers), GTK_TREE_MODEL (dialog->store));
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (dialog->servers), 0, "Server", renderer, "text", 0, NULL);

	dialog->encoding = gtk_combo_box_new_text ();
	gtk_widget_show (dialog->encoding);
	gtk_box_pack_start (GTK_BOX (dialog->encoding_hbox), dialog->encoding, FALSE, TRUE, 0);

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	gtk_size_group_add_widget (group, dialog->network_name);
	gtk_size_group_add_widget (group, dialog->encoding);
	gtk_size_group_add_widget (group, dialog->password);
	g_object_unref (group);
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
use_globals_set (GtkRadioButton *button, IrcNetworkEditor *e)
{
	gchar *text;

	text = gconf_client_get_string (e->gconf, "/apps/xchat/irc/nickname", NULL);
	gtk_entry_set_text (GTK_ENTRY (e->nickname), text);
	g_free (text);

	text = gconf_client_get_string (e->gconf, "/apps/xchat/irc/realname", NULL);
	gtk_entry_set_text (GTK_ENTRY (e->realname), text);
	g_free (text);

	gtk_widget_set_sensitive (e->nickname, FALSE);
	gtk_widget_set_sensitive (e->realname, FALSE);
}

static void
use_custom_set (GtkRadioButton *button, IrcNetworkEditor *e)
{
	gtk_widget_set_sensitive (e->nickname, TRUE);
	gtk_widget_set_sensitive (e->realname, TRUE);
}

static void
populate_server_list (IrcNetworkEditor *e)
{
	GSList *s;
	GtkTreeIter iter;
	ircserver *serv;

	for (s = e->network->servers; s; s = g_slist_next (s)) {
		serv = s->data;

		gtk_list_store_append (e->store, &iter);
		gtk_list_store_set (e->store, &iter, 0, serv->hostname, 1, serv, -1);
	}
}

static void
populate_autojoin_list (IrcNetworkEditor *e)
{
	g_print ("autojoin = %s\n", e->network->autojoin);
}

static void
irc_network_editor_populate (IrcNetworkEditor *e)
{
	e->gconf = gconf_client_get_default ();

	gtk_entry_set_text           (GTK_ENTRY         (e->network_name),     e->network->name);

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

	gtk_entry_set_text (GTK_ENTRY (e->password), e->network->password);

	{
		gchar **enc = (gchar **) encodings;
		do {
			gtk_combo_box_append_text (GTK_COMBO_BOX (e->encoding), _(*enc));
			enc++;
		} while (*enc);
	}
	gtk_combo_box_set_active (GTK_COMBO_BOX (e->encoding), e->network->encoding);

	gtk_dialog_add_button (GTK_DIALOG (e), GTK_STOCK_APPLY,  GTK_RESPONSE_APPLY);
	gtk_dialog_add_button (GTK_DIALOG (e), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (GTK_DIALOG (e), GTK_STOCK_OK,     GTK_RESPONSE_OK);
	gtk_dialog_set_default_response (GTK_DIALOG (e), GTK_RESPONSE_OK);

	gtk_container_set_border_width (GTK_CONTAINER (e), 6);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(e)->vbox), e->toplevel);
	gtk_dialog_set_has_separator (GTK_DIALOG (e), FALSE);
	gtk_window_set_modal (GTK_WINDOW (e), TRUE);

	populate_server_list (e);
	populate_autojoin_list (e);

	g_signal_connect (G_OBJECT (e->use_globals), "toggled", G_CALLBACK (use_globals_set), e);
	g_signal_connect (G_OBJECT (e->use_custom),  "toggled", G_CALLBACK (use_custom_set),  e);
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
apply_changes (IrcNetworkEditor *editor)
{
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
