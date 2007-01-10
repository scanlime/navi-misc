/*
 * preferences-dialog.c - helpers for the preference dialog
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
#include "preferences-dialog.h"
#include "pixmaps.h"
#include "util.h"

enum
{
	COL_ICON,
	COL_TITLE,
	COL_POS,
};

static GObjectClass *parent_class;

static void
preferences_dialog_finalize (GObject *object)
{
	//PreferencesDialog *p = (PreferencesDialog *) object;

	parent_class->finalize (object);
}

static void
preferences_dialog_dispose (GObject *object)
{
	PreferencesDialog *p = (PreferencesDialog *) object;

	if (p->irc_page) {
		g_object_unref (p->irc_page);
		p->irc_page = NULL;
	}

	if (p->colors_page) {
		g_object_unref (p->colors_page);
		p->colors_page = NULL;
	}

	if (p->effects_page) {
		g_object_unref (p->effects_page);
		p->effects_page = NULL;
	}

	if (p->dcc_page) {
		g_object_unref (p->dcc_page);
		p->dcc_page = NULL;
	}

	if (p->networks_page) {
		g_object_unref (p->networks_page);
		p->networks_page = NULL;
	}

#ifdef USE_PLUGIN
	if (p->plugins_page) {
		g_object_unref (p->plugins_page);
		p->plugins_page = NULL;
	}
#endif

#ifdef HAVE_LIBSEXY
	if (p->spellcheck_page) {
		g_object_unref (p->spellcheck_page);
		p->spellcheck_page = NULL;
	}
#endif

	if (p->gconf) {
		g_object_unref (p->gconf);
		p->gconf = NULL;
	}

	if (p->dialog) {
		gtk_widget_destroy (p->dialog);
		p->dialog = NULL;
	}

	parent_class->dispose (object);
}

static void
preferences_dialog_class_init (PreferencesDialogClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;

	parent_class = g_type_class_peek_parent (klass);

	object_class->finalize = preferences_dialog_finalize;
	object_class->dispose = preferences_dialog_dispose;
}

static void
page_selection_changed (GtkTreeSelection *select, PreferencesDialog *p)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	gint page;

	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 2, &page, -1);
		gtk_notebook_set_current_page (GTK_NOTEBOOK (p->settings_notebook), page);
	}
}

static void
preferences_response (GtkWidget *widget,
		      gint response,
		      PreferencesDialog *dialog)
{
	if (response == GTK_RESPONSE_HELP) {
		/* FIXME */
		return;
	}

	g_object_unref (dialog);
}

static void
preferences_dialog_add_page (PreferencesDialog *p, PreferencesPage *page)
{
	gint pos;
	GtkTreeIter iter;

	/* If the page has is own vbox, widgets are not in the glade files and so have to be
	 * added into the notebook */
	if (page->vbox) {
		pos = gtk_notebook_append_page (GTK_NOTEBOOK (p->settings_notebook), page->vbox, NULL);
		gtk_list_store_append (p->page_store, &iter);
		gtk_list_store_set (p->page_store, &iter,
			COL_ICON, page->icon,
			COL_TITLE, PREFERENCES_PAGE (page)->title,
			COL_POS, pos,
			-1);
	}
	// TODO : maybe use this to avoid the modification of page_store into each preferences-page-*.c
}

#ifdef USE_PLUGIN
static void
new_plugin_page (PreferencesPagePlugins *page, PreferencesPage *page_plugin, PreferencesDialog *p)
{
	preferences_dialog_add_page (p, page_plugin);
}

static void
remove_plugin_page (PreferencesPagePlugins *page, PreferencesPage *page_plugin, PreferencesDialog *p)
{
	GtkTreeIter iter;
	gboolean found = FALSE;
	gchar *name;
	gint pos;

	gtk_tree_model_get_iter_first (GTK_TREE_MODEL (p->page_store), &iter);
	do
	{
		gtk_tree_model_get (GTK_TREE_MODEL (p->page_store), &iter,
					COL_TITLE, &name,
					COL_POS, &pos, 
					-1);
		if (name && (strcmp (name, PREFERENCES_PAGE (page_plugin)->title) == 0))
			found = TRUE;
	} while (!found && gtk_tree_model_iter_next (GTK_TREE_MODEL (p->page_store), &iter));

	if (found) {
		gtk_notebook_remove_page (GTK_NOTEBOOK (p->settings_notebook), pos);
		gtk_list_store_remove (p->page_store, &iter);
	}
}
#endif // USE_PLUGIN

static void
preferences_dialog_init (PreferencesDialog *p)
{
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;
	GladeXML *xml;
	gchar *path;

	p->gconf = NULL;
	p->dialog = NULL;

	xml = NULL;
	path = locate_data_file ("preferences-dialog.glade");
	xml = glade_xml_new (path, "dialog", NULL);
	g_free (path);
	if (!xml) {
		return;
	}

#define GW(name) ((p->name) = glade_xml_get_widget (xml, #name))
	GW(dialog);
	GW(settings_page_list);
	GW(settings_notebook);
#undef GW

	p->gconf = gconf_client_get_default ();

	g_assert (p->dialog);

	g_signal_connect (p->dialog, "response", G_CALLBACK (preferences_response), p);
	g_signal_connect (p->dialog, "key-press-event", G_CALLBACK (dialog_escape_key_handler_hide), NULL);

	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (p->settings_notebook), FALSE);

	p->page_store = gtk_list_store_new (4, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT, G_TYPE_POINTER);
	gtk_tree_view_set_model (GTK_TREE_VIEW (p->settings_page_list), GTK_TREE_MODEL (p->page_store));
	column = gtk_tree_view_column_new ();
	icon_renderer = gtk_cell_renderer_pixbuf_new ();
	text_renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, icon_renderer, FALSE);
	gtk_tree_view_column_set_attributes (column, icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_column_pack_start (column, text_renderer, FALSE);
	gtk_tree_view_column_set_attributes (column, text_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (p->settings_page_list), column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (p->settings_page_list));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (page_selection_changed), p);

	p->irc_page      = preferences_page_irc_new      (p, xml);
#ifdef HAVE_LIBSEXY
	p->spellcheck_page= preferences_page_spellcheck_new (p,xml);
#endif
	p->colors_page   = preferences_page_colors_new   (p, xml);
	p->effects_page  = preferences_page_effects_new  (p, xml);
	p->dcc_page      = preferences_page_dcc_new      (p, xml);
	p->networks_page = preferences_page_networks_new (p, xml);
#ifdef USE_PLUGIN
	p->plugins_page  = preferences_page_plugins_new  (p, xml);

	g_signal_connect (p->plugins_page, "new-plugin-page",
			  G_CALLBACK (new_plugin_page), p);
	g_signal_connect (p->plugins_page, "remove-plugin-page",
			  G_CALLBACK (remove_plugin_page), p);
	preferences_page_plugins_check_plugins (p->plugins_page);
#endif
	g_object_unref (xml);
}

GType
preferences_dialog_get_type (void)
{
	static GType preferences_dialog_type = 0;
	if (G_UNLIKELY (preferences_dialog_type == 0)) {
		static const GTypeInfo preferences_dialog_info = {
			sizeof (PreferencesDialogClass),
			NULL, NULL,
			(GClassInitFunc) preferences_dialog_class_init,
			NULL, NULL,
			sizeof (PreferencesDialog),
			0,
			(GInstanceInitFunc) preferences_dialog_init,
		};
		preferences_dialog_type = g_type_register_static (G_TYPE_OBJECT, "PreferencesDialog", &preferences_dialog_info, 0);
	}

	return preferences_dialog_type;
}

PreferencesDialog *
preferences_dialog_new (void)
{
	PreferencesDialog *p = g_object_new (preferences_dialog_get_type (), NULL);
	if (p->dialog == NULL) {
		g_object_unref (p);
		return NULL;
	}

	return p;
}

void
preferences_dialog_show (PreferencesDialog *dialog)
{
	gtk_window_present_with_time (GTK_WINDOW (dialog->dialog),
				      gtk_get_current_event_time ());
}
