/*
 * preferences_irc_page.c - helpers for the irc preferences page
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
#include "preferences_irc_page.h"

static void gconf_nickname_changed (GConfClient *client, guint cnxn_id, const gchar* key, GConfValue* value, gboolean is_default, gpointer data);
static void entry_nickname_changed (GtkEntry *entry, gpointer data);

void initialize_preferences_irc_page()
{
	GtkWidget *widget;
	GtkSizeGroup *group;
	char *text;
	GConfClient *client;

	client = gconf_client_get_default ();

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

	widget = glade_xml_get_widget (gui.xml, "nick name");
	text = gconf_client_get_string (client, "/apps/xchat/irc/nickname", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/nickname", (GConfClientNotifyFunc) gconf_nickname_changed, NULL, NULL, NULL);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_nickname_changed), NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "real name");
	text = gconf_client_get_string (client, "/apps/xchat/irc/realname", NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "quit message");
	text = gconf_client_get_string (client, "/apps/xchat/irc/quitmsg", NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "part message");
	text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "away message");
	text = preferences_awaymsg ();
	text = gconf_client_get_string (client, "/apps/xchat/irc/awaymsg", NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "highlight list container");
	gtk_size_group_add_widget (group, widget);

	g_object_unref (group);

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

	widget = glade_xml_get_widget (gui.xml, "hilight add");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (gui.xml, "hilight remove");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (gui.xml, "font selection");
	gtk_size_group_add_widget (group, widget);

	g_object_unref (group);
}

static void
gconf_nickname_changed (GConfClient *client, guint cnxn_id, const gchar* key, GConfValue* value, gboolean is_default, gpointer data)
{
	GtkWidget *widget;
	gchar *text;

	g_print ("hello!\n");

	widget = glade_xml_get_widget (gui.xml, "nick name");
	g_signal_handlers_block_by_func (widget, "changed", entry_nickname_changed);
	text = gconf_client_get_string (client, "/apps/xchat/irc/nickname", NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_handlers_unblock_by_func (widget, "changed", entry_nickname_changed);
	g_free (text);
}

static void
entry_nickname_changed (GtkEntry *entry, gpointer data)
{
	GConfClient *client;
	const gchar *text;

	client = gconf_client_get_default ();
	text = gtk_entry_get_text (entry);
	if (text)
		gconf_client_set_string (client, "/apps/xchat/irc/nickname", text, NULL);
}
