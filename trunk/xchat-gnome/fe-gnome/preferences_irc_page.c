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

static void gconf_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkEntry *entry);
static void entry_changed (GtkEntry *entry, const gchar *key);

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
	gconf_client_notify_add (client, "/apps/xchat/irc/nickname", (GConfClientNotifyFunc) gconf_changed, NULL, NULL, NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_changed), "/apps/xchat/irc/nickname");
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "real name");
	text = gconf_client_get_string (client, "/apps/xchat/irc/realname", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/realname", (GConfClientNotifyFunc) gconf_changed, NULL, NULL, NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_changed), "/apps/xchat/irc/realname");
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "quit message");
	text = gconf_client_get_string (client, "/apps/xchat/irc/quitmsg", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/quitmsg", (GConfClientNotifyFunc) gconf_changed, NULL, NULL, NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_changed), "/apps/xchat/irc/quitmsg");
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "part message");
	text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/partmsg", (GConfClientNotifyFunc) gconf_changed, NULL, NULL, NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_changed), "/apps/xchat/irc/partmsg");
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "away message");
	text = gconf_client_get_string (client, "/apps/xchat/irc/awaymsg", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/partmsg", (GConfClientNotifyFunc) gconf_changed, NULL, NULL, NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_changed), "/apps/xchat/irc/awaymsg");
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
gconf_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkEntry *entry)
{
	gchar *text;

	g_signal_handlers_block_by_func (entry, "changed", entry_changed);
	text = gconf_client_get_string (client, key, NULL);
	gtk_entry_set_text (entry, text);
	g_free (text);
	g_signal_handlers_block_by_func (entry, "changed", entry_changed);
}

static void
entry_changed (GtkEntry *entry, const gchar *key)
{
	GConfClient *client;
	const gchar *text;

	client = gconf_client_get_default ();
	text = gtk_entry_get_text (entry);
	if (text)
		gconf_client_set_string (client, key, text, NULL);
}
