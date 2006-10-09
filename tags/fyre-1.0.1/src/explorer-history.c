/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * explorer-history.c - Implements a simple non-persistent history
 *                      tracker. History items can be recorded immediately
 *                      or on a timer. Each history item stores a timestamp,
 *                      parameters, and a small thumbnail. They are made
 *                      available to the user through a web-browser-like "Go" menu.
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

#include <config.h>
#include "explorer.h"
#include "histogram-imager.h"

/* These are stored in the history_queue. The parameter string and
 * thumbnail are both owned by the history node.
 */
typedef struct {
    GTimeVal   timestamp;   /* The time at which this node was created */
    GdkPixbuf* thumbnail;   /* Scaled-down version of the current histogram */
    gchar*     params;      /* Serialized image parameters */
    Explorer*  explorer;    /* For convenience in callbacks */
} HistoryNode;

struct delete_after_data {
    GtkWidget *menu;
    GtkWidget *separator;
    gboolean triggered;
};

static void         delete_after       (GtkWidget *item, gpointer user_data);

static HistoryNode* history_node_new   (HistogramImager* map);
static void         history_node_apply (HistoryNode* self, HistogramImager* map);
static void         history_node_free  (HistoryNode* self);

static void         on_go_back         (GtkWidget *widget, Explorer *self);
static void         on_go_forward      (GtkWidget *widget, Explorer *self);
static void         on_go_menu_show    (GtkWidget *menu, Explorer *self);
static void         on_go_menu_hide    (GtkWidget *menu, Explorer *self);
static void         on_go_menu_item    (GtkWidget *menu, gpointer user_data);
static void         on_change_notify   (ParameterHolder *holder, GParamSpec* spec, Explorer *self);
static gboolean     on_record_change   (gpointer user_data);

static void         explorer_append_history (Explorer* self, HistoryNode *node);
static void         explorer_prune_history  (Explorer* self, int max_nodes);
static void         explorer_update_history_sensitivity (Explorer* self);

static gdouble      timeval_subtract          (GTimeVal *a, GTimeVal *b);
static gchar*       explorer_strdup_time      (GTimeVal *tv);
static void         explorer_add_go_item      (Explorer *self, GList *node_link);
static void         explorer_cleanup_go_items (Explorer *self);


/************************************************************************************/
/***************************************************************** Public Methods ***/
/************************************************************************************/

void explorer_init_history(Explorer *self)
{
    GtkWidget *menu = glade_xml_get_widget(self->xml, "go_menu_menu");

    self->history_queue = g_queue_new();

    /* Connect signal handlers
     */
    glade_xml_signal_connect_data(self->xml, "on_go_back",     G_CALLBACK(on_go_back),        self);
    glade_xml_signal_connect_data(self->xml, "on_go_forward",  G_CALLBACK(on_go_forward),     self);
    g_signal_connect             (self->map, "notify",         G_CALLBACK(on_change_notify),  self);
    g_signal_connect             (menu,      "show",           G_CALLBACK(on_go_menu_show),   self);
    g_signal_connect             (menu,      "hide",           G_CALLBACK(on_go_menu_hide),   self);
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


/************************************************************************************/
/****************************************************************** History Nodes ***/
/************************************************************************************/

static HistoryNode* history_node_new   (HistogramImager* map)
{
    HistoryNode* self = g_new0(HistoryNode, 1);
    gint width, height;

    g_get_current_time(&self->timestamp);
    self->params = parameter_holder_save_string(PARAMETER_HOLDER(map));

    /* Use the normal icon size plus a little extra */
    gtk_icon_size_lookup(GTK_ICON_SIZE_MENU, &width, &height);
    width *= 1.75;
    height *= 1.75;
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
    node->explorer = self;
    g_queue_push_tail(self->history_queue, node);

    /* Seems like a reasonable length... */
    explorer_prune_history(self, 128);

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
/******************************************************************* Time Utilities */
/************************************************************************************/

static gdouble      timeval_subtract(GTimeVal *a, GTimeVal *b)
{
    return (a->tv_sec - b->tv_sec) +
	(a->tv_usec - b->tv_usec) / 1000000.0;
}

static gchar*       explorer_strdup_time      (GTimeVal *tv)
{
    double units;
    GTimeVal now;
    g_get_current_time(&now);
    units = timeval_subtract(&now, tv);

    if (units < 120.0)
	return g_strdup_printf("%.01f seconds ago", units);
    units /= 60.0;

    if (units < 120.0)
	return g_strdup_printf("%.01f minutes ago", units);
    units /= 60.0;

    if (units < 120.0)
	return g_strdup_printf("%.02f hours ago", units);
    units /= 24.0;

    return g_strdup_printf("%.02f days ago", units);
}


/************************************************************************************/
/***************************************************************** Menu Maintenance */
/************************************************************************************/

static void         explorer_add_go_item      (Explorer *self, GList *node_link)
{
    GtkWidget *menu = glade_xml_get_widget(self->xml, "go_menu_menu");
    HistoryNode *node = node_link->data;
    GtkWidget *item, *image;
    gchar* label;

    /* Add the timestamp */
    label = explorer_strdup_time(&node->timestamp);
    item = gtk_image_menu_item_new_with_label(label);
    g_free(label);

    /* Add the thumbnail */
    image = gtk_image_new_from_pixbuf(node->thumbnail);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);

    /* Set up a callback to activate this node.
     * Note that its data is our node_link, not the Explorer.
     */
    g_signal_connect(item, "activate", G_CALLBACK(on_go_menu_item), node_link);

    /* Add it to the menu */
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_widget_show(item);
}

static void         delete_after              (GtkWidget *item, gpointer user_data)
{
    struct delete_after_data *data = (struct delete_after_data*) user_data;

    if (item == data->separator)
	data->triggered = TRUE;
    else if (data->triggered)
	gtk_container_remove(GTK_CONTAINER(data->menu), item);
}

static void         explorer_cleanup_go_items (Explorer *self)
{
    /* Remove all items in the 'go' menu after our marked separator
     */
    GtkWidget *menu = glade_xml_get_widget(self->xml, "go_menu_menu");
    struct delete_after_data data;

    data.menu = menu;
    data.separator = glade_xml_get_widget(self->xml, "history_separator");
    data.triggered = FALSE;
    gtk_container_foreach(GTK_CONTAINER(menu), delete_after, &data);
}


/************************************************************************************/
/******************************************************************** GUI Callbacks */
/************************************************************************************/

static void         on_go_back        (GtkWidget *widget, Explorer *self)
{
    self->history_current_link = self->history_current_link->prev;
    explorer_update_history_sensitivity(self);

    self->history_freeze = TRUE;
    history_node_apply(self->history_current_link->data, HISTOGRAM_IMAGER(self->map));
    self->history_freeze = FALSE;
}

static void         on_go_forward     (GtkWidget *widget, Explorer *self)
{
    self->history_current_link = self->history_current_link->next;
    explorer_update_history_sensitivity(self);

    self->history_freeze = TRUE;
    history_node_apply(self->history_current_link->data, HISTOGRAM_IMAGER(self->map));
    self->history_freeze = FALSE;
}

static void         on_go_menu_show    (GtkWidget *menu, Explorer *self)
{
    const int max_linear_items = 4;
    const int max_scaled_items = 10;
    int i, num_scaled_items;
    GList *current;
    gdouble t_total, t;
    GTimeVal *scaled_reference;
    HistoryNode* node;
    explorer_cleanup_go_items(self);

    /* If our queue is empty, add a new record immediately. This
     * will prevent us from getting an empty 'go' menu right after
     * startup- instead it will have a thumbnail of our defaults
     * or whatever image the user happened to load. This can't
     * go in our initialization above since that happens before
     * we've actually rendered anything. Putting it here is more
     * reliable than using a timer.
     */
    if (self->history_queue->length < 1)
	explorer_append_history(self, history_node_new(HISTOGRAM_IMAGER(self->map)));

    /* The first few items are straight from the most recent list */
    current = g_queue_peek_tail_link(self->history_queue);
    for (i=0; i<max_linear_items; i++) {
	if (!current)
	    return;
	explorer_add_go_item(self, current);
	current = current->prev;
    }

    /* The rest of the list is spread evenly over time, from the end
     * of the above section back to the oldest history we have.
     */
    if (!current)
	return;
    node = current->data;
    scaled_reference = &node->timestamp;
    t_total = timeval_subtract(scaled_reference,
			       &((HistoryNode*)g_queue_peek_head(self->history_queue))->timestamp);
    num_scaled_items = MIN(self->history_queue->length - max_linear_items,
			   max_scaled_items);
    if (num_scaled_items <= 0)
	return;

    for (i=0; i<num_scaled_items; i++) {

	/* For each item on the scaled_items list, find the
	 * node at the proper position in time.
	 */
	while (1) {
	    if (!current)
		return;

	    node = current->data;
	    t  = timeval_subtract(scaled_reference, &node->timestamp);

	    if (t > (i*t_total/num_scaled_items))
		break;
	    current = current->prev;
	}

	explorer_add_go_item(self, current);
    }
}

static void         on_go_menu_hide    (GtkWidget *menu, Explorer *self)
{
    explorer_cleanup_go_items(self);
}

static void         on_go_menu_item    (GtkWidget *menu, gpointer user_data)
{
    GList *link = user_data;
    HistoryNode *node = link->data;
    Explorer *self = node->explorer;

    self->history_current_link = link;
    explorer_update_history_sensitivity(self);

    self->history_freeze = TRUE;
    history_node_apply(self->history_current_link->data, HISTOGRAM_IMAGER(self->map));
    self->history_freeze = FALSE;
}

static void         on_change_notify   (ParameterHolder *holder, GParamSpec* spec, Explorer *self)
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
    self->history_timer = g_timeout_add(150, on_record_change, self);
}

static gboolean     on_record_change   (gpointer user_data)
{
    Explorer* self = EXPLORER(user_data);

    explorer_append_history(self, history_node_new(HISTOGRAM_IMAGER(self->map)));
    self->history_timer = 0;
    return FALSE;
}

/* The End */
