/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * explorer-history.c - Implements a simple non-persistent history
 *                      tracker. History items can be recorded immediately
 *                      or on a timer. Each history item stores a timestamp,
 *                      parameters, and a small thumbnail. They are made
 *                      available to the user through a web-browser-like "Go" menu.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2005 David Trowbridge and Micah Dowty
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
#include "explorer.h"
#include "histogram-imager.h"

/* These are stored in the history_queue. The parameter string and
 * thumbnail are both owned by the history node.
 */
typedef struct {
    GTimeVal   timestamp;
    GdkPixbuf* thumbnail;
    gchar*     params;
} HistoryNode;

static HistoryNode* history_node_new   (HistogramImager* map);
static void         history_node_apply (HistoryNode* self, HistogramImager* map);
static void         history_node_free  (HistoryNode* self);

static void         on_go_back         (GtkWidget *widget,       Explorer *self);
static void         on_go_forward      (GtkWidget *widget,       Explorer *self);
static void         on_change_notify   (ParameterHolder *holder, GParamSpec* spec, Explorer *self);
static gboolean     on_record_change   (gpointer user_data);

static void         explorer_append_history (Explorer* self, HistoryNode *node);
static void         explorer_prune_history  (Explorer* self, int max_nodes);
static void         explorer_update_history_sensitivity (Explorer* self);


/************************************************************************************/
/***************************************************************** Public Methods ***/
/************************************************************************************/

void explorer_init_history(Explorer *self)
{
    self->history_queue = g_queue_new();

    /* Connect signal handlers
     */
    glade_xml_signal_connect_data(self->xml, "on_go_back",     G_CALLBACK(on_go_back),       self);
    glade_xml_signal_connect_data(self->xml, "on_go_forward",  G_CALLBACK(on_go_forward),    self);
    g_signal_connect             (self->map, "notify",         G_CALLBACK(on_change_notify), self);

    /* Put the defaults in our history */
    explorer_append_history(self, history_node_new(HISTOGRAM_IMAGER(self->map)));
}

void explorer_dispose_history(Explorer *self)
{
    if (self->history_timer) {
	g_source_remove(self->history_timer);
	self->history_timer = 0;
    }

    if (self->history_queue) {
	explorer_prune_history(self, 0);
	g_queue_free(self->history_queue);
	self->history_queue = NULL;
    }
}

void explorer_history_record_now(Explorer *self)
{
}

void explorer_history_record_timed(Explorer *self)
{
}


/************************************************************************************/
/****************************************************************** History Nodes ***/
/************************************************************************************/

static HistoryNode* history_node_new   (HistogramImager* map)
{
    HistoryNode* self = g_new0(HistoryNode, 1);
    gint width, height;

    self->params = parameter_holder_save_string(PARAMETER_HOLDER(map));

    gtk_icon_size_lookup(GTK_ICON_SIZE_MENU, &width, &height);
    self->thumbnail = histogram_imager_make_thumbnail(map, width, height);

    return self;
}

static void         history_node_apply (HistoryNode* self, HistogramImager* map)
{
    parameter_holder_load_string(PARAMETER_HOLDER(map), self->params);
}

static void         history_node_free  (HistoryNode* self)
{
    if (self->thumbnail) {
	gdk_pixbuf_unref(self->thumbnail);
	self->thumbnail = NULL;
    }
    if (self->params) {
	g_free(self->params);
	self->params = NULL;
    }
    g_free(self);
}


/************************************************************************************/
/******************************************************************** History Queue */
/************************************************************************************/

static void         explorer_append_history (Explorer* self, HistoryNode *node)
{
    g_queue_push_tail(self->history_queue, node);

    /* By my quick back-of-the-envelope calculations, this should
     * keep the history, at worst, at about 1MB of memory.
     */
    explorer_prune_history(self, 100);

    /* Our new node is now the current one for back/forward navigation */
    self->history_current_link = g_queue_peek_tail_link(self->history_queue);
    explorer_update_history_sensitivity(self);
}

static void         explorer_prune_history  (Explorer* self, int max_nodes)
{
    while (self->history_queue->length > max_nodes) {
	GList* link = g_queue_pop_head_link(self->history_queue);
	if (!link)
	    break;

	if (link == self->history_current_link)
	    self->history_current_link = NULL;

	history_node_free(link->data);
	g_list_free_1(link);
    }
}

static void         explorer_update_history_sensitivity (Explorer* self)
{
    gtk_widget_set_sensitive(glade_xml_get_widget(self->xml, "go_forward"),
			     self->history_current_link && self->history_current_link->next);
    gtk_widget_set_sensitive(glade_xml_get_widget(self->xml, "go_back"),
			     self->history_current_link && self->history_current_link->prev);
}


/************************************************************************************/
/******************************************************************** GUI Callbacks */
/************************************************************************************/

static void on_go_back       (GtkWidget *widget,       Explorer *self)
{
    self->history_current_link = self->history_current_link->prev;
    explorer_update_history_sensitivity(self);

    self->history_freeze = TRUE;
    history_node_apply(self->history_current_link->data, HISTOGRAM_IMAGER(self->map));
    self->history_freeze = FALSE;
}

static void on_go_forward    (GtkWidget *widget,       Explorer *self)
{
    self->history_current_link = self->history_current_link->next;
    explorer_update_history_sensitivity(self);

    self->history_freeze = TRUE;
    history_node_apply(self->history_current_link->data, HISTOGRAM_IMAGER(self->map));
    self->history_freeze = FALSE;
}

static void on_change_notify (ParameterHolder *holder, GParamSpec* spec, Explorer *self)
{
    /* In a history_freeze, we don't record changes automatically */
    if (self->history_freeze)
	return;

    /* Set a timer to record this change if no others occur
     * immediately afterwards. This consolidates periods of
     * very rapid movement.
     */
    if (self->history_timer) {
	g_source_remove(self->history_timer);
	self->history_timer = 0;
    }
    self->history_timer = g_timeout_add(100, on_record_change, self);
}

static gboolean     on_record_change   (gpointer user_data)
{
    Explorer* self = EXPLORER(user_data);

    explorer_append_history(self, history_node_new(HISTOGRAM_IMAGER(self->map)));
    self->history_timer = 0;
    return FALSE;
}

/* The End */
