/*
 * preferences-dialog.c - helpers for the preference dialog
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

#include "preferences-dialog.h"
#include "preferences-keybindings-page.h"
#include "preferences-plugins-page.h"
#include "pixmaps.h"

static GtkDialogClass *parent_class;

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
	if (g_file_test ("preferences-dialog.glade", G_FILE_TEST_EXISTS))
		xml = glade_xml_new ("preferences-dialog.glade", "toplevel", NULL);
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
	p->networks_page = preferences_page_networks_new (p, xml);

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
preferences_dialog_new ()
{
	PreferencesDialog *p = g_object_new (preferences_dialog_get_type (), 0);
	if (p->toplevel == NULL) {
		g_object_unref (p);
		return NULL;
	}

	return p;
}

/*******************************************************************************
 * CRUFT BARRIER ***************************************************************
 *******************************************************************************/

void initialize_pages_list ();
void hide_preferences_dialog (GtkWidget *widget, gpointer data);
void initialize_file_transfers_page ();
void settings_page_changed (GtkTreeSelection *selection, gpointer data);

static void close_window (GtkWidget *widget, GdkEvent *event, gpointer user_data);

void
initialize_preferences_dialog ()
{
	GtkWidget *close_button;

	gui.preferences_dialog = GTK_DIALOG (glade_xml_get_widget (gui.xml, "preferences"));
	gtk_window_set_transient_for (GTK_WINDOW (gui.preferences_dialog), GTK_WINDOW (gui.main_window));
	gtk_widget_hide_all (GTK_WIDGET (gui.preferences_dialog));
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (glade_xml_get_widget (gui.xml, "settings notebook")), FALSE);
	initialize_pages_list ();
	initialize_preferences_plugins_page ();
/*	initialize_preferences_keybindings_page ();*/

	close_button = glade_xml_get_widget (gui.xml, "close preferences");
	g_signal_connect (G_OBJECT (close_button), "clicked", G_CALLBACK (hide_preferences_dialog), NULL);

	/* (>")> Prevent the prefs window from being destroyed when the close button is clicked. */
	close_button = glade_xml_get_widget (gui.xml, "preferences");
	g_signal_connect (G_OBJECT (close_button), "delete-event", G_CALLBACK (close_window), NULL);
}

void
hide_preferences_dialog (GtkWidget *widget, gpointer data)
{
	gtk_widget_hide_all (GTK_WIDGET (gui.preferences_dialog));
}

static void
close_window (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	hide_preferences_dialog (widget, user_data);
}

void
initialize_pages_list ()
{
	GtkWidget *options_list;
	GtkListStore *store;
	GtkTreeIter iter;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;

	options_list = glade_xml_get_widget (gui.xml, "settings page list");

	store = gtk_list_store_new (3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
	gtk_tree_view_set_model (GTK_TREE_VIEW (options_list), GTK_TREE_MODEL (store));

	column = gtk_tree_view_column_new ();
	icon_renderer = gtk_cell_renderer_pixbuf_new ();
	text_renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, icon_renderer, FALSE);
	gtk_tree_view_column_set_attributes (column, icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_column_pack_start (column, text_renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, text_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (options_list), column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (options_list));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (settings_page_changed), NULL);

	gtk_list_store_append (store, &iter);
	gtk_list_store_set (store, &iter, 0, pix_prefs_plugins, 1, "Scripts and Plugins", 2, 0, -1);
	/*
	gtk_list_store_append (store, &iter);
	gtk_list_store_set (store, &iter, 0, pix_prefs_keybindings, 1, "Keyboard Shortcuts", 2, 1, -1);
	*/
}

void
settings_page_changed (GtkTreeSelection *selection, gpointer data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkWidget *notebook;
	gint page;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		notebook = glade_xml_get_widget (gui.xml, "settings notebook");
		gtk_tree_model_get (model, &iter, 2, &page, -1);
		gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), page);
	}
}
