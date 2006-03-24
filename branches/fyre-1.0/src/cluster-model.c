/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * cluster-model.c - A cluster manager, based on GtkListStore. This can be
 *                   used on its own, or it can be attached to a suitable
 *                   GUI view and controller.
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
#include "cluster-model.h"
#include <stdlib.h>
#include <string.h>

typedef void      (*ClusterForeachCallback)   (ClusterModel*         self,
					       RemoteClient*         client,
					       gpointer              user_data);

static void       cluster_model_class_init    (ClusterModelClass*    klass);
static void       cluster_model_init          (ClusterModel*         self);
static void       cluster_model_dispose       (GObject*              gobject);

static void       client_speed_callback       (RemoteClient*  client,
					       double         iters_per_sec,
					       double         bytes_per_sec,
					       gpointer       user_data);
static void       client_status_callback      (RemoteClient*  client,
					       const gchar*   status,
					       gpointer       user_data);

static void       on_param_notify             (ParameterHolder* holder,
					       GParamSpec*      spec,
					       ClusterModel*    self);
static void       on_calc_finished            (IterativeMap*  map,
					       ClusterModel*  self);
static void       on_calc_start               (IterativeMap*  map,
					       ClusterModel*  self);
static void       on_calc_stop                (IterativeMap*  map,
					       ClusterModel*  self);

static void       cluster_foreach_node        (ClusterModel*  self,
					       ClusterForeachCallback callback,
					       gpointer       user_data,
					       gboolean       only_ready_nodes);
static void       cluster_node_update_param   (ClusterModel  *self,
					       RemoteClient  *client,
					       gpointer       user_data);
static void       cluster_node_start          (ClusterModel  *self,
					       RemoteClient  *client,
					       gpointer       user_data);
static void       cluster_node_stop           (ClusterModel  *self,
					       RemoteClient  *client,
					       gpointer       user_data);
static void       cluster_node_merge_results  (ClusterModel  *self,
					       RemoteClient  *client,
					       gpointer       user_data);
static void       cluster_node_set_min_stream_interval (ClusterModel  *self,
							RemoteClient  *client,
							gpointer       user_data);
static void       cluster_model_discovery_callback     (DiscoveryClient* self,
							const gchar*     host,
							int              port,
							gpointer         user_data);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType cluster_model_get_type(void)
{
    static GType anim_type = 0;

    if (!anim_type) {
	static const GTypeInfo dj_info = {
	    sizeof(ClusterModelClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) cluster_model_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(ClusterModel),
	    0,
	    (GInstanceInitFunc) cluster_model_init,
	};

	anim_type = g_type_register_static(GTK_TYPE_LIST_STORE, "ClusterModel", &dj_info, 0);
    }

    return anim_type;
}

static void cluster_model_class_init(ClusterModelClass *klass)
{
    GObjectClass *object_class;
    object_class = (GObjectClass*) klass;

    object_class->dispose      = cluster_model_dispose;
}

static void cluster_model_dispose(GObject *gobject)
{
    ClusterModel *self = CLUSTER_MODEL(gobject);

    cluster_model_disable_discovery(self);

    if (self->master_map) {
	g_object_set_data(G_OBJECT(self->master_map), "ClusterModel", NULL);

	g_signal_handlers_disconnect_matched(self->master_map,
					     G_SIGNAL_MATCH_DATA,
					     0, 0, NULL, NULL, self);

	g_object_unref(self->master_map);
	self->master_map = NULL;
    }
}

static void cluster_model_init(ClusterModel *self)
{
}

ClusterModel*  cluster_model_new              (IterativeMap*         master_map)
{
    ClusterModel *self = CLUSTER_MODEL(g_object_new(cluster_model_get_type(), NULL));
    GType types[] = {
	/* CLUSTER_MODEL_ENABLED     */  G_TYPE_BOOLEAN,
	/* CLUSTER_MODEL_HOSTNAME    */  G_TYPE_STRING,
	/* CLUSTER_MODEL_PORT        */  G_TYPE_INT,
	/* CLUSTER_MODEL_STATUS      */  G_TYPE_STRING,
	/* CLUSTER_MODEL_CLIENT      */  G_TYPE_OBJECT,
	/* CLUSTER_MODEL_SPEED       */  G_TYPE_STRING,
	/* CLUSTER_MODEL_BANDWIDTH   */  G_TYPE_STRING,
    };

    gtk_list_store_set_column_types(GTK_LIST_STORE(self), 7, types);

    self->master_map = g_object_ref(master_map);

    g_signal_connect(self->master_map, "notify",               G_CALLBACK(on_param_notify),  self);
    g_signal_connect(self->master_map, "calculation-finished", G_CALLBACK(on_calc_finished), self);
    g_signal_connect(self->master_map, "calculation-start",    G_CALLBACK(on_calc_start),    self);
    g_signal_connect(self->master_map, "calculation-stop",     G_CALLBACK(on_calc_stop),     self);

    g_object_set_data(G_OBJECT(self->master_map), "ClusterModel", self);

    return self;
}

ClusterModel*  cluster_model_get              (IterativeMap*         master_map,
					       gboolean              allocate_if_necessary)
{
    ClusterModel *self = g_object_get_data(G_OBJECT(master_map), "ClusterModel");
    if (self) {
	return g_object_ref(self);
    }
    else {
	if (allocate_if_necessary)
	    return cluster_model_new(master_map);
	else
	    return NULL;
    }
}


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

RemoteClient*  cluster_model_add_node         (ClusterModel*         self,
					       const gchar*          hostname,
					       gint                  port)
{
    GtkTreeIter iter;

    gtk_list_store_append(GTK_LIST_STORE(self), &iter);
    gtk_list_store_set(GTK_LIST_STORE(self), &iter,
		       CLUSTER_MODEL_HOSTNAME,  hostname,
		       CLUSTER_MODEL_PORT,      port,
		       -1);

    return cluster_model_enable_node(self, &iter);
}

void           cluster_model_add_nodes        (ClusterModel*         self,
					       const gchar*          hosts)
{
    gchar **tokens = g_strsplit(hosts, ",", 0);
    gchar **token;

    for (token=tokens; *token; token++) {
	gchar **parts;
	gchar *host;
	int port;
	gchar *first_invalid;

	g_strstrip(*token);

	/* See if we can split it into host:port */
	parts = g_strsplit(*token, ":", 2);
	host = parts[0];
	if (parts[1]) {
	    /* We have host and port */
	    port = strtol(parts[1], &first_invalid, 10);
	    if (*first_invalid) {
		g_warning("Port number '%s' is invalid", parts[1]);
	    }
	}
	else {
	    port = FYRE_DEFAULT_PORT;
	}

	cluster_model_add_node(self, host, port);

	g_strfreev(parts);
    }
    g_strfreev(tokens);
}

gboolean       cluster_model_find_client      (ClusterModel*         self,
					       RemoteClient*         client,
					       GtkTreeIter*          iter)
{
    RemoteClient* iter_client;

    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(self), iter)) {
	do {
	    gtk_tree_model_get(GTK_TREE_MODEL(self), iter,
			       CLUSTER_MODEL_CLIENT, &iter_client,
			       -1);
	    if (iter_client)
		g_object_unref(iter_client);

	    if (iter_client == client)
		return TRUE;

	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(self), iter));
    }

    return FALSE;
}

gboolean       cluster_model_find_address     (ClusterModel*         self,
					       const gchar*          hostname,
					       gint                  port,
					       GtkTreeIter*          iter)
{
    const gchar* iter_host;
    gint iter_port;

    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(self), iter)) {
	do {
	    gtk_tree_model_get(GTK_TREE_MODEL(self), iter,
			       CLUSTER_MODEL_HOSTNAME, &iter_host,
			       CLUSTER_MODEL_PORT, &iter_port,
			       -1);

	    /* FIXME: strcmp isn't good enough, we should really be checking
	     *        whether the hostnames refer to the same internet
	     *        address.
	     */
	    if (port == iter_port && !strcmp(iter_host, hostname))
		return TRUE;

	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(self), iter));
    }

    return FALSE;
}

void           cluster_model_remove_node      (ClusterModel*         self,
					       GtkTreeIter*          iter)
{
    cluster_model_disable_node(self, iter);
    gtk_list_store_remove(GTK_LIST_STORE(self), iter);
}

RemoteClient*  cluster_model_enable_node      (ClusterModel*         self,
					       GtkTreeIter*          iter)
{
    RemoteClient *client;
    gchar *host;
    int port;

    gtk_tree_model_get(GTK_TREE_MODEL(self), iter,
		       CLUSTER_MODEL_HOSTNAME, &host,
		       CLUSTER_MODEL_PORT, &port,
		       -1);

    client = remote_client_new(host, port);
    remote_client_set_status_cb(client, client_status_callback, self);
    remote_client_set_speed_cb(client, client_speed_callback, self);

    if (self->set_min_stream_interval)
	client->min_stream_interval = self->min_stream_interval;

    gtk_list_store_set(GTK_LIST_STORE(self), iter,
		       CLUSTER_MODEL_CLIENT, client,
		       CLUSTER_MODEL_ENABLED, TRUE,
		       -1);

    /* The liststore will keep a reference, ditch ours.
     * When the client is removed from the liststore, it
     * should be disposed of.
     */
    g_object_unref(client);

    /* Tell the client to connect only after our callbacks are in place */
    remote_client_connect(client);

    /* Return a borrowed reference, just in case the caller needs it */
    return client;
}

void           cluster_model_disable_node     (ClusterModel*         self,
					       GtkTreeIter*          iter)
{
    gtk_list_store_set(GTK_LIST_STORE(self), iter,
		       CLUSTER_MODEL_CLIENT, NULL,
		       CLUSTER_MODEL_ENABLED, FALSE,
		       CLUSTER_MODEL_STATUS, "",
		       CLUSTER_MODEL_SPEED, "",
		       CLUSTER_MODEL_BANDWIDTH, "",
		       -1);
}

void           cluster_model_show_status      (ClusterModel*         self)
{
    GtkTreeIter iter;
    gchar* host;
    gboolean enabled;
    int port;
    gchar* status;
    gchar* speed;
    gchar* bandwidth;
    gchar* host_and_port;

    /* Iterate over all cluster nodes that are ready */
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(self), &iter)) {
	do {
	    gtk_tree_model_get(GTK_TREE_MODEL(self), &iter,
			       CLUSTER_MODEL_ENABLED, &enabled,
			       CLUSTER_MODEL_HOSTNAME, &host,
			       CLUSTER_MODEL_PORT, &port,
			       CLUSTER_MODEL_STATUS, &status,
			       CLUSTER_MODEL_SPEED, &speed,
			       CLUSTER_MODEL_BANDWIDTH, &bandwidth,
			       -1);
	    if (enabled) {

		if (port == FYRE_DEFAULT_PORT)
		    host_and_port = g_strdup(host);
		else
		    host_and_port = g_strdup_printf("%s:%d", host, port);

		/* These widths were chosen to line up somewhat with batch-rendering results */
		printf("  %-19s %-17s %16s [%s]\n",
		       host_and_port,
		       speed ? speed : "",
		       bandwidth ? bandwidth : "",
		       status);

		g_free(host_and_port);
	    }
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(self), &iter));
    }
}

void           cluster_model_set_min_stream_interval (ClusterModel*  self,
						      gdouble        seconds)
{
    self->min_stream_interval = seconds;
    self->set_min_stream_interval = TRUE;
    cluster_foreach_node(self, cluster_node_set_min_stream_interval, NULL, FALSE);
}

void           cluster_model_enable_discovery (ClusterModel* self)
{
    /* Currently, our scanning interval is hardcoded at 5 minutes */
    if (!self->discovery)
	self->discovery = discovery_client_new(FYRE_DEFAULT_SERVICE, 5*60,
					       cluster_model_discovery_callback,
					       self);
}

void           cluster_model_disable_discovery(ClusterModel* self)
{
    if (self->discovery) {
	g_object_unref(self->discovery);
	self->discovery = NULL;
    }
}


/************************************************************************************/
/************************************************************************ Callbacks */
/************************************************************************************/

static void      client_status_callback          (RemoteClient*  client,
						  const gchar*   status,
						  gpointer       user_data)
{
    ClusterModel*self = CLUSTER_MODEL(user_data);
    GtkTreeIter iter;

    /* This node might have just become ready. If our cluster is supposed to
     * be running, make sure this node is running too.
     */
    if (self->is_running && remote_client_is_ready(client)) {
	remote_client_send_all_params(client, PARAMETER_HOLDER(self->master_map));
	cluster_node_start(self, client, NULL);
    }

    cluster_model_find_client(self, client, &iter);

    gtk_list_store_set(GTK_LIST_STORE(self), &iter,
		       CLUSTER_MODEL_STATUS, status,
		       -1);
}

static void      client_speed_callback           (RemoteClient*  client,
						  double         iters_per_sec,
						  double         bytes_per_sec,
						  gpointer       user_data)
{
    ClusterModel*self = CLUSTER_MODEL(user_data);
    GtkTreeIter iter;
    gchar* speed_str;
    gchar* bandwidth_str;

    cluster_model_find_client(self, client, &iter);

    speed_str = g_strdup_printf("%.3e iter/s", iters_per_sec);
    bandwidth_str = g_strdup_printf("%.2f KB/s", bytes_per_sec / 1000);

    gtk_list_store_set(GTK_LIST_STORE(self), &iter,
		       CLUSTER_MODEL_SPEED, speed_str,
		       CLUSTER_MODEL_BANDWIDTH, bandwidth_str,
		       -1);
    g_free(speed_str);
    g_free(bandwidth_str);
}

static void       on_param_notify             (ParameterHolder* holder,
					       GParamSpec*      spec,
					       ClusterModel*    self)
{
    cluster_foreach_node(self, cluster_node_update_param, spec, TRUE);
}

static void       on_calc_finished            (IterativeMap*  map,
					       ClusterModel*  self)
{
    cluster_foreach_node(self, cluster_node_merge_results, NULL, TRUE);
}

static void       on_calc_start               (IterativeMap*  map,
					       ClusterModel*  self)
{
    self->is_running = TRUE;
    cluster_foreach_node(self, cluster_node_start, NULL, TRUE);
}

static void       on_calc_stop                (IterativeMap*  map,
					       ClusterModel*  self)
{
    self->is_running = FALSE;
    cluster_foreach_node(self, cluster_node_stop, NULL, TRUE);
}

static void       cluster_model_discovery_callback (DiscoveryClient* client,
						    const gchar*     host,
						    int              port,
						    gpointer         user_data)
{
    /* This is called by our DiscoveryClient when it's found a cluster
     * node. We may or may not already have this node- if we don't,
     * it gets added.
     */
    GtkTreeIter iter;
    ClusterModel* self = CLUSTER_MODEL(user_data);
    g_return_if_fail(client == self->discovery);

    if (cluster_model_find_address(self, host, port, &iter)) {
	/* Already found this host */
	return;
    }

    /* Yay, a new node */
    cluster_model_add_node(self, host, port);
}


/************************************************************************************/
/****************************************************************** Cluster Control */
/************************************************************************************/

static void      cluster_foreach_node            (ClusterModel*  self,
						  ClusterForeachCallback callback,
						  gpointer       user_data,
						  gboolean       only_ready_nodes)
{
    GtkTreeIter iter;
    RemoteClient* client;

    /* Iterate over all cluster nodes that are ready */
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(self), &iter)) {
	do {
	    gtk_tree_model_get(GTK_TREE_MODEL(self), &iter,
			       CLUSTER_MODEL_CLIENT, &client,
			       -1);
	    if (client) {
		if ((!only_ready_nodes) || remote_client_is_ready(client))
		    callback(self, client, user_data);
		g_object_unref(client);
	    }
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(self), &iter));
    }
}

static void      cluster_node_update_param       (ClusterModel  *self,
						  RemoteClient  *client,
						  gpointer       user_data)
{
    GParamSpec* spec = (GParamSpec*) user_data;
    remote_client_send_param(client, PARAMETER_HOLDER(self->master_map), spec->name);
}

static void      cluster_node_start              (ClusterModel  *self,
						  RemoteClient  *client,
						  gpointer       user_data)
{
    remote_client_command(client, NULL, NULL, "calc_start");
}

static void      cluster_node_stop               (ClusterModel  *self,
						  RemoteClient  *client,
						  gpointer       user_data)
{
    remote_client_command(client, NULL, NULL, "calc_stop");
}

static void      cluster_node_merge_results      (ClusterModel  *self,
						  RemoteClient  *client,
						  gpointer       user_data)
{
    remote_client_merge_results(client, self->master_map);
}

static void       cluster_node_set_min_stream_interval (ClusterModel  *self,
							RemoteClient  *client,
							gpointer       user_data)
{
    client->min_stream_interval = self->min_stream_interval;
}

/* The End */
