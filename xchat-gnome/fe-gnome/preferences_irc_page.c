/*
 * preferences_irc_page.c - helpers for the irc preferences page
 *
 * Copyright (C) 2004-2005 xchat-gnome team
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

static GtkListStore *hilight_store;
extern struct xchatprefs prefs;

static void gconf_entry_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkEntry *entry);
static void gconf_bool_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkToggleButton *button);
static void gconf_font_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkFontButton *button);
static void entry_changed (GtkEntry *entry, const gchar *key);
static void bool_changed (GtkToggleButton *button, const gchar *key);
static void font_changed (GtkFontButton *button, const gchar *key);
static void sysfonts_changed (GtkToggleButton *toggle, GtkWidget *font);
static void populate_hilight ();
static void save_hilight ();
static void hilight_add_clicked (GtkButton *button, gpointer data);
static void hilight_remove_clicked (GtkButton *button, GtkTreeView *view);
static void hilight_selection (GtkTreeSelection *selection, gpointer data);

void initialize_preferences_irc_page()
{
	GtkWidget *widget, *radiobtn;
	GtkSizeGroup *group;
	char *text;
	gboolean toggle;
	GConfClient *client;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;

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
	widget = glade_xml_get_widget (gui.xml, "highlight list");
	hilight_store = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (widget), GTK_TREE_MODEL (hilight_store));
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new ();
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (widget), column);
	populate_hilight ();
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
	g_signal_connect (G_OBJECT (selection), "changed", G_CALLBACK (hilight_selection), NULL);

	widget = glade_xml_get_widget (gui.xml, "hilight add");
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (hilight_add_clicked), NULL);
	widget = glade_xml_get_widget (gui.xml, "hilight remove");
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (hilight_remove_clicked), glade_xml_get_widget (gui.xml, "highlight list"));

	g_object_unref (group);

	widget = glade_xml_get_widget (gui.xml, "show colors");
	toggle = gconf_client_get_bool (client, "/apps/xchat/irc/showcolors", NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), toggle);
	gconf_client_notify_add (client, "/apps/xchat/irc/showcolors", (GConfClientNotifyFunc) gconf_bool_changed, widget, NULL, NULL);
	g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (bool_changed), "/apps/xchat/irc/showcolors");

	widget = glade_xml_get_widget (gui.xml, "show timestamps");
	toggle = gconf_client_get_bool (client, "/apps/xchat/irc/showtimestamps", NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), toggle);
	gconf_client_notify_add (client, "/apps/xchat/irc/showtimestamps", (GConfClientNotifyFunc) gconf_bool_changed, widget, NULL, NULL);
	g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (bool_changed), "/apps/xchat/irc/showtimestamps");

	/* Create a radio button group for the font radio buttons. */
	widget = glade_xml_get_widget (gui.xml, "usesysfonts");
	radiobtn = glade_xml_get_widget (gui.xml, "usethisfont");
	gtk_radio_button_set_group (GTK_RADIO_BUTTON(radiobtn), gtk_radio_button_get_group (GTK_RADIO_BUTTON(widget)));
	toggle = gconf_client_get_bool (client, "/apps/xchat/main_window/use_sys_fonts", NULL);
	/* Toggle the second button if necessary. */
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobtn), !toggle);
	gconf_client_notify_add (client, "/apps/xchat/main_window/use_sys_fonts", (GConfClientNotifyFunc) gconf_bool_changed, widget, NULL, NULL);
	g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (bool_changed), "/apps/xchat/main_window/use_sys_fonts");
	g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (sysfonts_changed), glade_xml_get_widget (gui.xml, "font selection"));

	widget = glade_xml_get_widget (gui.xml, "font selection");
	text = gconf_client_get_string (client, "/apps/xchat/main_window/font", NULL);
	gtk_font_button_set_font_name (GTK_FONT_BUTTON (widget), text);
	gtk_widget_set_sensitive (widget, !toggle);
	gconf_client_notify_add (client, "/apps/xchat/main-window/font", (GConfClientNotifyFunc) gconf_font_changed, NULL, NULL, NULL);
	g_signal_connect (G_OBJECT (widget), "font-set", G_CALLBACK (font_changed), "/apps/xchat/main_window/font");

	g_object_unref (client);
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
	g_object_unref (client);
}

static void
bool_changed (GtkToggleButton *button, const gchar *key)
{
	GConfClient *client;
	gboolean value;

	client = gconf_client_get_default ();
	value = gtk_toggle_button_get_active (button);
	gconf_client_set_bool (client, key, value, NULL);
	g_object_unref (client);
}

static void
font_changed (GtkFontButton *button, const gchar *key)
{
	GConfClient *client;
	const gchar *text;

	client = gconf_client_get_default ();
	text = gtk_font_button_get_font_name (button);
	gconf_client_set_string (client, key, text, NULL);
	g_object_unref (client);
}

static void
sysfonts_changed (GtkToggleButton *toggle, GtkWidget *font)
{
	gtk_widget_set_sensitive (font, !gtk_toggle_button_get_active (toggle));
}

static void
populate_hilight ()
{
	gchar **tokens = g_strsplit (prefs.bluestring, ",", 0);
	int i;
	GtkTreeIter iter;

	for (i = 0; tokens[i]; i++) {
		gtk_list_store_append (hilight_store, &iter);
		gtk_list_store_set (hilight_store, &iter, 0, tokens[i], -1);
	}

	g_strfreev (tokens);
}

static void
save_hilight ()
{
	GtkTreeIter iter;
	gchar *hilight, *tmp, *tmp2;

	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (hilight_store), &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (hilight_store), &iter, 0, &tmp, -1);
		hilight = g_strdup (tmp);
	} else {
		return;
	}
	while (gtk_tree_model_iter_next (GTK_TREE_MODEL (hilight_store), &iter)) {
		tmp2 = hilight;
		gtk_tree_model_get (GTK_TREE_MODEL (hilight_store), &iter, 0, &tmp, -1);
		hilight = g_strdup_printf ("%s,%s", tmp2, tmp);
		g_free (tmp2);
	}
	strncpy (prefs.bluestring, hilight, 300);
	g_free (hilight);
}

static void
hilight_add_clicked (GtkButton *button, gpointer data)
{
	GtkDialog *dialog;
	GtkWidget *text;
	gint response;
	GtkTreeIter iter;

	dialog = GTK_DIALOG (gtk_dialog_new_with_buttons ("Add Highlight Trigger",
			GTK_WINDOW (gui.main_window),
			GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OK, GTK_RESPONSE_OK,
			NULL));
	text = gtk_entry_new ();
	gtk_widget_show (text);
	gtk_container_add (GTK_CONTAINER (dialog->vbox), text);
	gtk_box_set_spacing (GTK_BOX (dialog->vbox), 12);
	gtk_container_set_border_width (GTK_CONTAINER (dialog), 12);
	response = gtk_dialog_run (dialog);
	if (response == GTK_RESPONSE_OK) {
		gtk_list_store_append (hilight_store, &iter);
		gtk_list_store_set (hilight_store, &iter, 0, g_strdup (gtk_entry_get_text (GTK_ENTRY (text))), -1);
		save_hilight ();
	}
	gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
hilight_remove_clicked (GtkButton *button, GtkTreeView *view)
{
	GtkTreeSelection *selection;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
	if (gtk_tree_selection_get_selected (selection, (GtkTreeModel**) &hilight_store, &iter)) {
		gtk_list_store_remove (hilight_store, &iter);
		save_hilight ();
	}
}

static void
hilight_selection (GtkTreeSelection *selection, gpointer data)
{
	GtkWidget *remove = glade_xml_get_widget (gui.xml, "hilight remove");
	gtk_widget_set_sensitive (remove, gtk_tree_selection_get_selected (selection, (GtkTreeModel**) &hilight_store, NULL));
}
