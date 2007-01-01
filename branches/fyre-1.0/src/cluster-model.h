/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * cluster-model.h - A cluster manager, based on GtkListStore. This can be
 *                   used on its own, or it can be attached to a suitable
 *                   GUI view and controller.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
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

#ifndef __CLUSTER_MODEL_H__
#define __CLUSTER_MODEL_H__

#include <gtk/gtk.h>
#include "remote-client.h"
#include "discovery-client.h"
#include "iterative-map.h"

G_BEGIN_DECLS

#define CLUSTER_MODEL_TYPE            (cluster_model_get_type ())
#define CLUSTER_MODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUSTER_MODEL_TYPE, ClusterModel))
#define CLUSTER_MODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CLUSTER_MODEL_TYPE, ClusterModelClass))
#define IS_CLUSTER_MODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUSTER_MODEL_TYPE))
#define IS_CLUSTER_MODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUSTER_MODEL_TYPE))

typedef struct _ClusterModel         ClusterModel;
typedef struct _ClusterModelClass    ClusterModelClass;

struct _ClusterModel {
    GtkListStore parent;

    IterativeMap* master_map;
    gboolean      is_running;

    gdouble       min_stream_interval;  /* Default for new clients */
    gboolean      set_min_stream_interval;

    DiscoveryClient* discovery;
};

struct _ClusterModelClass {
    GtkListStoreClass parent_class;
};

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
/******************************************************************* Public Methods */
/************************************************************************************/

GType          cluster_model_get_type         ();
ClusterModel*  cluster_model_new              (IterativeMap*         master_map);

/* Return a new reference to the ClusterModel associated with a particular
 * IterativeMap, optionally allocating a new one if necessary.
 */
ClusterModel*  cluster_model_get              (IterativeMap*         master_map,
					       gboolean              allocate_if_necessary);

RemoteClient*  cluster_model_add_node         (ClusterModel*         self,
					       const gchar*          hostname,
					       gint                  port);

void           cluster_model_set_min_stream_interval (ClusterModel*  self,
						      gdouble        seconds);

/* Show the cluster status on stdout. Good for debugging, and batch-mode rendering */
void           cluster_model_show_status      (ClusterModel*         self);

/* Add a comma-separated list of host[:port] specifiers */
void           cluster_model_add_nodes        (ClusterModel*         self,
					       const gchar*          hosts);

/* Enable/disable automatic autodiscovery of cluster nodes */
void           cluster_model_enable_discovery (ClusterModel*         self);
void           cluster_model_disable_discovery(ClusterModel*         self);

gboolean       cluster_model_find_client      (ClusterModel*         self,
					       RemoteClient*         client,
					       GtkTreeIter*          iter);
gboolean       cluster_model_find_address     (ClusterModel*         self,
					       const gchar*          hostname,
					       gint                  port,
					       GtkTreeIter*          iter);

void           cluster_model_remove_node      (ClusterModel*         self,
					       GtkTreeIter*          iter);
RemoteClient*  cluster_model_enable_node      (ClusterModel*         self,
					       GtkTreeIter*          iter);
void           cluster_model_disable_node     (ClusterModel*         self,
					       GtkTreeIter*          iter);

G_END_DECLS

#endif /* __CLUSTER_MODEL_H__ */

/* The End */
