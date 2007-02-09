/*
 * setup-dialog.c - Initial setup dialog for first-time users
 *
 * Copyright (C) 2004-2007 xchat-gnome team
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

#include <config.h>
#include <glib/gi18n.h>
#include <gconf/gconf-client.h>
#include <glade/glade-xml.h>
#include <gtk/gtk.h>
#include <string.h>
#include "setup-dialog.h"
#include "util.h"
#include "preferences.h"

static GladeXML *xml = NULL;
static gboolean done;

static void ok_clicked (GtkButton *button, gpointer data);
static void entry_changed (GtkEditable *entry, gpointer user_data);

void run_setup_dialog (void)
{
	GtkSizeGroup *group;
	GtkWidget *window;
	GtkWidget *nick_entry;
	GtkWidget *real_entry;
	GtkWidget *button;
	gchar *path;

	path = locate_data_file ("setup-dialog.glade");
	xml = glade_xml_new (path, NULL, NULL);
	g_free (path);
	if (!xml) {
		/* FIXME - if we can't do this, we've got major problems */
		return;
	}

	window = glade_xml_get_widget (xml, "setup window");

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	nick_entry = glade_xml_get_widget (xml, "nick name entry");
	real_entry = glade_xml_get_widget (xml, "real name entry");
	gtk_size_group_add_widget (group, nick_entry);
	gtk_size_group_add_widget (group, real_entry);
	g_signal_connect (G_OBJECT (nick_entry), "changed", G_CALLBACK (entry_changed), NULL);
	g_signal_connect (G_OBJECT (real_entry), "changed", G_CALLBACK (entry_changed), NULL);
	g_object_unref (group);

	gtk_entry_set_text (GTK_ENTRY (nick_entry), g_get_user_name ());
	gtk_entry_set_text (GTK_ENTRY (real_entry), g_get_real_name ());
	gtk_widget_grab_focus (nick_entry);

	button = glade_xml_get_widget (xml, "ok button");
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (ok_clicked), NULL);

	gtk_widget_show_all (window);
	done = FALSE;
	while (!done) {
		g_main_context_iteration (NULL, TRUE);
	}

	g_object_unref (xml);
	gtk_widget_destroy (window);
}

static void
ok_clicked (GtkButton *button, gpointer data)
{
	GtkWidget *nick_entry = glade_xml_get_widget (xml, "nick name entry");
	GtkWidget *real_entry = glade_xml_get_widget (xml, "real name entry");
	const gchar *nick = gtk_entry_get_text (GTK_ENTRY (nick_entry));
	const gchar *real = gtk_entry_get_text (GTK_ENTRY (real_entry));

	GConfClient *client = gconf_client_get_default ();

	gconf_client_set_string (client, "/apps/xchat/irc/nickname", nick, NULL);
	gconf_client_set_string (client, "/apps/xchat/irc/realname", real, NULL);

	gconf_client_set_string (client, "/apps/xchat/version", PACKAGE_VERSION, NULL);

	g_object_unref (client);

	/*
	 * We set the alternative nicknames here, so that people editing the
	 * config file can override them.
	 */
	set_nickname(nick);

	done = TRUE;
}

static void
entry_changed (GtkEditable *entry, gpointer user_data)
{
	GtkWidget *ok_button, *nick_entry, *real_entry;
	const gchar *nick, *real;

	ok_button = glade_xml_get_widget (xml, "ok button");
	nick_entry = glade_xml_get_widget (xml, "nick name entry");
	real_entry = glade_xml_get_widget (xml, "real name entry");
	nick = gtk_entry_get_text (GTK_ENTRY (nick_entry));
	real = gtk_entry_get_text (GTK_ENTRY (real_entry));

	if (strlen(nick) == 0 || strlen(real) == 0) {
		gtk_widget_set_sensitive (ok_button, FALSE);
	} else {
		gtk_widget_set_sensitive (ok_button, TRUE);
	}
}
