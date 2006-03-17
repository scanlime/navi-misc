/*
 * preferences-page-spellcheck.c - helpers for the spellcheck preferences page
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
#ifdef HAVE_LIBSEXY
#include <glib/gi18n.h>
#include <string.h>
#include <gconf/gconf-client.h>
#include <libsexy/sexy-spell-entry.h>
#include "preferences-page-spellcheck.h"
#include "preferences-dialog.h"
#include "gui.h"

enum
{
	COL_LANG_ACTIVATED = 0,
	COL_LANG_NAME,
	COL_LANG_CODE
};

static void
enabled_changed (GtkToggleButton *button, PreferencesSpellcheckPage *page)
{
	GConfClient *client;
	gboolean enabled;

	client = gconf_client_get_default ();
	enabled = gtk_toggle_button_get_active (button);
	gconf_client_set_bool (client, "/apps/xchat/spellcheck/enabled", enabled, NULL);

	gtk_widget_set_sensitive (page->vbox_langs, enabled);

	g_object_unref (client);
}

static void
language_changed (GtkCellRendererToggle *toggle, gchar *pathstr, PreferencesSpellcheckPage *page)
{
	GtkTreePath *path;
	GtkTreeIter iter;
	gchar *lang;
	gboolean activated, result;
	GSList *languages;
	GConfClient *client;
	GError *err = NULL;

	result = TRUE;
	path = gtk_tree_path_new_from_string (pathstr);
	if (gtk_tree_model_get_iter (GTK_TREE_MODEL (page->spellcheck_store), &iter, path)) {
	    gtk_tree_model_get (GTK_TREE_MODEL (page->spellcheck_store), &iter,
	                        COL_LANG_CODE, &lang,
	                        COL_LANG_ACTIVATED, &activated, -1);

		if (activated) {
			/* we want to deactive it */
			sexy_spell_entry_deactivate_language (SEXY_SPELL_ENTRY (gui.text_entry), lang);
		}
		else {
			/* we want to activate it */
			result = sexy_spell_entry_activate_language (SEXY_SPELL_ENTRY (gui.text_entry), lang, &err);
		}

		if (result) {
			/* ok we can update the configuration */
			/*gtk_list_store_set (page->spellcheck_store, &iter,
			                    COL_LANG_ACTIVATED, !activated,
			                    -1);
			*/
			client = gconf_client_get_default ();
			languages = sexy_spell_entry_get_active_languages (SEXY_SPELL_ENTRY (gui.text_entry));
			gconf_client_set_list (client, "/apps/xchat/spellcheck/languages",
			                       GCONF_VALUE_STRING, languages, NULL);

			if (languages == NULL)
				/* No more languages selected: desactivate spellchecking */
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->enable_spellcheck), FALSE);

			g_slist_foreach (languages, (GFunc) g_free, NULL);
			g_slist_free (languages);
			g_object_unref (client);
		}
		else {
			g_printerr (_("Error in language %s activation: %s\n"), lang, err->message);
			g_error_free (err);
		}
	}

	gtk_tree_path_free (path);
}

static void
gconf_enabled_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, GtkToggleButton *enabled_spellcheck)
{
	gboolean enabled;

	g_signal_handlers_block_by_func (enabled_spellcheck, "toggled", enabled_changed);
	enabled = gconf_client_get_bool (client, "/apps/xchat/spellcheck/enabled", NULL);
	gtk_toggle_button_set_active (enabled_spellcheck, enabled);
	g_signal_handlers_unblock_by_func (enabled_spellcheck, "toggled", enabled_changed);
}

static void
gconf_languages_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, GtkListStore *store)
{
	GtkTreeIter iter;
	gchar *lang;
	gboolean active;
	GError *err = NULL;
	GSList *new_languages, *old_languages;

	/* First we use the new languages list
	 *
	 * FIXME: This suck because it should be done in text-entry.c
	 * see FIXME in that file for information about this problem.
	 * */
	new_languages = gconf_client_get_list (client, "/apps/xchat/spellcheck/languages", GCONF_VALUE_STRING, NULL);

	if (new_languages != NULL)
		sexy_spell_entry_set_active_languages (SEXY_SPELL_ENTRY (gui.text_entry), new_languages, &err);

	if (err) {
		g_printerr (_("Error in spell checking configuration: %s\n"), err->message);
		g_error_free (err);

		old_languages = sexy_spell_entry_get_active_languages (SEXY_SPELL_ENTRY (gui.text_entry));
		if (old_languages != NULL) {
			gconf_client_set_list (client, "/apps/xchat/spellcheck/languages",
			                       GCONF_VALUE_STRING, old_languages, NULL);
			g_slist_foreach (old_languages, (GFunc) g_free, NULL);
			g_slist_free (old_languages);
		}
	}
	else {
		/* Then we update the store */
		if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &iter)) {
			do {
				gtk_tree_model_get (GTK_TREE_MODEL (store), &iter,
				                    COL_LANG_CODE, &lang, -1);
				active = sexy_spell_entry_language_is_active (SEXY_SPELL_ENTRY (gui.text_entry), lang);

				gtk_list_store_set (store, &iter, COL_LANG_ACTIVATED, !active -1);
			} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter));
		}
	}


	g_slist_foreach (new_languages, (GFunc) g_free, NULL);
	g_slist_free (new_languages);
}

PreferencesSpellcheckPage *
preferences_page_spellcheck_new (gpointer prefs_dialog, GladeXML *xml)
{
	PreferencesSpellcheckPage *page = g_new0 (PreferencesSpellcheckPage, 1);
	PreferencesDialog *p = (PreferencesDialog *) prefs_dialog;
	GtkTreeIter iter;
	gboolean enabled;
	GtkTreeViewColumn *column;
	GSList *languages, *l;

#define GW(name) ((page->name) = glade_xml_get_widget (xml, #name))
	GW(enable_spellcheck);
	GW(spellcheck_list);
	GW(vbox_langs);
#undef GW

	if (g_file_test ("../../data/spellcheck.png", G_FILE_TEST_EXISTS))
		page->icon = gdk_pixbuf_new_from_file ("../../data/spellcheck.png", NULL);
	else
		page->icon = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/spellcheck.png", NULL);
	gtk_list_store_append (p->page_store, &iter);
	gtk_list_store_set (p->page_store, &iter, 0, page->icon, 1, _("Spell checking"), 2, 7, -1);

	/* spellcheck languages list */
	page->spellcheck_store = gtk_list_store_new (3, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (page->spellcheck_list),
	                         GTK_TREE_MODEL (page->spellcheck_store));

	page->activated_renderer = gtk_cell_renderer_toggle_new ();
	g_object_set (G_OBJECT (page->activated_renderer), "activatable", TRUE, NULL);
	column = gtk_tree_view_column_new_with_attributes (_("Enable"), page->activated_renderer,
	                                                   _("active"), COL_LANG_ACTIVATED, NULL);
	gtk_tree_view_insert_column (GTK_TREE_VIEW (page->spellcheck_list), column, 0);

	page->name_renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Language"), page->name_renderer, "text",
	                                                   COL_LANG_NAME, NULL);
	gtk_tree_view_insert_column (GTK_TREE_VIEW (page->spellcheck_list), column, 1);

	g_signal_connect (G_OBJECT (page->enable_spellcheck),   "toggled", G_CALLBACK (enabled_changed), page);
	g_signal_connect (G_OBJECT (page->activated_renderer),  "toggled", G_CALLBACK (language_changed),page);

	page->nh[0] = gconf_client_notify_add (p->gconf, "/apps/xchat/spellcheck/enabled", (GConfClientNotifyFunc) gconf_enabled_changed, page->enable_spellcheck, NULL, NULL);
	page->nh[1] = gconf_client_notify_add (p->gconf, "/apps/xchat/spellcheck/languages", (GConfClientNotifyFunc) gconf_languages_changed, page->spellcheck_store, NULL, NULL);

	/* Populate the model */
	languages = sexy_spell_entry_get_languages (SEXY_SPELL_ENTRY (gui.text_entry));
	for (l = languages; l != NULL; l = l->next) {
		gboolean active;

		gtk_list_store_append (page->spellcheck_store, &iter);
		active = sexy_spell_entry_language_is_active (SEXY_SPELL_ENTRY (gui.text_entry), l->data);
		gchar *name = sexy_spell_entry_get_language_name (SEXY_SPELL_ENTRY (gui.text_entry), l->data);

		if (name) {
			gtk_list_store_set (page->spellcheck_store, &iter,
			                    COL_LANG_NAME, name,
			                    COL_LANG_ACTIVATED, active,
			                    COL_LANG_CODE, l->data,
			                    -1);
			g_free (name);
		}
		else
			gtk_list_store_set (page->spellcheck_store, &iter,
			                    COL_LANG_NAME, l->data,
			                    COL_LANG_ACTIVATED, active,
			                    COL_LANG_CODE, l->data,
			                    -1);
	}
	g_slist_free (languages);

	enabled = gconf_client_get_bool (p->gconf, "/apps/xchat/spellcheck/enabled", NULL);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->enable_spellcheck), enabled);
	gtk_widget_set_sensitive (page->vbox_langs, enabled);

	return page;
}

void
preferences_page_spellcheck_free (PreferencesSpellcheckPage *page)
{
	gint i;
	 GConfClient *client;

	client = gconf_client_get_default ();
	for (i = 0; i < 2; i++)
		gconf_client_notify_remove (client, page->nh[i]);
	g_object_unref (client);

	g_object_unref (page->icon);
	g_object_unref (page->spellcheck_store);
	g_free (page);
}

#endif
