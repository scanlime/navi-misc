/*
 * channel-list-window.c - channel list
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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
#include <glib/gi18n.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gconf/gconf-client.h>
#include "channel-list-window.h"
#include "util.h"
#include "gui.h"
#include "../common/xchat.h"
#include "../common/xchatc.h"
#include "../common/server.h"

G_DEFINE_TYPE(ChannelListWindow, channel_list_window, G_TYPE_OBJECT)

static GSList *chanlists = NULL;

static gboolean
channel_list_window_filter (GtkTreeModel *model, GtkTreeIter *iter, ChannelListWindow *window)
{
	char *name, *topic;
	int users;

	gtk_tree_model_get (model, iter, 0, &name, 2, &topic, 4, &users, -1);

	/* filter number of users first, since it's fast */
	if (users < window->minimum) {
		return FALSE;
	}
	if (window->maximum > 0 && users > window->maximum) {
		return FALSE;
	}

	/* text filtering */
	if (window->filter_topic && window->text_filter != NULL && strlen (window->text_filter) != 0) {
		/* We have something to filter */
		if (strcasestr (topic, window->text_filter) == NULL) {
			return FALSE;
		}
	}

	if (window->filter_name && window->text_filter != NULL && strlen (window->text_filter) != 0) {
		if (strcasestr (name, window->text_filter) == NULL) {
			return FALSE;
		}
	}

	return TRUE;
}

static gint
channel_list_window_compare_p (gconstpointer a, gconstpointer b, gpointer data)
{
	ChannelListWindow *as = (ChannelListWindow *) a;

	if (a == NULL) {
		return 1;
	}

	if (as->server == b) {
		return 0;
	} else {
		return 1;
	}
}

static void
channel_list_window_delete (ChannelListWindow *win)
{
	g_object_unref (win);
}

static gboolean
channel_list_window_delete_event (GtkWidget *widget, GdkEvent *event, ChannelListWindow *win)
{
	channel_list_window_delete (win);
	return FALSE;
}

static gboolean
channel_list_window_key_press (GtkWidget *widget, GdkEventKey *event, ChannelListWindow *win)
{
	if (event->keyval == GDK_Escape) {
		g_signal_stop_emission_by_name (widget, "key-press-event");
		channel_list_window_delete (win);
		return TRUE;
	}

	return FALSE;
}

static gboolean
channel_list_window_resensitize (ChannelListWindow *win)
{
	win->refresh_calls++;

	/* If we've been 2 seconds but don't have any results, make the
	 * button sensitive.  This usually happens if the server denies
	 * us a server list due to heavy load */
	if (win->refresh_calls == 1 && win->empty) {
		gtk_widget_set_sensitive (win->refresh_button, TRUE);
		win->refresh_timeout = 0;
		return FALSE;
	}

	/* If we've been 30 seconds, make the button sensitive no matter what
	 * and remove the timeout */
	if (win->refresh_calls == 15) {
		gtk_widget_set_sensitive (win->refresh_button, TRUE);
		win->refresh_timeout = 0;
		return FALSE;
	}
	return TRUE;
}

static void
channel_list_window_refresh (ChannelListWindow *win)
{
	GtkWidget *treeview;
	GtkTreeModel *model, *store, *filter;

	treeview = glade_xml_get_widget (win->xml, "channel list");
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
	filter = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));
	store = gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER (filter));
	gtk_list_store_clear (GTK_LIST_STORE (store));
	// FIXME: use min-users to optimize?
	win->server->p_list_channels (win->server, "", 1);
	win->empty = TRUE;

	gtk_widget_set_sensitive (win->refresh_button, FALSE);
	/* Make the button sensitive after a while.  This lets us avoid putting
	 * multiple entries in the list (if they click it twice), since we
	 * can't discern which results the server is giving us correspond to
	 * which request.  It also reduces the load on the server for
	 * click-happy kids
	 */
	win->refresh_calls = 0;
	win->refresh_timeout = g_timeout_add (2000, (GSourceFunc) channel_list_window_resensitize, (gpointer) win);
}

static gboolean
channel_list_window_resize (GtkWidget *widget, GdkEventConfigure *event, ChannelListWindow *win)
{
	GConfClient *client;

	client = gconf_client_get_default ();
	gconf_client_set_int (client, "/apps/xchat/channel_list/width",  event->width,  NULL);
	gconf_client_set_int (client, "/apps/xchat/channel_list/height", event->height, NULL);
	g_object_unref (client);

	return FALSE;
}

static void
channel_list_window_list_showed (ChannelListWindow *win)
{
	GConfClient *client;
	gint width, height;

	gtk_window_set_resizable (GTK_WINDOW (win->window), TRUE);

	if (win->empty) {
		/* We don't already load the list of channels */
		channel_list_window_refresh (win);

		}
	gtk_widget_show (win->refresh_button);

	/* Set the size of the window */
	client = gconf_client_get_default ();
	width  = gconf_client_get_int (client, "/apps/xchat/channel_list/width",  NULL);
	height = gconf_client_get_int (client, "/apps/xchat/channel_list/height", NULL);
	g_object_unref (client);
	
	if (width == 0 || height == 0) {
		gtk_window_resize (GTK_WINDOW (win->window), 640, 480);
	} else {
		gtk_window_resize (GTK_WINDOW (win->window), width, height);
	}

	g_signal_connect (G_OBJECT (win->window), "configure-event", G_CALLBACK (channel_list_window_resize), win);
}

static void
channel_list_window_list_hidden (ChannelListWindow *win)
{
	gtk_window_set_resizable (GTK_WINDOW (win->window), FALSE);

	gtk_widget_hide (win->refresh_button);

	g_signal_handlers_disconnect_by_func (G_OBJECT (win->window), G_CALLBACK (channel_list_window_resize), win);
}

static void
expander_activated (GtkExpander *expander, GParamSpec *param_spec, ChannelListWindow *win)
{
	if (gtk_expander_get_expanded (expander))
		channel_list_window_list_showed (win);
	else
		channel_list_window_list_hidden (win);
}

static void
on_refresh_button_clicked (GtkWidget *button, ChannelListWindow *win)
{
	channel_list_window_refresh (win);
}

static void
channel_list_window_join (ChannelListWindow *win)
{
	GtkWidget *widget;
	gchar *channel;

	widget = glade_xml_get_widget (win->xml, "channel_entry");
	channel = (gchar *) gtk_entry_get_text (GTK_ENTRY (widget));

	win->server->p_join (win->server, channel, "");
}

static void
join_button_clicked (GtkWidget *button, ChannelListWindow *win)
{
	channel_list_window_join (win);
}

static void
channel_entry_activated (GtkWidget *entry, ChannelListWindow *win)
{
	channel_list_window_join (win);
	channel_list_window_delete (win);
}

static void
channel_list_sensitive_join_button (ChannelListWindow *win, gboolean sensitive)
{
	GtkWidget *button;

	button = glade_xml_get_widget (win->xml, "join button");
	gtk_widget_set_sensitive (button, sensitive);
}

static gboolean
test_channel_name (const char *channel)
{
	gint len;

	if (!channel)
		return FALSE;

	len = strlen (channel);
	if (len > 1 || (len == 1 && channel[0] != '#'))
		return TRUE;
	else
		return FALSE;
}

static void
channel_entry_changed (GtkWidget *entry, ChannelListWindow *win)
{
	const gchar *text;

	text = gtk_entry_get_text (GTK_ENTRY (entry));

	if (!text || strlen (text) == 0) {
		gtk_entry_set_text (GTK_ENTRY (entry), "#");
		gtk_editable_set_position (GTK_EDITABLE (entry), -1);
		channel_list_sensitive_join_button (win, FALSE);
	}
	else
		channel_list_sensitive_join_button (win, test_channel_name (text));
}

static void
channel_list_window_selected (GtkTreeSelection *selection, ChannelListWindow *win)
{
	GtkWidget *entry;
	GtkTreeModel *model;
	GtkTreeIter iter;
	char *channel;

	entry = glade_xml_get_widget (win->xml, "channel_entry");
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 0, &channel, -1);
		g_signal_handlers_block_by_func (G_OBJECT (entry), G_CALLBACK (channel_entry_changed), win);
		gtk_entry_set_text (GTK_ENTRY (entry), channel);
		g_signal_handlers_unblock_by_func (G_OBJECT (entry), G_CALLBACK (channel_entry_changed), win);
		channel_list_sensitive_join_button (win, TRUE);
	}

}

static void
row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, ChannelListWindow *win)
{
	channel_list_window_join (win);
}

static void
minusers_changed (GtkSpinButton *button, ChannelListWindow *win)
{
	win->minimum = gtk_spin_button_get_value_as_int (button);
	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (win->filter));
}

static void
maxusers_changed (GtkSpinButton *button, ChannelListWindow *win)
{
	win->maximum = gtk_spin_button_get_value_as_int (button);
	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (win->filter));
}

static void
filter_changed (GtkEntry *entry, ChannelListWindow *win)
{
	if (win->text_filter != NULL) {
		g_free (win->text_filter);
	}
	win->text_filter = g_strdup (gtk_entry_get_text (entry));
	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (win->filter));
}

static void
apply_to_name_changed (GtkToggleButton *button, ChannelListWindow *win)
{
	win->filter_name = gtk_toggle_button_get_active (button);
	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (win->filter));
}

static void
apply_to_topic_changed (GtkToggleButton *button, ChannelListWindow *win)
{
	win->filter_topic = gtk_toggle_button_get_active (button);
	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (win->filter));
}

static void
close_button_clicked (GtkWidget *button, ChannelListWindow *win)
{
	channel_list_window_delete (win);
}

ChannelListWindow*
channel_list_window_new (session *sess, gboolean show_list)
{
	ChannelListWindow *win;
	GtkWidget *treeview, *widget;
	GtkCellRenderer *channel_r, *users_r, *topic_r;
	GtkTreeViewColumn *channel_c, *users_c, *topic_c;
	GtkTreeSelection *select;
	GSList *tmp;
	gchar *title, *path;

	if (sess == NULL) {
		return NULL;
	}

	if (chanlists == NULL) {
		chanlists = g_slist_alloc ();
	}

	/* check to see if we already have a channel list GUI available */
	tmp = g_slist_find_custom (chanlists, sess->server, (GCompareFunc) channel_list_window_compare_p);
	if (tmp) {
		return tmp->data;
	}

	win = g_object_new (CHANNEL_LIST_WINDOW_TYPE, NULL);

	win->server = sess->server;
	win->xml = NULL;

	win->minimum = 1;
	win->maximum = 0;
	win->text_filter = NULL;
	win->filter_topic = FALSE;
	win->filter_name = TRUE;
	win->empty = TRUE;

	win->refresh_timeout = 0;

	path = locate_data_file ("channel-list.glade");
	if (!path) {
		g_free (win);
		return NULL;
	}

	win->xml = glade_xml_new (path, NULL, NULL);
	g_free (path);
	if (!win->xml) {
		g_free (win);
		return NULL;
	}

	win->window = glade_xml_get_widget (win->xml, "channel_list_window");
	title = g_strdup_printf (_("%s Channel List"), server_get_network (sess->server, FALSE));
	gtk_window_set_title (GTK_WINDOW (win->window), title);
	g_free (title);
	g_signal_connect (G_OBJECT (win->window), "delete-event", G_CALLBACK (channel_list_window_delete_event), win);
	g_signal_connect (G_OBJECT (win->window), "key-press-event", G_CALLBACK (channel_list_window_key_press), win);
	
	widget = glade_xml_get_widget (win->xml, "channel_entry");
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (channel_entry_changed), win);
	g_signal_connect (G_OBJECT (widget), "activate", G_CALLBACK (channel_entry_activated), win);
	gtk_widget_grab_focus (widget);
	/* We simulate a change in the entry to set it at "#" */
	channel_entry_changed (widget, win);

	treeview = glade_xml_get_widget (win->xml, "channel list");
	gtk_tree_view_set_search_column (GTK_TREE_VIEW (treeview), 1);

	win->refresh_button = glade_xml_get_widget (win->xml, "refresh button");
	g_signal_connect (G_OBJECT (win->refresh_button), "clicked", G_CALLBACK (on_refresh_button_clicked), win);
	widget = glade_xml_get_widget (win->xml, "join button");
	gtk_widget_set_sensitive (widget, FALSE);
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (join_button_clicked), win);
	g_signal_connect (G_OBJECT (treeview), "row-activated", G_CALLBACK (row_activated), win);

	/*                                  channel name,  n-users,       topic,         server,         users */
	win->store = gtk_list_store_new (5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_INT);
	win->filter = gtk_tree_model_filter_new (GTK_TREE_MODEL (win->store), NULL);
	gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (win->filter), (GtkTreeModelFilterVisibleFunc) channel_list_window_filter, win, NULL);
	win->sort = GTK_TREE_MODEL_SORT (gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (win->filter)));
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (win->sort));
	gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (treeview), TRUE);

	channel_r = gtk_cell_renderer_text_new ();
	channel_c = gtk_tree_view_column_new_with_attributes (_("Channel Name"), channel_r, "text", 0, NULL);
	gtk_tree_view_column_set_resizable (channel_c, TRUE);
	gtk_tree_view_column_set_sort_column_id (channel_c, 0);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), channel_c);
	users_r = gtk_cell_renderer_text_new ();
	users_c = gtk_tree_view_column_new_with_attributes (_("Users"), users_r, "text", 1, NULL);
	gtk_tree_view_column_set_resizable (users_c, TRUE);
	gtk_tree_view_column_set_sort_column_id (users_c, 4);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), users_c);
	topic_r = gtk_cell_renderer_text_new ();
	topic_c = gtk_tree_view_column_new_with_attributes (_("Topic"), topic_r, "text", 2, NULL);
	gtk_tree_view_column_set_resizable (topic_c, TRUE);
	gtk_tree_view_column_set_sort_column_id (topic_c, 2);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), topic_c);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (channel_list_window_selected), win);

	widget = glade_xml_get_widget (win->xml, "minimum users");
	g_signal_connect (G_OBJECT (widget), "value-changed", G_CALLBACK (minusers_changed), win);
	widget = glade_xml_get_widget (win->xml, "maximum users");
	g_signal_connect (G_OBJECT (widget), "value-changed", G_CALLBACK (maxusers_changed), win);
	widget = glade_xml_get_widget (win->xml, "text filter");
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (filter_changed), win);
	widget = glade_xml_get_widget (win->xml, "apply to topic");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), win->filter_topic);
	g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (apply_to_topic_changed), win);
	widget = glade_xml_get_widget (win->xml, "apply to name");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), win->filter_name);
	g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (apply_to_name_changed), win);
	widget = glade_xml_get_widget (win->xml, "close button");
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (close_button_clicked), win);
	widget = glade_xml_get_widget (win->xml, "expander");
	gtk_expander_set_expanded (GTK_EXPANDER (widget), show_list);
	g_signal_connect (G_OBJECT (widget), "notify::expanded", G_CALLBACK (expander_activated), win);

	gtk_widget_show_all (win->window);

	if (show_list)
		channel_list_window_list_showed (win);
	else
		channel_list_window_list_hidden (win);

	chanlists = g_slist_append (chanlists, win);
	return win;
}

static void
channel_list_window_finalize (GObject *object)
{
	ChannelListWindow *win = CHANNEL_LIST_WINDOW (object);

	if (win->refresh_timeout) {
		g_source_remove (win->refresh_timeout);
	}
	chanlists = g_slist_remove (chanlists, (gpointer) win);
	gtk_widget_hide_all (win->window);

	g_object_unref (win->xml);
	G_OBJECT_CLASS (channel_list_window_parent_class)->finalize (object);
}

static void
channel_list_window_class_init (ChannelListWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = channel_list_window_finalize;
}

static void
channel_list_window_init (ChannelListWindow *win)
{
}

gboolean
channel_list_exists (server *serv)
{
	return (g_slist_find_custom (chanlists, serv, (GCompareFunc) channel_list_window_compare_p) != NULL);
}

void
create_channel_list_window (session *sess, gboolean show_list)
{
	channel_list_window_new (sess, show_list);
}

void
channel_list_append (server *serv, char *channel, char *users, char *topic)
{
	GtkWidget *treeview;
	GtkListStore *store;
	GtkTreeModelSort *sort;
	GtkTreeModelFilter *filter;
	GtkTreeIter iter;
	GSList *element;
	ChannelListWindow *win;
	int nusers;

	element = g_slist_find_custom (chanlists, serv, (GCompareFunc) channel_list_window_compare_p);
	if (element == NULL) {
		return;
	}

	win = element->data;
	treeview = glade_xml_get_widget (win->xml, "channel list");
	sort = GTK_TREE_MODEL_SORT (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
	filter = GTK_TREE_MODEL_FILTER (gtk_tree_model_sort_get_model (sort));
	store = GTK_LIST_STORE (gtk_tree_model_filter_get_model (filter));

	win->empty = FALSE;

	nusers = g_strtod (users, NULL);

	gtk_list_store_append (store, &iter);
	gtk_list_store_set (store, &iter, 0, channel, 1, users, 2, topic, 3, serv, 4, nusers, -1);
}
