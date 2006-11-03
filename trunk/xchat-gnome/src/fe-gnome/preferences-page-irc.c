/*
 * preferences-page-irc.c - helpers for the irc preferences page
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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
#include <string.h>
#include <gconf/gconf-client.h>
#include "../common/xchat.h"
#include "../common/text.h"
#include "../common/xchatc.h"
#include "preferences-page-irc.h"
#include "preferences-dialog.h"
#include "util.h"
#include "conversation-panel.h"

G_DEFINE_TYPE(PreferencesPageIrc, preferences_page_irc, PREFERENCES_PAGE_TYPE)

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
sysfonts_changed (GtkToggleButton *toggle, PreferencesPageIrc *page)
{
	gtk_widget_set_sensitive (page->font_selection, !gtk_toggle_button_get_active (toggle));
}

static void
highlight_selection_changed (GtkTreeSelection *select, PreferencesPageIrc *page)
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
save_highlight (PreferencesPageIrc *page)
{
	GtkTreeIter iter;
	gchar *highlight, *tmp1, *tmp2;
	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (page->highlight_store), &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (page->highlight_store), &iter, 0, &tmp1, -1);
		highlight = g_strdup (tmp1);
	} else {
		prefs.irc_extra_hilight[0] = '\0';
		return;
	}
	while (gtk_tree_model_iter_next (GTK_TREE_MODEL (page->highlight_store), &iter)) {
		tmp2 = highlight;
		gtk_tree_model_get (GTK_TREE_MODEL (page->highlight_store), &iter, 0, &tmp1, -1);
		highlight = g_strdup_printf ("%s,%s", tmp2, tmp1);
		g_free (tmp2);
	}
	strncpy (prefs.irc_extra_hilight, highlight, 300);
	g_free (highlight);
}

static void
highlight_add (GtkButton *button, PreferencesPageIrc *page)
{
	GtkTreeIter iter;
	GtkTreePath *path;

	gtk_list_store_append (page->highlight_store, &iter);
	path = gtk_tree_model_get_path (GTK_TREE_MODEL (page->highlight_store), &iter);
	gtk_tree_view_set_cursor (GTK_TREE_VIEW (page->highlight_list), path, page->highlight_column, TRUE);
	gtk_tree_path_free (path);
}

static void
highlight_edit (GtkButton *button, PreferencesPageIrc *page)
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
highlight_remove (GtkButton *button, PreferencesPageIrc *page)
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
highlight_edited (GtkCellRendererText *renderer, gchar *arg1, gchar *newtext, PreferencesPageIrc *page)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->highlight_list));
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		if (strlen (newtext)) {
			gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, newtext, -1);
		} else {
			gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
		}
		save_highlight (page);
	}
}

static void
highlight_canceled (GtkCellRendererText *renderer, PreferencesPageIrc *page)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *text;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->highlight_list));
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (page->highlight_store), &iter, 0, &text, -1);
		if (text == NULL)
			gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
	}
}

static void
auto_logging_changed (GtkToggleButton *button, gpointer data)
{
	gboolean active;
	GSList *list;
	session *sess;

	active = gtk_toggle_button_get_active (button);

	if (active) {
		prefs.logging = 1;
	} else {
		prefs.logging = 0;
	}

	list = sess_list;
	while (list) {
		sess = list->data;
		if (active) {
			log_open (sess);
		} else {
			log_close (sess);
		}
		list = list->next;
	}
}

static void
show_marker_changed (GtkToggleButton *button, gpointer data)
{
	gboolean active;

	active = gtk_toggle_button_get_active (button);
	conversation_panel_set_show_marker (CONVERSATION_PANEL (gui.conversation_panel), active);

	if (active) {
		prefs.show_marker = 1;
	} else {
		prefs.show_marker = 0;
	}
}

PreferencesPageIrc*
preferences_page_irc_new (gpointer prefs_dialog, GladeXML *xml)
{
	PreferencesPageIrc *page = g_object_new (PREFERENCES_PAGE_IRC_TYPE, NULL);
	PreferencesDialog *p = (PreferencesDialog *) prefs_dialog;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	gchar *text;
	gboolean toggle;
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

	GW(auto_logging);
	GW(show_timestamps);
	GW(show_marker);
#undef GW

	GtkIconTheme *theme = gtk_icon_theme_get_default ();
	PREFERENCES_PAGE (page)->icon = gtk_icon_theme_load_icon (theme, "preferences-system", 16, 0, NULL);

	gtk_list_store_append (p->page_store, &iter);
	gtk_list_store_set (p->page_store, &iter, 0, PREFERENCES_PAGE (page)->icon, 1, _("IRC Preferences"), 2, 0, -1);

	g_signal_connect (G_OBJECT (page->nick_name),        "changed",  G_CALLBACK (entry_changed),    "/apps/xchat/irc/nickname");
	g_signal_connect (G_OBJECT (page->real_name),        "changed",  G_CALLBACK (entry_changed),    "/apps/xchat/irc/realname");
	g_signal_connect (G_OBJECT (page->quit_message),     "changed",  G_CALLBACK (entry_changed),    "/apps/xchat/irc/quitmsg");
	g_signal_connect (G_OBJECT (page->part_message),     "changed",  G_CALLBACK (entry_changed),    "/apps/xchat/irc/partmsg");
	g_signal_connect (G_OBJECT (page->away_message),     "changed",  G_CALLBACK (entry_changed),    "/apps/xchat/irc/awaymsg");
	g_signal_connect (G_OBJECT (page->usesysfonts),      "toggled",  G_CALLBACK (bool_changed),     "/apps/xchat/main_window/use_sys_fonts");
	g_signal_connect (G_OBJECT (page->usesysfonts),      "toggled",  G_CALLBACK (sysfonts_changed), page);
	g_signal_connect (G_OBJECT (page->font_selection),   "font-set", G_CALLBACK (font_changed),     "/apps/xchat/main_window/font");
	g_signal_connect (G_OBJECT (page->show_timestamps),  "toggled",  G_CALLBACK (bool_changed),     "/apps/xchat/irc/showtimestamps");
	g_signal_connect (G_OBJECT (page->auto_logging),     "toggled",  G_CALLBACK (auto_logging_changed), NULL);
	g_signal_connect (G_OBJECT (page->show_marker),      "toggled",  G_CALLBACK (show_marker_changed), NULL);
	g_signal_connect (G_OBJECT (page->highlight_add),    "clicked",  G_CALLBACK (highlight_add),    page);
	g_signal_connect (G_OBJECT (page->highlight_edit),   "clicked",  G_CALLBACK (highlight_edit),   page);
	g_signal_connect (G_OBJECT (page->highlight_remove), "clicked",  G_CALLBACK (highlight_remove), page);

	page->nh[0] = gconf_client_notify_add (p->gconf, "/apps/xchat/irc/nickname",              (GConfClientNotifyFunc) gconf_entry_changed, page->nick_name,       NULL, NULL);
	page->nh[1] = gconf_client_notify_add (p->gconf, "/apps/xchat/irc/realname",              (GConfClientNotifyFunc) gconf_entry_changed, page->real_name,       NULL, NULL);
	page->nh[2] = gconf_client_notify_add (p->gconf, "/apps/xchat/irc/quitmsg",               (GConfClientNotifyFunc) gconf_entry_changed, page->quit_message,    NULL, NULL);
	page->nh[3] = gconf_client_notify_add (p->gconf, "/apps/xchat/irc/partmsg",               (GConfClientNotifyFunc) gconf_entry_changed, page->part_message,    NULL, NULL);
	page->nh[4] = gconf_client_notify_add (p->gconf, "/apps/xchat/irc/awaymsg",               (GConfClientNotifyFunc) gconf_entry_changed, page->away_message,    NULL, NULL);
	page->nh[5] = gconf_client_notify_add (p->gconf, "/apps/xchat/main_window/use_sys_fonts", (GConfClientNotifyFunc) gconf_bool_changed,  page->usesysfonts,     NULL, NULL);
	page->nh[6] = gconf_client_notify_add (p->gconf, "/apps/xchat/main-window/font",          (GConfClientNotifyFunc) gconf_font_changed,  page->font_selection,  NULL, NULL);
	page->nh[7] = gconf_client_notify_add (p->gconf, "/apps/xchat/irc/showtimestamps",        (GConfClientNotifyFunc) gconf_bool_changed,  page->show_timestamps, NULL, NULL);

	text = gconf_client_get_string (p->gconf, "/apps/xchat/irc/nickname", NULL);
	if (text) {
		gtk_entry_set_text (GTK_ENTRY (page->nick_name), text);
		g_free (text);
	} else {
		gtk_entry_set_text (GTK_ENTRY (page->nick_name), "");
	}

	text = gconf_client_get_string (p->gconf, "/apps/xchat/irc/realname", NULL);
	if (text) {
		gtk_entry_set_text (GTK_ENTRY (page->real_name), text);
		g_free (text);
	} else {
		gtk_entry_set_text (GTK_ENTRY (page->real_name), "");
	}

	text = gconf_client_get_string (p->gconf, "/apps/xchat/irc/quitmsg", NULL);
	if (text) {
		gtk_entry_set_text (GTK_ENTRY (page->quit_message), text);
		g_free (text);
	} else {
		gtk_entry_set_text (GTK_ENTRY (page->quit_message), "");
	}

	text = gconf_client_get_string (p->gconf, "/apps/xchat/irc/partmsg", NULL);
	if (text) {
		gtk_entry_set_text (GTK_ENTRY (page->part_message), text);
		g_free (text);
	} else {
		gtk_entry_set_text (GTK_ENTRY (page->part_message), "");
	}

	text = gconf_client_get_string (p->gconf, "/apps/xchat/irc/awaymsg", NULL);
	if (text) {
		gtk_entry_set_text (GTK_ENTRY (page->away_message), text);
		g_free (text);
	} else {
		gtk_entry_set_text (GTK_ENTRY (page->away_message), "");
	}

	toggle = gconf_client_get_bool (p->gconf, "/apps/xchat/main_window/use_sys_fonts", NULL);
	if (toggle)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->usesysfonts), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->usethisfont), TRUE);
	gtk_widget_set_sensitive (page->font_selection, !toggle);

	text = gconf_client_get_string (p->gconf, "/apps/xchat/main_window/font", NULL);
	if (text) {
		gtk_font_button_set_font_name (GTK_FONT_BUTTON (page->font_selection), text);
		g_free (text);
	}

	toggle = gconf_client_get_bool (p->gconf, "/apps/xchat/irc/showtimestamps", NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->show_timestamps), toggle);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->auto_logging), prefs.logging);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->show_marker), prefs.show_marker);
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
	g_signal_connect (G_OBJECT (renderer), "editing-canceled", G_CALLBACK (highlight_canceled), page);

	highlight_entries = g_strsplit (prefs.irc_extra_hilight, ",", 0);
	for (i = 0; highlight_entries[i]; i++) {
		gtk_list_store_append (page->highlight_store, &iter);
		gtk_list_store_set (page->highlight_store, &iter, 0, highlight_entries[i], -1);
	}
	g_strfreev (highlight_entries);

	return page;
}

static void
preferences_page_irc_init (PreferencesPageIrc *page)
{
}

static void
preferences_page_irc_dispose (GObject *object)
{
	PreferencesPageIrc *page = (PreferencesPageIrc *) object;

	if (page->highlight_store)
	{
		g_object_unref (page->highlight_store);
		page->highlight_store = NULL;
	}

	G_OBJECT_CLASS (preferences_page_irc_parent_class)->dispose (object);
}

static void
preferences_page_irc_finalize (GObject *object)
{
	PreferencesPageIrc *page = (PreferencesPageIrc *) object;
	gint i;
	GConfClient *client;

	client = gconf_client_get_default ();
	for (i = 0; i < 8; i++) {
		gconf_client_notify_remove (client, page->nh[i]);
	}

	g_object_unref (client);

	G_OBJECT_CLASS (preferences_page_irc_parent_class)->finalize (object);
}

static void
preferences_page_irc_class_init (PreferencesPageIrcClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;

	object_class->dispose = preferences_page_irc_dispose;
	object_class->finalize = preferences_page_irc_finalize;
}
