/*
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * Copyright (C) 2005 Novell, Inc. (www.novell.com)
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

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <gconf/gconf-client.h>
#include <calendar/gui/e-cal-popup.h>
#include <calendar/gui/e-cal-config.h>
#include <libgnome/gnome-i18n.h>

static GtkListStore *store = NULL;

void       publish_calendar_context_activate (EPlugin *ep, ECalPopupTargetSource *target);
GtkWidget *publish_calendar_locations (EPlugin *epl, EConfigHookItemFactoryData *data);

enum {
	URL_LIST_ENABLED_COLUMN,
	URL_LIST_LOCATION_COLUMN,
	URL_LIST_URL_COLUMN,
	URL_LIST_N_COLUMNS,
};

typedef struct {
	GConfClient *gconf;
	GtkWidget   *treeview;
	GtkWidget   *url_add;
	GtkWidget   *url_edit;
	GtkWidget   *url_remove;
	GtkWidget   *url_enable;
} PublishUIData;

static void
url_list_enable_toggled (GtkCellRendererToggle *renderer,
                         const char            *path_string,
			 PublishUIData         *ui)
{
	/* FIXME: implement */
}

static void
selection_changed (GtkTreeSelection *selection, PublishUIData *ui)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_widget_set_sensitive (ui->url_edit, TRUE);
		gtk_widget_set_sensitive (ui->url_remove, TRUE);
		gtk_widget_set_sensitive (ui->url_enable, TRUE);
	} else {
		gtk_widget_set_sensitive (ui->url_edit, FALSE);
		gtk_widget_set_sensitive (ui->url_remove, FALSE);
		gtk_widget_set_sensitive (ui->url_enable, FALSE);
	}
}

static void
url_list_double_click (GtkTreeView       *treeview,
		       GtkTreePath       *path,
		       GtkTreeViewColumn *column,
		       PublishUIData     *ui)
{
}

static void
url_add_clicked (GtkButton *button, PublishUIData *ui)
{
}

static void
url_edit_clicked (GtkButton *button, PublishUIData *ui)
{
}

static void
url_remove_clicked (GtkButton *button, PublishUIData *ui)
{
}

static void
url_enable_clicked (GtkButton *button, PublishUIData *ui)
{
}

void
publish_calendar_context_activate (EPlugin *ep, ECalPopupTargetSource *target)
{
	/* FIXME: implement */
}

GtkWidget *
publish_calendar_locations (EPlugin *epl, EConfigHookItemFactoryData *data)
{
	GladeXML *xml;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	GtkWidget *toplevel;
	PublishUIData *ui = g_new0 (PublishUIData, 1);

	xml = glade_xml_new (PLUGINDIR "/publish-calendar.glade", "toplevel", NULL);

	ui->treeview = glade_xml_get_widget (xml, "url list");
	if (store == NULL)
		store = gtk_list_store_new (URL_LIST_N_COLUMNS, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_view_set_model (GTK_TREE_VIEW (ui->treeview), GTK_TREE_MODEL (store));

	renderer = gtk_cell_renderer_toggle_new ();
	g_object_set (G_OBJECT (renderer), "activatable", TRUE, NULL);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (ui->treeview), -1, _("Enabled"),
			                             renderer, "active", URL_LIST_ENABLED_COLUMN, NULL);
	g_signal_connect (G_OBJECT (renderer), "toggled", G_CALLBACK (url_list_enable_toggled), ui);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (ui->treeview), -1, _("Location"),
			                             renderer, "text", URL_LIST_LOCATION_COLUMN, NULL);
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (ui->treeview));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (selection), "changed", G_CALLBACK (selection_changed), ui);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (ui->treeview), TRUE);
	g_signal_connect (G_OBJECT (ui->treeview), "row-activated", G_CALLBACK (url_list_double_click), ui);

	ui->url_add = glade_xml_get_widget (xml, "url add");
	g_signal_connect (G_OBJECT (ui->url_add), "clicked", G_CALLBACK (url_add_clicked), ui);
	ui->url_edit = glade_xml_get_widget (xml, "url edit");
	g_signal_connect (G_OBJECT (ui->url_edit), "clicked", G_CALLBACK (url_edit_clicked), ui);
	ui->url_remove = glade_xml_get_widget (xml, "url remove");
	g_signal_connect (G_OBJECT (ui->url_remove), "clicked", G_CALLBACK (url_remove_clicked), ui);
	ui->url_enable = glade_xml_get_widget (xml, "url enable");
	g_signal_connect (G_OBJECT (ui->url_enable), "clicked", G_CALLBACK (url_enable_clicked), ui);

	selection_changed (selection, ui);

	toplevel = glade_xml_get_widget (xml, "toplevel");
	gtk_widget_show_all (toplevel);
	gtk_box_pack_start (GTK_BOX (data->parent), toplevel, FALSE, TRUE, 0);
}
