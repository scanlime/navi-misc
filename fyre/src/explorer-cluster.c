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

static void explorer_init_cluster_view(Explorer *self);

static void on_cluster_list_cursor_changed(GtkWidget *widget, gpointer user_data);
static void on_cluster_add_host(GtkWidget *widget, gpointer user_data);
static void on_cluster_remove_host(GtkWidget *widget, gpointer user_data);
static void on_cluster_hostname_changed(GtkWidget *widget, gpointer user_data);
static void on_cluster_port_changed(GtkWidget *widget, gpointer user_data);

enum {
    CLUSTER_MODEL_ENABLED,
    CLUSTER_MODEL_HOSTNAME,
    CLUSTER_MODEL_PORT,
    CLUSTER_MODEL_STATUS,
    CLUSTER_MODEL_CLIENT,
};


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

void explorer_init_cluster(Explorer *self)
{
    /* Connect signal handlers
     */
    glade_xml_signal_connect_data(self->xml, "on_cluster_list_cursor_changed",   G_CALLBACK(on_cluster_list_cursor_changed), self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_add_host",              G_CALLBACK(on_cluster_add_host),            self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_remove_host",           G_CALLBACK(on_cluster_remove_host),         self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_hostname_changed",      G_CALLBACK(on_cluster_hostname_changed),    self);
    glade_xml_signal_connect_data(self->xml, "on_cluster_port_changed",          G_CALLBACK(on_cluster_port_changed),        self);

    explorer_init_cluster_view(self);
}

void explorer_dispose_cluster(Explorer *self)
{
}

static void explorer_init_cluster_view(Explorer *self) {
    GtkTreeView *tv = GTK_TREE_VIEW(glade_xml_get_widget(self->xml, "cluster_list"));
    GtkTreeViewColumn *col;
    GtkListStore *model;

    model = gtk_list_store_new(5,
			       G_TYPE_BOOLEAN,     /* CLUSTER_MODEL_ENABLED    */
			       G_TYPE_STRING,      /* CLUSTER_MODEL_HOSTNAME   */
			       G_TYPE_INT,         /* CLUSTER_MODEL_PORT       */
			       G_TYPE_STRING,      /* CLUSTER_MODEL_STATUS     */
			       G_TYPE_OBJECT);     /* CLUSTER_MODEL_CLIENT     */

    gtk_tree_view_set_model(tv, GTK_TREE_MODEL(model));

    gtk_tree_view_insert_column_with_attributes(tv, -1, "Enabled",
						gtk_cell_renderer_toggle_new(),
						"active", CLUSTER_MODEL_ENABLED,
						NULL);

    gtk_tree_view_insert_column_with_attributes(tv, -1, "Hostname",
						gtk_cell_renderer_text_new(),
						"active", CLUSTER_MODEL_HOSTNAME,
						NULL);

    gtk_tree_view_insert_column_with_attributes(tv, -1, "Port",
						gtk_cell_renderer_text_new(),
						"active", CLUSTER_MODEL_PORT,
						NULL);

    gtk_tree_view_insert_column_with_attributes(tv, -1, "Status",
						gtk_cell_renderer_text_new(),
						"active", CLUSTER_MODEL_STATUS,
						NULL);
}


/************************************************************************************/
/************************************************************************ Callbacks */
/************************************************************************************/

static void on_cluster_list_cursor_changed(GtkWidget *widget, gpointer user_data)
{
}

static void on_cluster_add_host(GtkWidget *widget, gpointer user_data)
{
}

static void on_cluster_remove_host(GtkWidget *widget, gpointer user_data)
{
}

static void on_cluster_hostname_changed(GtkWidget *widget, gpointer user_data)
{
}

static void on_cluster_port_changed(GtkWidget *widget, gpointer user_data)
{
}

/* The End */
