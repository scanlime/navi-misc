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
irc_network_editor_class_init (IrcNetworkEditorClass *klass)
{
}

static void
irc_network_editor_init (IrcNetworkEditor *dialog)
{
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
	GW(encoding);

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
#undef GW
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
irc_network_editor_populate (IrcNetworkEditor *e)
{
	g_print ("name = 0x%x\n", e->network->name);
	gtk_entry_set_text           (GTK_ENTRY         (e->network_name),     e->network->name);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->autoconnect),      e->network->autoconnect);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->use_ssl),          e->network->use_ssl);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->cycle),            e->network->cycle);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->autoreconnect),    e->network->reconnect);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->giveup_reconnect), e->network->nogiveup_reconnect);

	gtk_entry_set_text           (GTK_ENTRY         (e->password),         e->network->password);
	gtk_combo_box_set_active     (GTK_COMBO_BOX     (e->encoding),         e->network->encoding);

	if (e->network->use_global)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->use_globals), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->use_custom), TRUE);

	gtk_dialog_add_button (GTK_DIALOG (e), GTK_STOCK_APPLY,  GTK_RESPONSE_APPLY);
	gtk_dialog_add_button (GTK_DIALOG (e), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (GTK_DIALOG (e), GTK_STOCK_OK,     GTK_RESPONSE_OK);
}

IrcNetworkEditor *
irc_network_editor_new (IrcNetwork *network)
{
	//IrcNetworkEditor *e = IRC_NETWORK_EDITOR (g_object_new (irc_network_editor_get_type (), 0));
	IrcNetworkEditor *e = g_object_new (irc_network_editor_get_type (), 0);

	e->network = network;
	irc_network_editor_populate (e);

	return e;
}

void
irc_network_editor_run (IrcNetworkEditor *editor)
{
	gint response;

	response = gtk_dialog_run (editor);
	gtk_widget_hide (editor);
}
