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

	GW(toplevel);
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
	gtk_combo_box_set_active (GTK_COMBO_BOX (e->encoding), e->network->encoding);

	gtk_dialog_add_button (GTK_DIALOG (e), GTK_STOCK_APPLY,  GTK_RESPONSE_APPLY);
	gtk_dialog_add_button (GTK_DIALOG (e), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (GTK_DIALOG (e), GTK_STOCK_OK,     GTK_RESPONSE_OK);

	gtk_container_set_border_width (GTK_CONTAINER (e), 6);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(e)->vbox), e->toplevel);
	gtk_dialog_set_has_separator (GTK_DIALOG (e), FALSE);
	gtk_window_set_modal (GTK_WINDOW (e), TRUE);

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

void
irc_network_editor_run (IrcNetworkEditor *editor)
{
	gint response;

	response = gtk_dialog_run (GTK_DIALOG (editor));
	gtk_widget_hide (GTK_WIDGET (editor));
}
