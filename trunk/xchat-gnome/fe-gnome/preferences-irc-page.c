/*
 * preferences-irc-page.c - helpers for the irc preferences page
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
#include "preferences-irc-page.h"
#include "preferences-dialog.h"

extern struct xchatprefs prefs;

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
gconf_entry_changed (GConfClient *client, guint cnxn_id,  GConfEntry *entry, GtkEntry *gtkentry)
{
	gchar *text;

	g_signal_handlers_block_by_func (gtkentry, "changed", entry_changed);
	text = gconf_client_get_string (client, entry->key, NULL);
	gtk_entry_set_text (gtkentry, text);
	g_free (text);
	g_signal_handlers_unblock_by_func (gtkentry, "changed", entry_changed);
}

static void
gconf_bool_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, GtkToggleButton *button)
{
	gboolean toggle;

	g_signal_handlers_block_by_func (button, "toggled", bool_changed);
	toggle = gconf_client_get_bool (client, entry->key, NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), toggle);
	g_signal_handlers_unblock_by_func (button, "toggled", bool_changed);
}

static void
gconf_font_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, GtkFontButton *button)
{
	gchar *text;

	g_signal_handlers_block_by_func (button, "font-set", font_changed);
	text = gconf_client_get_string (client, entry->key, NULL);
	gtk_font_button_set_font_name (button, text);
	g_free (text);
	g_signal_handlers_unblock_by_func (button, "font-set", font_changed);
}

static void
sysfonts_changed (GtkToggleButton *toggle, GtkWidget *font)
{
	gtk_widget_set_sensitive (font, !gtk_toggle_button_get_active (toggle));
}

static void
highlight_selection_changed (GtkTreeSelection *select, PreferencesIrcPage *page)
{
	if (gtk_tree_selection_get_selected (select, NULL, NULL)) {
		gtk_widget_set_sensitive (page->highlight_edit, TRUE);
		gtk_widget_set_sensitive (page->highlight_remove, TRUE);
	} else {
		gtk_widget_set_sensitive (page->highlight_edit, FALSE);
		gtk_widget_set_sensitive (page->highlight_remove, FALSE);
	}
}

static void
save_highlight (PreferencesIrcPage *page)
{
	GtkTreeIter iter;
	gchar *highlight, *tmp1, *tmp2;
	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (page->highlight_store), &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (page->highlight_store), &iter, 0, &tmp1, -1);
		highlight = g_strdup (tmp1);
	} else {
		prefs.bluestring[0] = '\0';
		return;
	}
	while (gtk_tree_model_iter_next (GTK_TREE_MODEL (page->highlight_store), &iter)) {
		tmp2 = highlight;
		gtk_tree_model_get (GTK_TREE_MODEL (page->highlight_store), &iter, 0, &tmp1, -1);
		highlight = g_strdup_printf ("%s,%s", tmp2, tmp1);
		g_free (tmp2);
	}
	strncpy (prefs.bluestring, highlight, 300);
	g_free (highlight);
}

static void
highlight_add (GtkButton *button, PreferencesIrcPage *page)
{
	GtkTreeIter iter;
	GtkTreePath *path;

	gtk_list_store_append (page->highlight_store, &iter);
	path = gtk_tree_model_get_path (GTK_TREE_MODEL (page->highlight_store), &iter);
	gtk_tree_view_set_cursor (GTK_TREE_VIEW (page->highlight_list), path, page->highlight_column, TRUE);
	gtk_tree_path_free (path);
}

static void
highlight_edit (GtkButton *button, PreferencesIrcPage *page)
{
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreePath *path;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->highlight_list));
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		path = gtk_tree_model_get_path (GTK_TREE_MODEL (page->highlight_store), &iter);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (page->highlight_list), path, page->highlight_column, TRUE);
		gtk_tree_path_free (path);
	}
}

static void
highlight_remove (GtkButton *button, PreferencesIrcPage *page)
{
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->highlight_list));
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
		save_highlight (page);
	}
}

static void
highlight_edited (GtkCellRendererText *renderer, gchar *arg1, gchar *newtext, PreferencesIrcPage *page)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->highlight_list));
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		if (strlen (newtext))
			gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, newtext, -1);
		else
			gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
		save_highlight (page);
	}
}

PreferencesIrcPage *
preferences_page_irc_new (gpointer prefs_dialog, GladeXML *xml)
{
	PreferencesIrcPage *page = g_new0 (PreferencesIrcPage, 1);
	PreferencesDialog *p = (PreferencesDialog *) prefs_dialog;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	gchar *text;
	gboolean toggle;
	GtkSizeGroup *group;
	GtkTreeSelection *select;
	gchar **highlight_entries;
	gint i;

#define GW(name) ((page->name) = glade_xml_get_widget (xml, #name))
	GW(nick_name);
	GW(real_name);
	GW(quit_message);
	GW(part_message);
	GW(away_message);

	GW(highlight_list);
	GW(highlight_add);
	GW(highlight_edit);
	GW(highlight_remove);

	GW(usesysfonts);
	GW(usethisfont);
	GW(font_selection);

	GW(show_colors);
	GW(show_timestamps);
#undef GW

	page->icon = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/irc.png", NULL);
	gtk_list_store_append (p->page_store, &iter);
	gtk_list_store_set (p->page_store, &iter, 0, page->icon, 1, "IRC Preferences", 2, 0, -1);

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	gtk_size_group_add_widget (group, page->nick_name);
	gtk_size_group_add_widget (group, page->real_name);
	gtk_size_group_add_widget (group, page->quit_message);
	gtk_size_group_add_widget (group, page->part_message);
	gtk_size_group_add_widget (group, page->away_message);
	gtk_size_group_add_widget (group, page->font_selection);
	g_object_unref (group);

	g_signal_connect (G_OBJECT (page->nick_name),        "changed",  G_CALLBACK (entry_changed),    "/apps/xchat/irc/nickname");
	g_signal_connect (G_OBJECT (page->real_name),        "changed",  G_CALLBACK (entry_changed),    "/apps/xchat/irc/realname");
	g_signal_connect (G_OBJECT (page->quit_message),     "changed",  G_CALLBACK (entry_changed),    "/apps/xchat/irc/quitmsg");
	g_signal_connect (G_OBJECT (page->part_message),     "changed",  G_CALLBACK (entry_changed),    "/apps/xchat/irc/partmsg");
	g_signal_connect (G_OBJECT (page->away_message),     "changed",  G_CALLBACK (entry_changed),    "/apps/xchat/irc/awaymsg");
	g_signal_connect (G_OBJECT (page->usesysfonts),      "toggled",  G_CALLBACK (bool_changed),     "/apps/xchat/main_window/use_sys_fonts");
	g_signal_connect (G_OBJECT (page->usesysfonts),      "toggled",  G_CALLBACK (sysfonts_changed), NULL);
	g_signal_connect (G_OBJECT (page->font_selection),   "font-set", G_CALLBACK (font_changed),     "/apps/xchat/main_window/font");
	g_signal_connect (G_OBJECT (page->show_colors),      "toggled",  G_CALLBACK (bool_changed),     "/apps/xchat/irc/showcolors");
	g_signal_connect (G_OBJECT (page->show_timestamps),  "toggled",  G_CALLBACK (bool_changed),     "/apps/xchat/irc/showtimestamps");
	g_signal_connect (G_OBJECT (page->highlight_add),    "clicked",  G_CALLBACK (highlight_add),    page);
	g_signal_connect (G_OBJECT (page->highlight_edit),   "clicked",  G_CALLBACK (highlight_edit),   page);
	g_signal_connect (G_OBJECT (page->highlight_remove), "clicked",  G_CALLBACK (highlight_remove), page);

	gconf_client_notify_add (p->gconf, "/apps/xchat/irc/nickname",              (GConfClientNotifyFunc) gconf_entry_changed, page->nick_name,       NULL, NULL);
	gconf_client_notify_add (p->gconf, "/apps/xchat/irc/realname",              (GConfClientNotifyFunc) gconf_entry_changed, page->real_name,       NULL, NULL);
	gconf_client_notify_add (p->gconf, "/apps/xchat/irc/quitmsg",               (GConfClientNotifyFunc) gconf_entry_changed, page->quit_message,    NULL, NULL);
	gconf_client_notify_add (p->gconf, "/apps/xchat/irc/partmsg",               (GConfClientNotifyFunc) gconf_entry_changed, page->part_message,    NULL, NULL);
	gconf_client_notify_add (p->gconf, "/apps/xchat/irc/awaymsg",               (GConfClientNotifyFunc) gconf_entry_changed, page->away_message,    NULL, NULL);
	gconf_client_notify_add (p->gconf, "/apps/xchat/main_window/use_sys_fonts", (GConfClientNotifyFunc) gconf_bool_changed,  page->usesysfonts,     NULL, NULL);
	gconf_client_notify_add (p->gconf, "/apps/xchat/main-window/font",          (GConfClientNotifyFunc) gconf_font_changed,  page->font_selection,  NULL, NULL);
	gconf_client_notify_add (p->gconf, "/apps/xchat/irc/showcolors",            (GConfClientNotifyFunc) gconf_bool_changed,  page->show_colors,     NULL, NULL);
	gconf_client_notify_add (p->gconf, "/apps/xchat/irc/showtimestamps",        (GConfClientNotifyFunc) gconf_bool_changed,  page->show_timestamps, NULL, NULL);

	text = gconf_client_get_string (p->gconf, "/apps/xchat/irc/nickname", NULL);
	gtk_entry_set_text (GTK_ENTRY (page->nick_name), text);
	g_free (text);

	text = gconf_client_get_string (p->gconf, "/apps/xchat/irc/realname", NULL);
	gtk_entry_set_text (GTK_ENTRY (page->real_name), text);
	g_free (text);

	text = gconf_client_get_string (p->gconf, "/apps/xchat/irc/quitmsg", NULL);
	gtk_entry_set_text (GTK_ENTRY (page->quit_message), text);
	g_free (text);

	text = gconf_client_get_string (p->gconf, "/apps/xchat/irc/partmsg", NULL);
	gtk_entry_set_text (GTK_ENTRY (page->part_message), text);
	g_free (text);

	text = gconf_client_get_string (p->gconf, "/apps/xchat/irc/awaymsg", NULL);
	gtk_entry_set_text (GTK_ENTRY (page->away_message), text);
	g_free (text);

	toggle = gconf_client_get_bool (p->gconf, "/apps/xchat/main_window/use_sys_fonts", NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->usesysfonts), toggle);
	gtk_widget_set_sensitive (page->font_selection, !toggle);

	text = gconf_client_get_string (p->gconf, "/apps/xchat/main_window/font", NULL);
	gtk_font_button_set_font_name (GTK_FONT_BUTTON (page->font_selection), text);
	g_free (text);

	toggle = gconf_client_get_bool (p->gconf, "/apps/xchat/irc/showcolors", NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->show_colors), toggle);

	toggle = gconf_client_get_bool (p->gconf, "/apps/xchat/irc/showtimestamps", NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->show_timestamps), toggle);

	/* highlight list */
	page->highlight_store = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (page->highlight_list), GTK_TREE_MODEL (page->highlight_store));
	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "editable", TRUE, NULL);
	page->highlight_column = gtk_tree_view_column_new_with_attributes ("highlight", renderer, "text", 0, NULL);
	gtk_tree_view_insert_column (GTK_TREE_VIEW (page->highlight_list), page->highlight_column, 0);
	gtk_widget_set_sensitive (page->highlight_edit, FALSE);
	gtk_widget_set_sensitive (page->highlight_remove, FALSE);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->highlight_list));
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (highlight_selection_changed), page);
	g_signal_connect (G_OBJECT (renderer), "edited", G_CALLBACK (highlight_edited), page);

	highlight_entries = g_strsplit (prefs.bluestring, ",", 0);
	for (i = 0; highlight_entries[i]; i++) {
		gtk_list_store_append (page->highlight_store, &iter);
		gtk_list_store_set (page->highlight_store, &iter, 0, highlight_entries[i], -1);
	}
	g_strfreev (highlight_entries);

	return page;
}

void
preferences_page_irc_free (PreferencesIrcPage *page)
{
	gdk_pixbuf_unref (page->icon);
}
