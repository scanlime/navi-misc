/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * explorer-cluster.c - Implements the explorer GUI's clustering support.
 *                      This is only compiled when we have Gnet support.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
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

#include "explorer.h"
#include "remote-client.h"
#include <gtk/gtk.h>
#include <stdlib.h>

static void      explorer_init_cluster_view      (Explorer *self);
static gboolean  explorer_validate_host_and_port (Explorer *self);
static void      explorer_set_port               (Explorer *self,
						  int       port);

static void      explorer_get_client_iter        (Explorer      *self,
						  RemoteClient  *client,
						  GtkTreeIter   *iter);

static void      cluster_foreach_ready_node      (Explorer    *self,
						  void       (*callback)(Explorer *, RemoteClient*, gpointer),
						  gpointer     user_data);

static void      cluster_node_enable             (Explorer    *self,
						  GtkTreeIter *iter);
static void      cluster_node_disable            (Explorer    *self,
						  GtkTreeIter *iter);

static void      cluster_node_update_param       (Explorer      *self,
						  RemoteClient  *client,
						  gpointer       user_data);
static void      cluster_node_start              (Explorer      *self,
						  RemoteClient  *client,
						  gpointer       user_data);
static void      cluster_node_stop               (Explorer      *self,
						  RemoteClient  *client,
						  gpointer       user_data);
static void      cluster_node_merge_results      (Explorer      *self,
						  RemoteClient  *client,
						  gpointer       user_data);

static void      client_status_callback          (RemoteClient*  client,
						  const gchar*   status,
						  gpointer       user_data);
static void      client_speed_callback           (RemoteClient*  client,
						  double         iters_per_sec,
						  double         bytes_per_sec,
						  gpointer       user_data);

static void      on_cluster_list_cursor_changed  (GtkWidget *widget, gpointer user_data);
static void      on_cluster_add_host             (GtkWidget *widget, gpointer user_data);
static void      on_cluster_remove_host          (GtkWidget *widget, gpointer user_data);
static void      on_cluster_host_or_port_changed (GtkWidget *widget, gpointer user_data);
static void      on_node_enabled_toggled         (GtkCellRendererToggle *cell_renderer,
						  gchar *path, gpointer user_data);
static void      on_param_notify                 (ParameterHolder *holder, GParamSpec *spec, gpointer user_data);

enum {
    CLUSTER_MODEL_ENABLED,
    CLUSTER_MODEL_HOSTNAME,
    CLUSTER_MODEL_PORT,
    CLUSTER_MODEL_STATUS,
    CLUSTER_MODEL_CLIENT,
    CLUSTER_MODEL_SPEED,
    CLUSTER_MODEL_BANDWIDTH,
};


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

    g_signal_connect(self->map, "notify", G_CALLBACK(on_param_notify), self);

    explorer_init_cluster_view(self);
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

    self->cluster_model = gtk_list_store_new(7,
					     G_TYPE_BOOLEAN,     /* CLUSTER_MODEL_ENABLED    */
					     G_TYPE_STRING,      /* CLUSTER_MODEL_HOSTNAME   */
					     G_TYPE_INT,         /* CLUSTER_MODEL_PORT       */
					     G_TYPE_STRING,      /* CLUSTER_MODEL_STATUS     */
					     G_TYPE_OBJECT,      /* CLUSTER_MODEL_CLIENT     */
					     G_TYPE_STRING,      /* CLUSTER_MODEL_SPEED      */
					     G_TYPE_STRING);     /* CLUSTER_MODEL_BANDWIDTH  */

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
/****************************************************************** Cluster Control */
/************************************************************************************/

static void      cluster_foreach_ready_node      (Explorer    *self,
						  void       (*callback)(Explorer *, RemoteClient*, gpointer),
						  gpointer     user_data)
{
    GtkTreeIter iter;
    RemoteClient* client;

    /* Iterate over all cluster nodes that are ready */
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(self->cluster_model), &iter)) {
	do {
	    gtk_tree_model_get(GTK_TREE_MODEL(self->cluster_model), &iter,
			       CLUSTER_MODEL_CLIENT, &client,
			       -1);
	    if (client) {
		if (remote_client_is_ready(client))
		    callback(self, client, user_data);
		g_object_unref(client);
	    }
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(self->cluster_model), &iter));
    }
}

static void on_param_notify(ParameterHolder *holder, GParamSpec *spec, gpointer user_data)
{
    Explorer *self = EXPLORER(user_data);
    cluster_foreach_ready_node(self, cluster_node_update_param, spec);
}

void      explorer_cluster_start         (Explorer *self)
{
    self->cluster_running = TRUE;
    cluster_foreach_ready_node(self, cluster_node_start, NULL);
}

void      explorer_cluster_stop          (Explorer *self)
{
    self->cluster_running = FALSE;
    cluster_foreach_ready_node(self, cluster_node_stop, NULL);
}

void      explorer_cluster_merge_results (Explorer *self)
{
    cluster_foreach_ready_node(self, cluster_node_merge_results, NULL);
}

static void      cluster_node_update_param       (Explorer      *self,
						  RemoteClient  *client,
						  gpointer       user_data)
{
    GParamSpec* spec = (GParamSpec*) user_data;
    remote_client_send_param(client, PARAMETER_HOLDER(self->map), spec->name);
}

static void      cluster_node_start              (Explorer      *self,
						  RemoteClient  *client,
						  gpointer       user_data)
{
    remote_client_command(client, NULL, NULL, "calc_start");
}

static void      cluster_node_stop               (Explorer      *self,
						  RemoteClient  *client,
						  gpointer       user_data)
{
    remote_client_command(client, NULL, NULL, "calc_stop");
}

static void      cluster_node_merge_results      (Explorer      *self,
						  RemoteClient  *client,
						  gpointer       user_data)
{
    remote_client_merge_results(client, self->map);
}


/************************************************************************************/
/******************************************************************** GUI Callbacks */
/************************************************************************************/

static void on_cluster_list_cursor_changed(GtkWidget *widget, gpointer user_data)
{
    Explorer *self = EXPLORER(user_data);
    gtk_widget_set_sensitive(glade_xml_get_widget(self->xml, "cluster_remove_host"), TRUE);
}

static void      explorer_get_client_iter        (Explorer      *self,
						  RemoteClient  *client,
						  GtkTreeIter   *iter)
{
    RemoteClient* iter_client;

    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(self->cluster_model), iter)) {
	do {
	    gtk_tree_model_get(GTK_TREE_MODEL(self->cluster_model), iter,
			       CLUSTER_MODEL_CLIENT, &iter_client,
			       -1);
	    if (iter_client)
		g_object_unref(iter_client);
	    if (iter_client == client)
		return;
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(self->cluster_model), iter));
    }
    g_assert_not_reached();
}


static void      client_status_callback          (RemoteClient*  client,
						  const gchar*   status,
						  gpointer       user_data)
{
    Explorer *self = EXPLORER(user_data);
    GtkTreeIter iter;

    /* This node might have just become ready. If our cluster is supposed to
     * be running, make sure this node is running too.
     */
    if (self->cluster_running && remote_client_is_ready(client)) {
	remote_client_send_all_params(client, PARAMETER_HOLDER(self->map));
	cluster_node_start(self, client, NULL);
    }

    explorer_get_client_iter(self, client, &iter);

    gtk_list_store_set(self->cluster_model, &iter,
		       CLUSTER_MODEL_STATUS, status,
		       -1);
}

static void      client_speed_callback           (RemoteClient*  client,
						  double         iters_per_sec,
						  double         bytes_per_sec,
						  gpointer       user_data)
{
    Explorer *self = EXPLORER(user_data);
    GtkTreeIter iter;
    gchar* speed_str;
    gchar* bandwidth_str;

    explorer_get_client_iter(self, client, &iter);

    speed_str = g_strdup_printf("%.3e iter/s", iters_per_sec);
    bandwidth_str = g_strdup_printf("%.2f KB/s", bytes_per_sec / 1000);

    gtk_list_store_set(self->cluster_model, &iter,
		       CLUSTER_MODEL_SPEED, speed_str,
		       CLUSTER_MODEL_BANDWIDTH, bandwidth_str,
		       -1);
    g_free(speed_str);
    g_free(bandwidth_str);
}

static void      cluster_node_enable             (Explorer    *self,
						  GtkTreeIter *iter)
{
    RemoteClient *client;
    gchar *host;
    int port;

    gtk_tree_model_get(GTK_TREE_MODEL(self->cluster_model), iter,
		       CLUSTER_MODEL_HOSTNAME, &host,
		       CLUSTER_MODEL_PORT, &port,
		       -1);

    client = remote_client_new(host, port);
    remote_client_set_status_cb(client, client_status_callback, self);
    remote_client_set_speed_cb(client, client_speed_callback, self);

    gtk_list_store_set(self->cluster_model, iter,
		       CLUSTER_MODEL_CLIENT, client,
		       CLUSTER_MODEL_ENABLED, TRUE,
		       -1);

    /* The liststore will keep a reference, ditch ours.
     * When the client is removed from the liststore, it
     * should be disposed of.
     */
    g_object_unref(client);

    remote_client_connect(client);
}

static void      cluster_node_disable            (Explorer    *self,
						  GtkTreeIter *iter)
{
    gtk_list_store_set(self->cluster_model, iter,
		       CLUSTER_MODEL_CLIENT, NULL,
		       CLUSTER_MODEL_ENABLED, FALSE,
		       CLUSTER_MODEL_STATUS, "",
		       CLUSTER_MODEL_SPEED, "",
		       CLUSTER_MODEL_BANDWIDTH, "",
		       -1);
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
	cluster_node_disable(self, &iter);
    else
	cluster_node_enable(self, &iter);
}

static void on_cluster_add_host(GtkWidget *widget, gpointer user_data)
{
    Explorer *self = EXPLORER(user_data);
    const gchar* host = gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(self->xml, "cluster_hostname")));
    int port = atoi(gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(self->xml, "cluster_port"))));
    GtkTreeIter iter;

    gtk_list_store_append(self->cluster_model, &iter);
    gtk_list_store_set(self->cluster_model, &iter,
		       CLUSTER_MODEL_HOSTNAME,  host,
		       CLUSTER_MODEL_PORT,      port,
		       -1);

    cluster_node_enable(self, &iter);

    gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(self->xml, "cluster_hostname")), "");
}

static void on_cluster_remove_host(GtkWidget *widget, gpointer user_data)
{
    Explorer *self = EXPLORER(user_data);
    GtkTreeView *tv = GTK_TREE_VIEW(glade_xml_get_widget(self->xml, "cluster_list"));
    GtkTreeIter iter;
    GtkTreePath *path;
    gboolean enabled;

    /* Get an iter to the selected row */
    gtk_tree_view_get_cursor(tv, &path, NULL);
    gtk_tree_model_get_iter(GTK_TREE_MODEL(self->cluster_model), &iter, path);
    gtk_tree_path_free(path);

    /* If it's enabled, fix that */
    gtk_tree_model_get(GTK_TREE_MODEL(self->cluster_model), &iter,
		       CLUSTER_MODEL_ENABLED, &enabled,
		       -1);
    if (enabled)
	cluster_node_disable(self, &iter);

    /* Remove it and insensitize the removal button */
    gtk_widget_set_sensitive(glade_xml_get_widget(self->xml, "cluster_remove_host"), FALSE);
    gtk_list_store_remove(self->cluster_model, &iter);
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

/* The End */
