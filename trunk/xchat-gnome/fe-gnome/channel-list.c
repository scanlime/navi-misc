/*
 * channel-list.c - channel list
 *
 * Copyright (C) 2004-2005 xchat-gnome team
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

#define _GNU_SOURCE
#include <string.h>
#include <gnome.h>
#include "channel-list.h"
#include "../common/xchat.h"
#include "../common/xchatc.h"
#include "../common/server.h"

static GSList *chanlists = NULL;

static gboolean
chanlist_filter (GtkTreeModel *model, GtkTreeIter *iter, channel_list_window *window)
{
	char *name, *topic, *found_topic = NULL, *found_name = NULL;
	int users;

	/* FIXME */
	return TRUE;

	gtk_tree_model_get (model, iter, 0, &name, 2, &topic, 4, &users, -1);

	if (window->filter_name && name == NULL)
		return TRUE;
	if (window->filter_topic && topic == NULL)
		return TRUE;

	/* filter number of users first, since it's fast */
	if (users < window->minimum)
		return FALSE;
	if (window->maximum > 0 && users > window->maximum)
		return FALSE;

	if (!(window->filter_topic || window->filter_name))
		return FALSE;

	if ((window->text_filter == NULL) || (strlen (window->text_filter) == 0))
		return FALSE;

	if (window->filter_topic)
		found_topic = strcasestr (topic, window->text_filter);

	if (window->filter_name)
		found_name = strcasestr (name, window->text_filter);

	return (found_name || found_topic);
}

static gint
chanlist_compare_p (gconstpointer a, gconstpointer b, gpointer data)
{
	channel_list_window *as = (channel_list_window *) a;

	if (a == NULL)
		return 1;

	if (as->server == b)
		return 0;
	else
		return 1;
}

static gboolean
chanlist_delete (GtkWidget *widget, GdkEvent *event, channel_list_window *win)
{
	GtkWidget *window;

	if (win->refresh_timeout)
		g_source_remove (win->refresh_timeout);

	g_slist_remove (chanlists, (gpointer) win);

	window = glade_xml_get_widget (win->xml, "window 1");
	gtk_widget_hide_all (window);
	g_object_unref (win->xml);
	g_free (win);
	return FALSE;
}

static gboolean
resensitize (channel_list_window *win)
{
	win->refresh_calls++;

	/* If we've been 2 seconds but don't have any results, make the
	 * button sensitive.  This usually happens if the server denies
	 * us a server list due to heavy load */
	if (win->refresh_calls == 1 && win->empty) {
		GtkWidget *button = glade_xml_get_widget (win->xml, "refresh button");
		gtk_widget_set_sensitive (button, TRUE);
		win->refresh_timeout = 0;
		return FALSE;
	}

	/* If we've been 30 seconds, make the button sensitive no matter what
	 * and remove the timeout */
	if (win->refresh_calls == 15) {
		GtkWidget *button = glade_xml_get_widget (win->xml, "refresh button");
		gtk_widget_set_sensitive (button, TRUE);
		win->refresh_timeout = 0;
		return FALSE;
	}
	return TRUE;
}

static void
chanlist_refresh (GtkWidget *button, channel_list_window *win)
{
	GtkWidget *treeview;
	GtkTreeModel *model, *store, *filter;

	treeview = glade_xml_get_widget (win->xml, "channel list");
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
	filter = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));
	store = gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER (filter));
	gtk_list_store_clear (GTK_LIST_STORE (store));
	win->server->p_list_channels (win->server, "");
	win->empty = TRUE;

	gtk_widget_set_sensitive (button, FALSE);
	/* Make the button sensitive after a while.  This lets us avoid putting
	 * multiple entries in the list (if they click it twice), since we
	 * can't discern which results the server is giving us correspond to
	 * which request.  It also reduces the load on the server for
	 * click-happy kids
	 */
	win->refresh_calls = 0;
	win->refresh_timeout = g_timeout_add (2000, (GSourceFunc) resensitize, (gpointer) win);
}

static void
chanlist_save (GtkWidget *button, channel_list_window *win)
{
}

static void
chanlist_join (GtkWidget *button, channel_list_window *win)
{
	GtkWidget *treeview;
	GtkTreeModel *model;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	char *channel;
	server *serv;

	treeview = glade_xml_get_widget (win->xml, "channel list");
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	if (gtk_tree_selection_get_selected (select, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, 0, &channel, 3, &serv, -1);
		serv->p_join (serv, channel, "");
	}
}

static void
chanlist_selected (GtkTreeSelection *selection, channel_list_window *win)
{
	GtkWidget *button;
	button = glade_xml_get_widget (win->xml, "join button");

	gtk_widget_set_sensitive (button, gtk_tree_selection_get_selected (selection, NULL, NULL));
}

static gboolean
chanlist_resize (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	gnome_config_set_int ("/xchat-gnome/channel_list/width", event->width);
	gnome_config_set_int ("/xchat-gnome/channel_list/height", event->height);
	gnome_config_sync ();
	return FALSE;
}

static void
minusers_changed (GtkSpinButton *button, channel_list_window *win)
{
	win->minimum = gtk_spin_button_get_value_as_int (button);
	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (win->filter));
}

static void
maxusers_changed (GtkSpinButton *button, channel_list_window *win)
{
	win->maximum = gtk_spin_button_get_value_as_int (button);
	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (win->filter));
}

static void
filter_changed (GtkEntry *entry, channel_list_window *win)
{
	if (win->text_filter != NULL)
		g_free (win->text_filter);
	win->text_filter = g_strdup (gtk_entry_get_text (entry));
	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (win->filter));
}

static void
apply_to_name_changed (GtkToggleButton *button, channel_list_window *win)
{
	win->filter_name = gtk_toggle_button_get_active (button);
	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (win->filter));
}

static void
apply_to_topic_changed (GtkToggleButton *button, channel_list_window *win)
{
	win->filter_topic = gtk_toggle_button_get_active (button);
	gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER (win->filter));
}

void
create_channel_list (session *sess)
{
	channel_list_window *win;
	GtkWidget *treeview, *widget;
	GtkCellRenderer *channel_r, *users_r, *topic_r;
	GtkTreeViewColumn *channel_c, *users_c, *topic_c;
	GtkSizeGroup *group;
	GtkTreeSelection *select;
	int width, height;
	gchar *title;

	if (sess == NULL)
		return;

	if (chanlists == NULL)
		chanlists = g_slist_alloc ();

	/* check to see if we already have a channel list GUI available */
	if (g_slist_find_custom (chanlists, sess->server, (GCompareFunc) chanlist_compare_p) != NULL)
		return;

	win = g_malloc (sizeof (channel_list_window));

	win->server = sess->server;
	win->xml = NULL;

	win->minimum = 1;
	win->maximum = 0;
	win->text_filter = NULL;
	win->filter_topic = FALSE;
	win->filter_name = TRUE;

	win->refresh_timeout = 0;

	if (g_file_test ("channel-list.glade", G_FILE_TEST_EXISTS))
		win->xml = glade_xml_new ("channel-list.glade", NULL, NULL);
	if (!win->xml)
		win->xml = glade_xml_new (XCHATSHAREDIR"/channel-list.glade", NULL, NULL);
	if (!win->xml) {
		free (win);
		return;
	}

	widget = glade_xml_get_widget (win->xml, "window 1");
	title = g_strdup_printf ("%s Channel List", server_get_network (sess->server, FALSE));
	gtk_window_set_title (GTK_WINDOW (widget), title);
	g_free (title);
	g_signal_connect (G_OBJECT (widget), "delete-event", G_CALLBACK (chanlist_delete), win);

	treeview = glade_xml_get_widget (win->xml, "channel list");
	gtk_tree_view_set_search_column (GTK_TREE_VIEW (treeview), 1);

	widget = glade_xml_get_widget (win->xml, "refresh button");
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (chanlist_refresh), win);
	widget = glade_xml_get_widget (win->xml, "save button");
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (chanlist_save), win);
	widget = glade_xml_get_widget (win->xml, "join button");
	gtk_widget_set_sensitive (widget, FALSE);
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (chanlist_join), win);

	/*                                  channel name,  n-users,       topic,         server,         users */
	win->store = gtk_list_store_new (5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_INT);
	win->filter = gtk_tree_model_filter_new (GTK_TREE_MODEL (win->store), NULL);
	/* gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (win->filter), (GtkTreeModelFilterVisibleFunc) chanlist_filter, win, NULL); */
	win->sort = GTK_TREE_MODEL_SORT (gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (win->filter)));
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (win->sort));
	gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (treeview), TRUE);

	channel_r = gtk_cell_renderer_text_new ();
	channel_c = gtk_tree_view_column_new_with_attributes ("Channel Name", channel_r, "text", 0, NULL);
	gtk_tree_view_column_set_resizable (channel_c, TRUE);
	gtk_tree_view_column_set_sort_column_id (channel_c, 0);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), channel_c);
	users_r = gtk_cell_renderer_text_new ();
	users_c = gtk_tree_view_column_new_with_attributes ("Users", users_r, "text", 1, NULL);
	gtk_tree_view_column_set_resizable (users_c, TRUE);
	gtk_tree_view_column_set_sort_column_id (users_c, 4);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), users_c);
	topic_r = gtk_cell_renderer_text_new ();
	topic_c = gtk_tree_view_column_new_with_attributes ("Topic", topic_r, "text", 2, NULL);
	gtk_tree_view_column_set_resizable (topic_c, TRUE);
	gtk_tree_view_column_set_sort_column_id (topic_c, 2);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), topic_c);

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget (win->xml, "minimum users");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (win->xml, "maximum users");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (win->xml, "text filter");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (win->xml, "toggles hbox");
	gtk_size_group_add_widget (group, widget);
	g_object_unref (group);

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget (win->xml, "minusers label");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (win->xml, "maxusers label");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (win->xml, "filter label");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (win->xml, "applyto label");
	gtk_size_group_add_widget (group, widget);
	g_object_unref (group);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (chanlist_selected), win);

	widget = glade_xml_get_widget (win->xml, "minimum users");
	g_signal_connect (G_OBJECT (widget), "value-changed", G_CALLBACK (minusers_changed), win);
	widget = glade_xml_get_widget (win->xml, "maximum users");
	g_signal_connect (G_OBJECT (widget), "value-changed", G_CALLBACK (maxusers_changed), win);
	widget = glade_xml_get_widget (win->xml, "text filter");
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (filter_changed), win);
	widget = glade_xml_get_widget (win->xml, "apply to topic");
	g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (apply_to_topic_changed), win);
	widget = glade_xml_get_widget (win->xml, "apply to name");
	g_signal_connect (G_OBJECT (widget), "toggled", G_CALLBACK (apply_to_name_changed), win);

	widget = glade_xml_get_widget (win->xml, "window 1");
	width = gnome_config_get_int_with_default ("/xchat-gnome/channel_list/width", 0);
	height = gnome_config_get_int_with_default ("/xchat-gnome/channel_list/height", 0);
	if (width == 0 || height == 0)
		gtk_window_set_default_size (GTK_WINDOW (widget), 640, 480);
	else
		gtk_window_set_default_size (GTK_WINDOW (widget), width, height);
	g_signal_connect (G_OBJECT (widget), "configure-event", G_CALLBACK (chanlist_resize), NULL);
	gtk_widget_show_all (widget);

	g_slist_append (chanlists, win);
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
	channel_list_window *win;
	int nusers;

	element = g_slist_find_custom (chanlists, serv, (GCompareFunc) chanlist_compare_p);
	if (element == NULL)
		return;

	win = element->data;
	treeview = glade_xml_get_widget (win->xml, "channel list");
	sort = GTK_TREE_MODEL_SORT (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
	filter = GTK_TREE_MODEL_FILTER (gtk_tree_model_sort_get_model (sort));
	store = GTK_LIST_STORE (gtk_tree_model_filter_get_model (filter));

	win->empty = FALSE;

	nusers = atoi (users);

	gtk_list_store_append (store, &iter);
	gtk_list_store_set (store, &iter, 0, channel, 1, users, 2, topic, 3, serv, 4, nusers, -1);
}

void
repopulate_channel_list (channel_list_window *win)
{
}

gboolean
channel_list_exists (server *serv)
{
	return (g_slist_find_custom (chanlists, serv, (GCompareFunc) chanlist_compare_p) != NULL);
}
