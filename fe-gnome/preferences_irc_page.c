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

#include "preferences_irc_page.h"
#include "preferences.h"

void initialize_preferences_irc_page() {
	GtkWidget *widget;
	GtkSizeGroup *group;
	char *text;

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

	widget = glade_xml_get_widget(gui.xml, "nick name");
	text = preferences_nickname();
	gtk_entry_set_text(GTK_ENTRY(widget), text);
	g_free(text);
	gtk_size_group_add_widget(group, widget);

	widget = glade_xml_get_widget(gui.xml, "real name");
	text = preferences_realname();
	gtk_entry_set_text(GTK_ENTRY(widget), text);
	g_free(text);
	gtk_size_group_add_widget(group, widget);

	widget = glade_xml_get_widget(gui.xml, "quit message");
	text = preferences_quitmsg();
	gtk_entry_set_text(GTK_ENTRY(widget), text);
	g_free(text);
	gtk_size_group_add_widget(group, widget);

	widget = glade_xml_get_widget(gui.xml, "part message");
	text = preferences_partmsg();
	gtk_entry_set_text(GTK_ENTRY(widget), text);
	g_free(text);
	gtk_size_group_add_widget(group, widget);

	widget = glade_xml_get_widget(gui.xml, "away message");
	text = preferences_awaymsg();
	gtk_entry_set_text(GTK_ENTRY(widget), text);
	g_free(text);
	gtk_size_group_add_widget(group, widget);

	widget = glade_xml_get_widget(gui.xml, "highlight list container");
	gtk_size_group_add_widget(group, widget);

	g_object_unref(group);
}
