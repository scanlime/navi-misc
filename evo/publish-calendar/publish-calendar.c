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
#include "url-editor-dialog.h"

static GtkListStore *store = NULL;
static GHashTable *uri_timeouts = NULL;
static GSList *publish_uris = NULL;

int        e_plugin_lib_enable (EPlugin *ep, int enable);
void       publish_calendar_context_activate (EPlugin *ep, ECalPopupTargetSource *target);
GtkWidget *publish_calendar_locations (EPlugin *epl, EConfigHookItemFactoryData *data);

typedef struct {
	GConfClient *gconf;
	GtkWidget   *treeview;
	GtkWidget   *url_add;
	GtkWidget   *url_edit;
	GtkWidget   *url_remove;
	GtkWidget   *url_enable;
} PublishUIData;

static void
url_list_changed (PublishUIData *ui)
{
	GtkTreeModel *model = NULL;
	GSList *url_list = NULL;
	GtkTreeIter iter;
	gboolean valid;
	GConfClient *client;

	url_list = NULL;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (ui->treeview));
	valid = gtk_tree_model_get_iter_first (model, &iter);
	while (valid) {
		EPublishUri *url;
		char *xml;

		gtk_tree_model_get (model, &iter, URL_LIST_URL_COLUMN, &url, -1);

		if ((xml = e_publish_uri_to_xml (url)))
			url_list = g_slist_append (url_list, xml);

		valid = gtk_tree_model_iter_next (model, &iter);
	}
	client = gconf_client_get_default ();
	gconf_client_set_list (client, "/apps/evolution/calendar/publish/uris", GCONF_VALUE_STRING, url_list, NULL);
	g_slist_foreach (url_list, (GFunc) g_free, NULL);
	g_slist_free (url_list);
}

static void
url_list_enable_toggled (GtkCellRendererToggle *renderer,
                         const char            *path_string,
			 PublishUIData         *ui)
{
	GtkTreeSelection *selection;
	EPublishUri *url = NULL;
	GtkTreeModel *model;
	GtkTreePath *path;
	GtkTreeIter iter;

	path = gtk_tree_path_new_from_string (path_string);
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (ui->treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (ui->treeview));

	if (gtk_tree_model_get_iter (model, &iter, path)) {
		gtk_tree_model_get (model, &iter, URL_LIST_URL_COLUMN, &url, -1);

		url->enabled = !url->enabled;
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, URL_LIST_ENABLED_COLUMN, url->enabled, -1);
	}

	gtk_tree_path_free (path);
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
url_add_clicked (GtkButton *button, PublishUIData *ui)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkWidget *url_editor;
	EPublishUri *uri;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (ui->treeview));
	url_editor = url_editor_dialog_new (model, NULL);
	url_editor_dialog_run ((UrlEditorDialog *) url_editor);

	uri = URL_EDITOR_DIALOG (url_editor)->uri;
	if (uri->location) {
		gtk_list_store_append (GTK_LIST_STORE (model), &iter);
		gtk_list_store_set (GTK_LIST_STORE (model), &iter,
				    URL_LIST_ENABLED_COLUMN, uri->enabled,
				    URL_LIST_LOCATION_COLUMN, uri->location,
				    URL_LIST_URL_COLUMN, uri, -1);
		url_list_changed (ui);
	} else {
		g_free (uri);
	}
	gtk_widget_destroy (url_editor);
}

static void
url_edit_clicked (GtkButton *button, PublishUIData *ui)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (ui->treeview));
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		EPublishUri *uri;
		GtkWidget *url_editor;

		gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, 2, &uri, -1);
		url_editor = url_editor_dialog_new (model, uri);
		url_editor_dialog_run ((UrlEditorDialog *) url_editor);

		gtk_list_store_set (GTK_LIST_STORE (model), &iter,
				    URL_LIST_ENABLED_COLUMN, uri->enabled,
				    URL_LIST_LOCATION_COLUMN, uri->location,
				    URL_LIST_URL_COLUMN, uri, -1);
		url_list_changed (ui);
	}
}

static void
url_list_double_click (GtkTreeView       *treeview,
		       GtkTreePath       *path,
		       GtkTreeViewColumn *column,
		       PublishUIData     *ui)
{
	url_edit_clicked (NULL, ui);
}

static void
url_remove_clicked (GtkButton *button, PublishUIData *ui)
{
	EPublishUri *url = NULL;
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkWidget *confirm;
	gint response;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (ui->treeview));
	if (!gtk_tree_selection_get_selected (selection, &model, &iter))
		return;

	gtk_tree_model_get (model, &iter, URL_LIST_URL_COLUMN, &url, -1);

	confirm = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			                  GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE,
					  _("Are you sure you want to remove this URL?"));
	gtk_dialog_add_button (GTK_DIALOG (confirm), GTK_STOCK_CANCEL, GTK_RESPONSE_NO);
	gtk_dialog_add_button (GTK_DIALOG (confirm), GTK_STOCK_REMOVE, GTK_RESPONSE_YES);
	gtk_dialog_set_default_response (GTK_DIALOG (confirm), GTK_RESPONSE_CANCEL);

	response = gtk_dialog_run (GTK_DIALOG (confirm));
	gtk_widget_destroy (confirm);

	if (response == GTK_RESPONSE_YES) {
		int len;
		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);

		len = gtk_tree_model_iter_n_children (model, NULL);
		if (len > 0) {
			gtk_tree_selection_select_iter (selection, &iter);
		} else {
			gtk_widget_set_sensitive (ui->url_edit, FALSE);
			gtk_widget_set_sensitive (ui->url_remove, FALSE);
			gtk_widget_set_sensitive (ui->url_enable, FALSE);
		}

		g_free (url);
		url_list_changed (ui);
	}
}

static void
url_enable_clicked (GtkButton *button, PublishUIData *ui)
{
	EPublishUri *url = NULL;
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (ui->treeview));
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, URL_LIST_URL_COLUMN, &url, -1);
		url->enabled = !url->enabled;
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, URL_LIST_URL_COLUMN, url->enabled, -1);
		gtk_tree_selection_select_iter (selection, &iter);
		url_list_changed (ui);
	}
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
	GSList *l;
	GtkTreeIter iter;
	GConfClient *client;

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
	ui->url_edit = glade_xml_get_widget (xml, "url edit");
	ui->url_remove = glade_xml_get_widget (xml, "url remove");
	ui->url_enable = glade_xml_get_widget (xml, "url enable");
	g_signal_connect (G_OBJECT (ui->url_add), "clicked", G_CALLBACK (url_add_clicked), ui);
	g_signal_connect (G_OBJECT (ui->url_edit), "clicked", G_CALLBACK (url_edit_clicked), ui);
	g_signal_connect (G_OBJECT (ui->url_remove), "clicked", G_CALLBACK (url_remove_clicked), ui);
	g_signal_connect (G_OBJECT (ui->url_enable), "clicked", G_CALLBACK (url_enable_clicked), ui);
	gtk_widget_set_sensitive (GTK_WIDGET (ui->url_edit), FALSE);
	gtk_widget_set_sensitive (GTK_WIDGET (ui->url_remove), FALSE);
	gtk_widget_set_sensitive (GTK_WIDGET (ui->url_enable), FALSE);

	client = gconf_client_get_default ();
	l = publish_uris;
	while (l) {
		EPublishUri *url = (EPublishUri *) l->data;

		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
				    URL_LIST_ENABLED_COLUMN, url->enabled,
				    URL_LIST_LOCATION_COLUMN, url->location,
				    URL_LIST_URL_COLUMN, url, -1);

		l = g_slist_next (l);
	}
	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &iter))
		gtk_tree_selection_select_iter (selection, &iter);

	toplevel = glade_xml_get_widget (xml, "toplevel");
	gtk_widget_show_all (toplevel);
	gtk_box_pack_start (GTK_BOX (data->parent), toplevel, FALSE, TRUE, 0);

	g_object_unref (xml);

	return toplevel;
}

static void
publish (EPublishUri *uri)
{
}

int e_plugin_lib_enable (EPlugin *ep, int enable)
{
	GSList *uris, *l;
	GConfClient *client;

	if (enable) {
		client = gconf_client_get_default ();
		uris = gconf_client_get_list (client, "/apps/evolution/calendar/publish/uris", GCONF_VALUE_STRING, NULL);

		uri_timeouts = g_hash_table_new (g_direct_hash, g_direct_equal);

		l = uris;
		while (l) {
			gchar *xml = l->data;
			guint id = 0;

			EPublishUri *uri = e_publish_uri_from_xml (xml);

			if (!uri->location) {
				g_free (uri);
				continue;
			}

			publish_uris = g_slist_prepend (publish_uris, uri);

			/* Add a timeout - we always set it for now+frequency, since we
			 * publish later on in this function */
			switch (uri->publish_frequency) {
			case URI_PUBLISH_DAILY:
				id = g_timeout_add (24 * 60 * 60 * 1000, (GSourceFunc) publish, uri);
				g_hash_table_insert (uri_timeouts, uri, GUINT_TO_POINTER (id));
				break;
			case URI_PUBLISH_WEEKLY:
				id = g_timeout_add (7 * 24 * 60 * 60 * 1000, (GSourceFunc) publish, uri);
				g_hash_table_insert (uri_timeouts, uri, GUINT_TO_POINTER (id));
				break;
			}

			publish (uri);

			l = g_slist_next (l);
		}
		g_slist_foreach (uris, (GFunc) g_free, NULL);
		g_slist_free (uris);

		g_object_unref (client);
	}

	return 0;
}
