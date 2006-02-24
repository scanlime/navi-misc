/*
 * preferences-dialog.c - helpers for the preference dialog
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
#include "preferences-dialog.h"
#include "pixmaps.h"
#include "util.h"

static GtkDialogClass *parent_class;

static void
preferences_dialog_finalize (GObject *object)
{
	PreferencesDialog *p = (PreferencesDialog *) object;

	preferences_page_irc_free      (p->irc_page);
	preferences_page_colors_free   (p->colors_page);
	preferences_page_effects_free  (p->effects_page);
	preferences_page_dcc_free      (p->dcc_page);
	preferences_page_networks_free (p->networks_page);
#ifdef USE_PLUGIN
	preferences_page_plugins_free  (p->plugins_page);
#endif

	((GObjectClass *) parent_class)->finalize (object);
}

static void
preferences_dialog_dispose (GObject *object)
{
	PreferencesDialog *p = (PreferencesDialog *) object;

	if (p->gconf) {
		g_object_unref (p->gconf);
		p->gconf = NULL;
	}

	((GObjectClass *) parent_class)->dispose (object);
}

static void
preferences_dialog_class_init (PreferencesDialogClass *klass)
{
	GObjectClass *parent = (GObjectClass *) klass;
	parent->finalize = preferences_dialog_finalize;
	parent->dispose = preferences_dialog_dispose;
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
preferences_dialog_init (PreferencesDialog *p)
{
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;
	GladeXML *xml;

	p->gconf = NULL;
	p->toplevel = NULL;

	xml = NULL;
	if (g_file_test ("../../data/preferences-dialog.glade", G_FILE_TEST_EXISTS))
		xml = glade_xml_new ("../../data/preferences-dialog.glade", "toplevel", NULL);
	if (!xml)
		xml = glade_xml_new (XCHATSHAREDIR "/preferences-dialog.glade", "toplevel", NULL);
	if (!xml)
		return;

#define GW(name) ((p->name) = glade_xml_get_widget (xml, #name))
	GW(toplevel);
	GW(settings_page_list);
	GW(settings_notebook);
#undef GW

	p->gconf = gconf_client_get_default ();

	gtk_dialog_add_button (GTK_DIALOG (p), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
	gtk_dialog_add_button (GTK_DIALOG (p), GTK_STOCK_HELP, GTK_RESPONSE_HELP);
	gtk_container_set_border_width (GTK_CONTAINER (p), 6);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (p)->vbox), p->toplevel);
	gtk_dialog_set_has_separator (GTK_DIALOG (p), FALSE);
	g_signal_connect (G_OBJECT (p), "key-press-event", G_CALLBACK (dialog_escape_key_handler_hide), NULL);

	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (p->settings_notebook), FALSE);

	p->page_store = gtk_list_store_new (3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
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
	p->colors_page   = preferences_page_colors_new   (p, xml);
	p->effects_page  = preferences_page_effects_new  (p, xml);
	p->dcc_page      = preferences_page_dcc_new      (p, xml);
	p->networks_page = preferences_page_networks_new (p, xml);
#ifdef USE_PLUGIN
	p->plugins_page  = preferences_page_plugins_new  (p, xml);
#endif

	gtk_window_set_title (GTK_WINDOW (p), "Preferences");

	g_object_unref (xml);
}

GType
preferences_dialog_get_type (void)
{
	static GType preferences_dialog_type = 0;
	if (!preferences_dialog_type) {
		static const GTypeInfo preferences_dialog_info = {
			sizeof (PreferencesDialogClass),
			NULL, NULL,
			(GClassInitFunc) preferences_dialog_class_init,
			NULL, NULL,
			sizeof (PreferencesDialog),
			0,
			(GInstanceInitFunc) preferences_dialog_init,
		};
		preferences_dialog_type = g_type_register_static (GTK_TYPE_DIALOG, "PreferencesDialog", &preferences_dialog_info, 0);

		parent_class = g_type_class_ref (GTK_TYPE_DIALOG);
	}

	return preferences_dialog_type;
}

PreferencesDialog *
preferences_dialog_new (void)
{
	PreferencesDialog *p = g_object_new (preferences_dialog_get_type (), 0);
	if (p->toplevel == NULL) {
		g_object_unref (p);
		return NULL;
	}

	return p;
}
