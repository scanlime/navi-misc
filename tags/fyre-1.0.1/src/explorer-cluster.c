/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * explorer-cluster.c - Implements the explorer GUI's clustering support.
 *                      This is only compiled when we have Gnet support.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

#include "config.h"
#include "explorer.h"
#include "cluster-model.h"
#include <gtk/gtk.h>
#include <stdlib.h>

static void      explorer_init_cluster_view      (Explorer *self);
static gboolean  explorer_validate_host_and_port (Explorer *self);
static void      explorer_set_port               (Explorer *self,
						  int       port);

static void      on_cluster_list_cursor_changed  (GtkWidget *widget, gpointer user_data);
static void      on_cluster_add_host             (GtkWidget *widget, gpointer user_data);
static void      on_cluster_remove_host          (GtkWidget *widget, gpointer user_data);
static void      on_cluster_host_or_port_changed (GtkWidget *widget, gpointer user_data);
static void      on_cluster_merge_time_changed   (GtkWidget *widget, gpointer user_data);
static void      on_cluster_discovery_toggled    (GtkWidget *widget, gpointer user_data);
static void      on_node_enabled_toggled         (GtkCellRendererToggle *cell_renderer,
						  gchar *path, gpointer user_data);
static gboolean  on_cluster_window_delete        (GtkWidget *widget, GdkEvent *event,
						  gpointer user_data);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

void explorer_init_cluster(Explorer *self)
{
    /* Connect signal handlers
     */
    glade_xml_signal_connect_data(self->xml, "on_cluster_list_cursor_changed",   G_CALLBACK(on_cluster_list_cursor_changed),  self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_add_host",              G_CALLBACK(on_cluster_add_host),             self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_remove_host",           G_CALLBACK(on_cluster_remove_host),          self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_host_or_port_changed",  G_CALLBACK(on_cluster_host_or_port_changed), self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_host_or_port_changed",  G_CALLBACK(on_cluster_host_or_port_changed), self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_merge_time_changed",    G_CALLBACK(on_cluster_merge_time_changed),   self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_window_delete",         G_CALLBACK(on_cluster_window_delete),        self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_discovery_toggled",     G_CALLBACK(on_cluster_discovery_toggled),    self);

    self->cluster_model = cluster_model_get(self->map, TRUE);
    explorer_init_cluster_view(self);

    /* Set the initial merge time */
    on_cluster_merge_time_changed(glade_xml_get_widget(self->xml, "cluster_merge_time"), self);

    /* Set the initial status of node autodiscovery */
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(self->xml, "cluster_discovery")),
				 self->cluster_model->discovery != NULL ? TRUE : FALSE);

    explorer_set_port(self, FYRE_DEFAULT_PORT);
}

void explorer_dispose_cluster(Explorer *self)
{
    if (self->cluster_model) {
	g_object_unref(self->cluster_model);
	self->cluster_model = NULL;
    }
}

static void explorer_init_cluster_view(Explorer *self) {
    GtkTreeView *tv = GTK_TREE_VIEW(glade_xml_get_widget(self->xml, "cluster_list"));
    GtkCellRenderer *renderer;

    gtk_tree_view_set_model(tv, GTK_TREE_MODEL(self->cluster_model));

    renderer = gtk_cell_renderer_toggle_new();
    g_signal_connect(renderer, "toggled", G_CALLBACK(on_node_enabled_toggled), self);
    gtk_tree_view_insert_column_with_attributes(tv, -1, "Enabled",
						renderer,
						"active", CLUSTER_MODEL_ENABLED,
						NULL);

    gtk_tree_view_insert_column_with_attributes(tv, -1, "Hostname",
						gtk_cell_renderer_text_new(),
						"text", CLUSTER_MODEL_HOSTNAME,
						NULL);

    gtk_tree_view_insert_column_with_attributes(tv, -1, "Port",
						gtk_cell_renderer_text_new(),
						"text", CLUSTER_MODEL_PORT,
						NULL);

    gtk_tree_view_insert_column_with_attributes(tv, -1, "Status",
						gtk_cell_renderer_text_new(),
						"text", CLUSTER_MODEL_STATUS,
						NULL);

    gtk_tree_view_insert_column_with_attributes(tv, -1, "Speed",
						gtk_cell_renderer_text_new(),
						"text", CLUSTER_MODEL_SPEED,
						NULL);

    gtk_tree_view_insert_column_with_attributes(tv, -1, "Bandwidth",
						gtk_cell_renderer_text_new(),
						"text", CLUSTER_MODEL_BANDWIDTH,
						NULL);
}


/************************************************************************************/
/******************************************************************** GUI Callbacks */
/************************************************************************************/

static gboolean on_cluster_window_delete(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    /* Just hide the window when the user tries to close it */
    Explorer *self = EXPLORER(user_data);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(self->xml, "toggle_cluster_window")), FALSE);
    return TRUE;
}

static void on_cluster_list_cursor_changed(GtkWidget *widget, gpointer user_data)
{
    Explorer *self = EXPLORER(user_data);
    gtk_widget_set_sensitive(glade_xml_get_widget(self->xml, "cluster_remove_host"), TRUE);
}

static void on_node_enabled_toggled(GtkCellRendererToggle *cell_renderer, gchar *path, gpointer user_data)
{
    GtkTreePath *path_obj;
    GtkTreeIter iter;
    Explorer *self = EXPLORER(user_data);
    gboolean enabled;

    path_obj = gtk_tree_path_new_from_string(path);
    gtk_tree_model_get_iter(GTK_TREE_MODEL(self->cluster_model), &iter, path_obj);
    gtk_tree_path_free(path_obj);

    gtk_tree_model_get(GTK_TREE_MODEL(self->cluster_model), &iter,
		       CLUSTER_MODEL_ENABLED, &enabled,
		       -1);

    if (enabled)
	cluster_model_disable_node(self->cluster_model, &iter);
    else
	cluster_model_enable_node(self->cluster_model, &iter);
}

static void on_cluster_add_host(GtkWidget *widget, gpointer user_data)
{
    Explorer *self = EXPLORER(user_data);
    const gchar* host = gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(self->xml, "cluster_hostname")));
    int port = atoi(gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(self->xml, "cluster_port"))));

    cluster_model_add_node(self->cluster_model, host, port);

    gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(self->xml, "cluster_hostname")), "");
    explorer_set_port(self, FYRE_DEFAULT_PORT);
    gtk_widget_grab_focus(glade_xml_get_widget(self->xml, "cluster_hostname"));
}

static void on_cluster_remove_host(GtkWidget *widget, gpointer user_data)
{
    Explorer *self = EXPLORER(user_data);
    GtkTreeView *tv = GTK_TREE_VIEW(glade_xml_get_widget(self->xml, "cluster_list"));
    GtkTreeIter iter;
    GtkTreePath *path;

    /* Get an iter to the selected row */
    gtk_tree_view_get_cursor(tv, &path, NULL);
    gtk_tree_model_get_iter(GTK_TREE_MODEL(self->cluster_model), &iter, path);
    gtk_tree_path_free(path);

    cluster_model_remove_node(self->cluster_model, &iter);

    gtk_widget_set_sensitive(glade_xml_get_widget(self->xml, "cluster_remove_host"), FALSE);
}

static void on_cluster_host_or_port_changed(GtkWidget *widget, gpointer user_data)
{
    /* Validate the host and port, enabling the 'add' button only if
     * they seem to be at least superficially valid.
     */
    Explorer *self = EXPLORER(user_data);
    gtk_widget_set_sensitive(glade_xml_get_widget(self->xml, "cluster_add_host"),
			     explorer_validate_host_and_port(self));
}

static gboolean explorer_validate_host_and_port(Explorer *self)
{
    const gchar* host = gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(self->xml, "cluster_hostname")));
    const gchar* port = gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(self->xml, "cluster_port")));
    gchar* first_invalid;
    int port_n;

    if (!*host)
	return FALSE;

    if (!*port)
	return FALSE;

    port_n = strtol(port, &first_invalid, 10);
    if (first_invalid[0] != '\0')
	return FALSE;

    if (port_n < 0 || port_n > 65535)
	return FALSE;

    return TRUE;
}

static void explorer_set_port(Explorer *self, int port)
{
    gchar* text = g_strdup_printf("%d", port);
    gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(self->xml, "cluster_port")), text);
    g_free(text);
}

static void on_cluster_merge_time_changed(GtkWidget *widget, gpointer user_data)
{
    Explorer *self = EXPLORER(user_data);
    cluster_model_set_min_stream_interval(self->cluster_model,
					  gtk_range_get_adjustment(GTK_RANGE(widget))->value);
}

static void on_cluster_discovery_toggled(GtkWidget *widget, gpointer user_data)
{
    Explorer *self = EXPLORER(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	cluster_model_enable_discovery(self->cluster_model);
    else
	cluster_model_disable_discovery(self->cluster_model);
}

/* The End */
