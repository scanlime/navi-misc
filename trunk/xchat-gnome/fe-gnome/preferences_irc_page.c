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

static void gconf_entry_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkEntry *entry);
static void gconf_bool_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkToggleButton *button);
static void gconf_font_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkFontButton *button);
static void entry_changed (GtkEntry *entry, const gchar *key);
static void bool_changed (GtkToggleButton *button, const gchar *key);
static void font_changed (GtkFontButton *button, const gchar *key);

void initialize_preferences_irc_page()
{
	GtkWidget *widget;
	GtkSizeGroup *group;
	char *text;
	gboolean toggle;
	GConfClient *client;

	client = gconf_client_get_default ();

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

	widget = glade_xml_get_widget (gui.xml, "nick name");
	text = gconf_client_get_string (client, "/apps/xchat/irc/nickname", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/nickname", (GConfClientNotifyFunc) gconf_entry_changed, NULL, NULL, NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_changed), "/apps/xchat/irc/nickname");
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "real name");
	text = gconf_client_get_string (client, "/apps/xchat/irc/realname", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/realname", (GConfClientNotifyFunc) gconf_entry_changed, NULL, NULL, NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_changed), "/apps/xchat/irc/realname");
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "quit message");
	text = gconf_client_get_string (client, "/apps/xchat/irc/quitmsg", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/quitmsg", (GConfClientNotifyFunc) gconf_entry_changed, NULL, NULL, NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_changed), "/apps/xchat/irc/quitmsg");
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "part message");
	text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/partmsg", (GConfClientNotifyFunc) gconf_entry_changed, NULL, NULL, NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_changed), "/apps/xchat/irc/partmsg");
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "away message");
	text = gconf_client_get_string (client, "/apps/xchat/irc/awaymsg", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/partmsg", (GConfClientNotifyFunc) gconf_entry_changed, NULL, NULL, NULL);
	gtk_entry_set_text (GTK_ENTRY (widget), text);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (entry_changed), "/apps/xchat/irc/awaymsg");
	g_free (text);
	gtk_size_group_add_widget (group, widget);

	widget = glade_xml_get_widget (gui.xml, "highlight list container");
	gtk_size_group_add_widget (group, widget);

	g_object_unref (group);

	widget = glade_xml_get_widget (gui.xml, "usesysfonts");
	toggle = gconf_client_get_bool (client, "/apps/xchat/main_window/use_sys_fonts", NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), toggle);
	gconf_client_notify_add (client, "/apps/xchat/main_window/use_sys_fonts", (GConfClientNotifyFunc) gconf_bool_changed, NULL, NULL, NULL);
	g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (bool_changed), "/apps/xchat/main_window/use_sys_fonts");

	widget = glade_xml_get_widget (gui.xml, "font selection");
	text = gconf_client_get_string (client, "/apps/xchat/main_window/font", NULL);
	gtk_font_button_set_font_name (GTK_FONT_BUTTON (widget), text);
	if (toggle)
		gtk_widget_set_sensitive (widget, FALSE);
	else
		gtk_widget_set_sensitive (widget, TRUE);
	gconf_client_notify_add (client, "/apps/xchat/main-window/font", (GConfClientNotifyFunc) gconf_font_changed, NULL, NULL, NULL);
	g_signal_connect (G_OBJECT (widget), "font-set", G_CALLBACK (font_changed), "/apps/xchat/main_window/font");
}

static void
gconf_entry_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkEntry *entry)
{
	gchar *text;

	g_signal_handlers_block_by_func (entry, "changed", entry_changed);
	text = gconf_client_get_string (client, key, NULL);
	gtk_entry_set_text (entry, text);
	g_free (text);
	g_signal_handlers_unblock_by_func (entry, "changed", entry_changed);
}

static void
gconf_bool_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkToggleButton *button)
{
	gboolean toggle;

	g_signal_handlers_block_by_func (button, "toggled", bool_changed);
	toggle = gconf_client_get_bool (client, key, NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), toggle);
	g_signal_handlers_unblock_by_func (button, "toggled", bool_changed);
}

static void
gconf_font_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkFontButton *button)
{
	gchar *text;

	g_signal_handlers_block_by_func (button, "font-set", font_changed);
	text = gconf_client_get_string (client, key, NULL);
	gtk_font_button_set_font_name (button, text);
	g_free (text);
	g_signal_handlers_unblock_by_func (button, "font-set", font_changed);
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

static void
bool_changed (GtkToggleButton *button, const gchar *key)
{
	GConfClient *client;
	gboolean value;

	client = gconf_client_get_default ();
	value = gtk_toggle_button_get_active (button);
	gconf_client_set_bool (client, key, value, NULL);
}

static void
font_changed (GtkFontButton *button, const gchar *key)
{
	GConfClient *client;
	const gchar *text;

	client = gconf_client_get_default ();
	text = gtk_font_button_get_font_name (button);
	gconf_client_set_string (client, key, text, NULL);
}
