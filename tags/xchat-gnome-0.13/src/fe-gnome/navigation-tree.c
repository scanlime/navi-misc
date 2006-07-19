/*
 * navigation-tree.c - functions to create and maintain the navigation tree
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
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include "navigation-tree.h"
#include "userlist-gui.h"
#include "userlist.h"
#include "pixmaps.h"
#include "palette.h"
#include "channel-list.h"
#include "main-window.h"

#include "conversation-panel.h"
#include "find-bar.h"
#include "status-bar.h"
#include "text-entry.h"
#include "topic-label.h"

#include "util.h"
#include "../common/xchat.h"
#include "../common/fe.h"
#include "../common/servlist.h"
#include "../common/plugin.h"

/***** NavTree *****/
static void      navigation_tree_init (NavTree *navtree);
static void      navigation_tree_class_init (NavTreeClass *klass);
static void      navigation_tree_dispose (GObject *object);
static void      navigation_tree_finalize (GObject *object);
static void      navigation_tree_update_refs (NavTree *navtree);
/* Context menus. */
static GtkWidget *navigation_context (GtkWidget *treeview, session *selected);
static GtkWidget *server_context (GtkWidget *treeview, session *selected);
static GtkWidget *channel_context (GtkWidget *treeview, session *selected);
static GtkWidget *dialog_context (GtkWidget *treeview, session *selected);
static gboolean  channel_is_autojoin (ircnet *network, gchar *chan);
static void      show_context_menu (GtkWidget *treeview, GdkEventButton *event);
/* Callbacks. */
static gboolean  click (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static gboolean  declick (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static gboolean  popup_menu (GtkWidget *treeview, gpointer user_data);
static void      navigation_selection_changed (GtkTreeSelection *treeselection, gpointer user_data);
static void      row_expanded (GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data);
static void      row_collapsed (GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data);
static void      on_server_information (GtkAction *action, gpointer data);
static void      on_server_reconnect (GtkAction *action, gpointer data);
static void      on_server_disconnect (GtkAction *action, gpointer data);
static void      on_server_channel_list (GtkAction *action, gpointer data);
static void      on_save (GtkAction *action, gpointer data);
static void      on_close (GtkAction *action, gpointer data);
static void      on_find (GtkAction *action, gpointer data);
static void      on_channel_leave (GtkAction *action, gpointer data);
static void      on_channel_bans (GtkAction *action, gpointer data);
static void      on_channel_join (GtkAction *action, gpointer data);
static void      on_server_autoconnect (GtkAction *action, gpointer data);
static void      on_channel_autojoin (GtkAction *action, gpointer data);

static GtkActionEntry action_entries[] = {
	/* Server context menu */
	{"ServerInformation", GTK_STOCK_DIALOG_INFO,    N_("_Information"),     "", NULL, G_CALLBACK (on_server_information)},
	{"ServerReconnect",   GTK_STOCK_REFRESH,        N_("_Reconnect"),       "", NULL, G_CALLBACK (on_server_reconnect)},
	{"ServerDisconnect",  GTK_STOCK_STOP,           N_("_Disconnect"),      "", NULL, G_CALLBACK (on_server_disconnect)},
	{"ServerClose",       GTK_STOCK_CLOSE,          N_("_Close"),           "", NULL, G_CALLBACK (on_close)},
	{"ServerChannels",    GTK_STOCK_INDEX,          N_("_Channels..."),     "", NULL, G_CALLBACK (on_server_channel_list)},

	/* Channel context menu */
	{"ChannelSave",       GTK_STOCK_SAVE,           N_("_Save Transcript"), "", NULL, G_CALLBACK (on_save)},
	{"ChannelLeave",      GTK_STOCK_QUIT,           N_("_Leave"),           "", NULL, G_CALLBACK (on_channel_leave)},
	{"ChannelClose",      GTK_STOCK_CLOSE,          N_("_Close"),           "", NULL, G_CALLBACK (on_close)},
	{"ChannelJoin",       GTK_STOCK_JUMP_TO,        N_("_Join"),            "", NULL, G_CALLBACK (on_channel_join)},
	{"ChannelFind",       GTK_STOCK_FIND,           N_("_Find..."),         "", NULL, G_CALLBACK (on_find)},
	{"ChannelBans",       GTK_STOCK_DIALOG_WARNING, N_("_Bans..."),         "", NULL, G_CALLBACK (on_channel_bans)},

	/* Dialog context menu */
	{"DialogSave",        GTK_STOCK_SAVE,           N_("_Save Transcript"), "", NULL, G_CALLBACK (on_save)},
	{"DialogClose",       GTK_STOCK_CLOSE,          N_("_Close"),           "", NULL, G_CALLBACK (on_close)},
	{"DialogFind",        GTK_STOCK_FIND,           N_("_Find..."),         "", NULL, G_CALLBACK (on_find)},
};

static GtkToggleActionEntry toggle_action_entries[] = {
	/* Server context menu */
	{"ServerAutoConnect", NULL, N_("_Auto-connect on startup"), "", NULL, G_CALLBACK (on_server_autoconnect), FALSE},

	/* Channel context menu */
	{"ChannelAutoJoin",   NULL, N_("_Auto-join on connect"),    "", NULL, G_CALLBACK (on_channel_autojoin), FALSE},
};

static GtkActionGroup *action_group;

GType
navigation_tree_get_type (void)
{
	static GType navigation_tree_type = 0;

	/* If we haven't registered the type yet. */
	if (!navigation_tree_type) {
		static const GTypeInfo navigation_tree_info = {
			sizeof (NavTreeClass),
			NULL,		/* base init. */
			NULL,		/* base finalize. */
			(GClassInitFunc) navigation_tree_class_init,
			NULL,		/* class finalize. */
			NULL,		/* class data. */
			sizeof (NavTree),
			0,		/* n_preallocs. */
			(GInstanceInitFunc) navigation_tree_init,
		};

		/* Register the type. */
		navigation_tree_type = g_type_register_static (GTK_TYPE_TREE_VIEW, "NavTree", &navigation_tree_info, 0);
	}

	return navigation_tree_type;
}

static void
navigation_tree_init (NavTree *navtree)
{
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;

	g_object_set ((gpointer) navtree, "headers-visible", FALSE, NULL);
	navtree->current_rowref = NULL;
	navtree->model = NULL;
	navtree->selection_changed_id = 0;

	action_group = gtk_action_group_new ("NavigationContext");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (action_group, action_entries, G_N_ELEMENTS (action_entries), NULL);
	gtk_action_group_add_toggle_actions (action_group, toggle_action_entries, G_N_ELEMENTS (toggle_action_entries), NULL);
	gtk_ui_manager_insert_action_group (gui.manager, action_group, 0);
	g_object_unref (action_group);

	/* This sets up all our columns. */
	column = gtk_tree_view_column_new ();
	icon_renderer = gtk_cell_renderer_pixbuf_new ();
	text_renderer = gtk_cell_renderer_text_new ();
	/* Icon columns. */
	gtk_tree_view_column_pack_start (column, icon_renderer, FALSE);
	gtk_tree_view_column_set_attributes (column, icon_renderer, "pixbuf", 0, NULL);
	/* text columns. */
	gtk_tree_view_column_pack_start (column, text_renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, text_renderer, "text", 1, "foreground-gdk", 4, NULL);
	/* Add the column to the TreeView. */
	gtk_tree_view_append_column (GTK_TREE_VIEW (navtree), column);

	/* Set our selection mode. */
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	/* Connect the callbacks. */
	navtree->selection_changed_id = g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (navigation_selection_changed), NULL);
	g_signal_connect (G_OBJECT (navtree), "row-expanded", G_CALLBACK (row_expanded), NULL);
	g_signal_connect (G_OBJECT (navtree), "row-collapsed", G_CALLBACK (row_collapsed), NULL);
	g_signal_connect (G_OBJECT (navtree), "button_press_event", G_CALLBACK (click), NULL);
	g_signal_connect (G_OBJECT (navtree), "button_release_event", G_CALLBACK (declick), NULL);
	g_signal_connect (G_OBJECT (navtree), "popup_menu", G_CALLBACK (popup_menu), NULL);

	/* Initialize our row references. */
	navtree->last_server = NULL;
	navtree->last_channel = NULL;
}

static void
navigation_tree_class_init (NavTreeClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;

	object_class->dispose = navigation_tree_dispose;
	object_class->finalize = navigation_tree_finalize;
}

static void
navigation_tree_dispose (GObject *object)
{
	NavTree *navtree = (NavTree *) object;
	if (navtree->model) {
		g_object_unref ((gpointer) navtree->model);
		navtree->model = NULL;
	}
}

static void
navigation_tree_finalize (GObject *object)
{
	NavTree *navtree = (NavTree *) object;
	gtk_tree_row_reference_free (navtree->current_rowref);
	navtree->current_rowref = NULL;
}

/* New NavTree. */
NavTree *
navigation_tree_new (NavModel *model)
{
	NavTree *new_tree;

	/* Create the new NavTree. */
	new_tree = NAVTREE (g_object_new (navigation_tree_get_type (), NULL));

	/* Assign a NavModel to the NavTree. */
	new_tree->model = model;
	gtk_tree_view_set_model (GTK_TREE_VIEW (new_tree), new_tree->model->sorted);

	return new_tree;
}

/* Add/remove server/channel functions. */
void
navigation_tree_create_new_network_entry (NavTree *navtree, struct session *sess)
{
	GtkWidget *menuitem, *button;

	navigation_model_add_new_network (navtree->model, sess);

	/* Select the new network. */
	navigation_tree_select_session (navtree, sess);

	/* We rely on the above select_session call triggering the selection 'changed' event to do book-keeping */

	/* Our row references to the last channel and server should also be
	 * updated.
	 */
	navigation_tree_update_refs (navtree);

	/* Make the close button insensitive. */
	button = glade_xml_get_widget (gui.xml, "close discussion");
	gtk_widget_set_sensitive (button, FALSE);

	/* Make the topic button insensitive. */
	menuitem = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/DiscussionMenu/DiscussionChangeTopicItem");
	if (menuitem == NULL)
		g_warning ("can't disable topic change menu item");
	gtk_widget_set_sensitive (menuitem, FALSE);
}

void
navigation_tree_create_new_channel_entry (NavTree *navtree, struct session *sess, gboolean focus)
{
	GtkWidget *menuitem, *button;
	ircnet *net;

	navigation_model_add_new_channel (navtree->model, sess);

	if (focus) {
		navigation_tree_select_session (navtree, sess);

		/* We rely on the above select_session call triggering the selection 'changed' event to do book-keeping */

		/* Update the row refs here. */
		navigation_tree_update_refs (navtree);

		/* Set the userlist model */
		gtk_tree_view_set_model (GTK_TREE_VIEW (gui.userlist),
		                         GTK_TREE_MODEL (userlist_get_store (u, sess)));

		topic_label_set_current (TOPIC_LABEL (gui.topic_label), sess);
		net = sess->server->network;
		if (net == NULL)
			rename_main_window (NULL, sess->channel);
		else
			rename_main_window (net->name, sess->channel);

		button = glade_xml_get_widget (gui.xml, "close discussion");
		if (sess->type == SESS_CHANNEL)
			gtk_widget_set_sensitive (button, TRUE);
		menuitem = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/DiscussionMenu/DiscussionChangeTopicItem");
		if (menuitem == NULL)
			g_warning ("can't access topic change menu item");
		else
			gtk_widget_set_sensitive (menuitem, sess->type == SESS_CHANNEL);

		/* Set our nick. */
		set_nickname (sess->server, NULL);
	}
}

void
navigation_tree_remove_channel (NavTree *navtree, struct session *sess)
{
	GtkTreePath *path = gtk_tree_row_reference_get_path (navtree->current_rowref);
	GtkTreeSelection *select = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));
	GtkTreeIter iter;
	GtkTreeModel *sorted;

	/* Try to move back a channel, otherwise move up to the server. */
	if (!gtk_tree_path_prev (path)) {
		sorted = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
		gtk_tree_model_get_iter (sorted, &iter, path);
		if (gtk_tree_model_iter_next (sorted, &iter)) {
			gtk_tree_path_free (path);
			path = gtk_tree_model_get_path (sorted, &iter);
		} else {
			gtk_tree_path_up (path);
		}
	}

	/* Change the selection and remove the channel. The selection has to be changed
 	 * first to ensure that the deref'ing and ref'ing work properly.
 	 */
	gtk_tree_selection_select_path (select, path);
	navigation_model_remove (navtree->model, sess);

	/* We rely on the above select_path call triggering the selection 'changed' event to do book-keeping */

	navigation_tree_update_refs (navtree);

	gtk_tree_path_free (path);
}

void
navigation_tree_remove_server (NavTree *navtree, struct session *sess)
{
	GtkTreePath *path = gtk_tree_row_reference_get_path (navtree->current_rowref);
	GtkTreeSelection *select = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	if (!gtk_tree_path_prev (path)) {
		/* Removing the first server in the navigation tree. */
		GtkTreeModel *sorted = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
		GtkTreeIter  iter;

		gtk_tree_model_get_iter_first (sorted, &iter);

		if (gtk_tree_model_iter_next (sorted, &iter)) {
			/* There's another server after the first one. */
			gtk_tree_selection_select_iter (select, &iter);
			navigation_model_remove (navtree->model, sess);

			/* We rely on the above select_iter call triggering the selection 'changed' event to do book-keeping */
		} else {
			/* The first server is the only server. */
			GtkTreeModel *store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (sorted));

			gtk_tree_model_get_iter_first (store, &iter);
			navigation_tree_server_rm_chans (navtree, &iter);
			gtk_tree_model_get_iter_first (store, &iter);
			gtk_tree_store_set (GTK_TREE_STORE (store), &iter, 1, _("<none>"), -1);
		}
	} else {
		/* This isn't the first server. Select the server above it and remove it. */
		gtk_tree_selection_select_path (select, path);
		navigation_model_remove (navtree->model, sess);
	}

	navigation_tree_update_refs (navtree);

	gtk_tree_path_free (path);
}

void
navigation_tree_server_rm_chans (NavTree *navtree, GtkTreeIter * parent)
{
	GtkTreeModel *sorted = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
	GtkTreeModel *store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (sorted));
	GtkTreeIter child;

	if (gtk_tree_model_iter_children (store, &child, parent)) {
		session *s;
		do {
			gtk_tree_model_get (store, &child, 2, &s, -1);
			fe_close_window (s);
			conversation_panel_remove_session (CONVERSATION_PANEL (gui.conversation_panel), s);
			topic_label_remove_session        (TOPIC_LABEL        (gui.topic_label),        s);
			text_entry_remove_session         (TEXT_ENTRY         (gui.text_entry),         s);
		} while (gtk_tree_model_iter_next (store, &child));
	}
}

/* Channel/server selection functions. */
void
navigation_tree_select_nth_channel (NavTree *navtree, gint chan_num)
{
	GtkTreeView *view;
	GtkTreeModel *model;
	GtkTreeIter server;
	GtkTreeSelection *selection;
	GtkTreePath *path;
	gint kids;

	view = GTK_TREE_VIEW (navtree);
	selection = gtk_tree_view_get_selection (view);
	model = gtk_tree_view_get_model (view);

	/* Make sure we get the an iter in the tree. */
	if (model != NULL && gtk_tree_model_get_iter_first (model, &server)) {
		/* Loop until we run out of channels or until we find the one
		 * we're looking for.
		 */
		do {
			/* Get path to current server. */
			path = gtk_tree_model_get_path (model, &server);

			/* Only count the channels in the server if the list is expanded. */
			if (gtk_tree_view_row_expanded (view, path)) {
				/* Get the number of channels on the current server starting
				 * with the first.
				 */
				kids = gtk_tree_model_iter_n_children (model, &server);
				/* If the server has enough channels to contain the one we're looking
				 * for select it and return.
				 */
				if (chan_num < kids) {
					GtkTreeIter new_iter;

					gtk_tree_model_iter_nth_child (model, &new_iter, &server, chan_num);
					gtk_tree_selection_select_iter (selection, &new_iter);

					return;
				}
				/* If our number wants a channel out of the range of this server
				 * subtract the number of channels in the current server so that
				 * when we find the server that contains the channel we want chan_num
				 * will be the channel's position in the list.
				 */
				chan_num -= kids;
			}
			/* Move to the next channel, if possible. */
		} while (gtk_tree_model_iter_next (model, &server));
	}
}

void
navigation_tree_select_session (NavTree *navtree, struct session *sess)
{
	GtkTreeIter *iter = navigation_model_get_sorted_iter (navtree->model, sess);

	if (iter) {
		GtkTreePath *path;
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

		/* Make sure that if we're selecting a channel it's server is expanded. */
		path = gtk_tree_model_get_path (navtree->model->sorted, iter);
		if (gtk_tree_path_get_depth (path) > 1) {
			gtk_tree_path_up (path);
			gtk_tree_view_expand_row (GTK_TREE_VIEW (navtree), path, TRUE);
		}

		gtk_tree_selection_select_iter (selection, iter);
		gtk_tree_path_free (path);
	}
	gtk_tree_iter_free (iter);
}

void
navigation_tree_select_next_channel (NavTree *navtree)
{
	GtkTreeIter       iter;
	GtkTreeModel     *model;
	GtkTreeSelection *selection;
	GtkTreePath      *path;
	gchar            *path_string;
	gint              depth;
	gint              server_index = 0;
	gint              channel_index = 0;


	/* Get the GtkSelection from the GtkTreeView. */
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	/* If nothing is currently selected, select the first visible channel. */
	if (!gtk_tree_selection_get_selected (selection, &model, &iter)) {
		navigation_tree_select_nth_channel (navtree, 0);
		return;
	}

	/* Get a path to the iter. */
	path = gtk_tree_model_get_path (model, &iter);
	depth = gtk_tree_path_get_depth (path);
	path_string = gtk_tree_path_to_string (path);

	/* If a channel is currently selected... */
	if (depth == 2) {
		gint    channels = 0;
		gchar** split = g_strsplit (path_string, ":", 2);

		server_index  = g_strtod (split[0], NULL);
		channel_index = g_strtod (split[1], NULL);

		g_strfreev (split);

		g_free (path_string);
		path_string = g_strdup_printf ("%d", server_index);
		gtk_tree_model_get_iter_from_string (model, &iter, path_string);
		channels = gtk_tree_model_iter_n_children (model, &iter);

		if (channel_index == channels - 1) {
			channel_index = 0;

			if (++server_index >= navtree->model->servers) {
				navigation_tree_select_nth_channel (navtree, 0);
				g_free (path_string);
				gtk_tree_path_free (path);
				return;
			}

			depth = 1;
		} else {
			channel_index++;

			g_free (path_string);
			path_string = g_strdup_printf ("%d:%d", server_index, channel_index);

			if (!gtk_tree_model_get_iter_from_string (model, &iter, path_string)) {
				g_free (path_string);
				gtk_tree_path_free (path);
				return;
			}
		}
	} else {
		server_index = g_strtod (path_string, NULL);
	}

	if (depth == 1) {
		/* winner becomes true when we've found a server and channel to
		 * select next.
		 */
		gboolean winner = FALSE;

		g_free (path_string);
		path_string = g_strdup_printf ("%d", server_index);

		gtk_tree_model_get_iter_from_string (model, &iter, path_string);

		/* Iterate over all the available servers until we find one with
		 * a channel to select.
		 */
		do {
			/* Check each server for children. */
			if (gtk_tree_model_iter_has_child (model, &iter)) {
				gtk_tree_path_free (path);
				path = gtk_tree_model_get_path (model, &iter);
				/* If a server has children, check to see if it's expanded. */
				if (gtk_tree_view_row_expanded (GTK_TREE_VIEW (navtree), path)) {
					winner = TRUE;
					break;
				}
			}
			server_index++;
		} while (gtk_tree_model_iter_next (model, &iter));

		/* If we reached the end of the list and didn't find a channel,
		 * select the first visible channel.
		 */
		if (winner) {
			g_free (path_string);
			path_string = g_strdup_printf ("%d:%d", server_index, 0);
			gtk_tree_model_get_iter_from_string (model, &iter, path_string);
		} else {
			navigation_tree_select_nth_channel (navtree, 0);
			return;
		}
	}

	gtk_tree_selection_select_iter (selection, &iter);

	/* Clean up. */
	gtk_tree_path_free (path);
	g_free (path_string);
}

void
navigation_tree_select_prev_channel (NavTree *navtree)
{
	GtkTreeModel     *model;
	GtkTreeSelection *selection;
	GtkTreeIter       iter;
	GtkTreePath      *path;
	gint              depth;

	/* Get the GtkTreeSelection. */
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	/* Try to get the currently selected item. Failing that, select the
	 * first visible channel.
	 */
	if (!gtk_tree_selection_get_selected (selection, &model, &iter)) {
		navigation_tree_select_nth_channel (navtree, 0);
		return;
	}

	/* Get the path, depth, and path string. */
	path = gtk_tree_model_get_path (model, &iter);
	depth = gtk_tree_path_get_depth (path);

	if (depth == 2) {
		/* If a channel is selected... */
		if (!gtk_tree_path_prev (path)) {
			/* If it's the first channel on the server, pretend we've got
		 	 * that server selected.
		 	 */
			depth = 1;
			gtk_tree_path_up (path);
		}
	}

	if (depth == 1) {
		if (!gtk_tree_path_prev (path)) {
			/* If it's the first server, just move to the last
			 * channel in the tree.
			 */
			gtk_tree_path_free (path);
			path = gtk_tree_row_reference_get_path (navtree->last_channel);
		} else {
			/* Find a server somewhere above that has children. */
			do {
				int children = 0;

				gtk_tree_model_get_iter (model, &iter, path);
				children = gtk_tree_model_iter_n_children (model, &iter);

				/* If the server has children and is expanded,
				 * set the path to the last entry on this
				 * server.
				 */
				if (children > 0 && gtk_tree_view_row_expanded (GTK_TREE_VIEW (navtree), path)) {
					gtk_tree_path_append_index (path, children-1);
					break;
				}
			} while (gtk_tree_path_prev (path));

			/* If we haven't selected a channel at this point, we
			 * need to select the last channel in the tree.
			 */
			if (gtk_tree_path_get_depth (path) == 1) {
				gtk_tree_path_free (path);
				path = gtk_tree_row_reference_get_path (navtree->last_channel);
			}
		}
	}

	/* At this point path should point to the correct channel for selection.
	 * If we can't get that iter for some reason, clean up and bail out.
	 */
	if (!gtk_tree_model_get_iter (model, &iter, path)) {
		gtk_tree_path_free (path);
		return;
	}

	/* Clean up. */
	gtk_tree_path_free (path);

	/* Select the iter. */
	gtk_tree_selection_select_iter (selection, &iter);
}

void
navigation_tree_select_next_network (NavTree *navtree)
{
	GtkTreeIter      iter;
	GtkTreeModel     *model;
	GtkTreePath      *path;
	GtkTreeSelection *selection;

	/* Get the GtkTreeSelection. */
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	/* Try to get the current selection. */
	if (!gtk_tree_selection_get_selected (selection, &model, &iter)) {
		model = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
		if (navtree->current_rowref) {
			/* If nothing is selected and we have a current_rowref set the iter to that path. */
			path = gtk_tree_row_reference_get_path (navtree->current_rowref);
			gtk_tree_model_get_iter (model, &iter, path);
		} else {
			/* If we have no current_rowref and nothing selected select the first server and return. */
			gtk_tree_model_get_iter_first (model, &iter);
			gtk_tree_selection_select_iter (selection, &iter);
			return;
		}
	} else {
		/* If we have something selected we'll need to set path to the iter. */
		path = gtk_tree_model_get_path (model, &iter);
	}

	if (gtk_tree_path_get_depth (path) > 1) {
		/* If our path is at a depth greater than one it's not a server, make it one. */
		GtkTreeIter parent;
		gtk_tree_model_iter_parent (model, &parent, &iter);
		iter = parent;
	}

	gtk_tree_path_free(path);

	if (!gtk_tree_model_iter_next (model, &iter)) {
		/* If we can't move to the next we need to move back to the root. Move iter to the root. */
		GtkTreeIter root;
		gtk_tree_model_get_iter_first (model, &root);
		iter = root;
	}

	/* Select the iter. */
	gtk_tree_selection_select_iter (selection, &iter);
}

void
navigation_tree_select_prev_network (NavTree *navtree)
{
	GtkTreeIter       iter;
	GtkTreeModel     *model;
	GtkTreeSelection *selection;
	GtkTreePath      *path;

	/* Get our tree view and selection. */
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (navtree));

	if (!gtk_tree_selection_get_selected (selection, &model, &iter)) {
		/* If there is nothing selected in the GtkTreeSelection... */
		/* Get the model. */
		model = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
		if (navtree->current_rowref) {
			/* If there's a current_rowref set path to that. */
			path = gtk_tree_row_reference_get_path (navtree->current_rowref);
		} else {
			/* Otherwise set path to the root. */
			path = gtk_tree_path_new_from_string ("0");
		}
	} else {
		/* If there is a selection set path to that point. */
		path = gtk_tree_model_get_path (model, &iter);
	}

	/* If the path isn't a server move it up one. */
	if (gtk_tree_path_get_depth (path) > 1)
		gtk_tree_path_up (path);

	/* If we can't move it back... */
	if (!gtk_tree_path_prev (path)) {
		/* Find the last entry in the server. */
		GtkTreeIter current, previous;
		gtk_tree_model_get_iter_first (model, &current);
		previous = current;
		while (gtk_tree_model_iter_next (model, &current))
			previous = current;
		/* Free the old path and set it to the iter that points to the last entry. */
		gtk_tree_path_free (path);
		path = gtk_tree_model_get_path (model, &previous);
	}

	/* Select the path in our GtkTreeSelection. */
	gtk_tree_selection_select_path (selection, path);
	gtk_tree_path_free(path);
}

session*
navigation_tree_get_selected_session (gboolean *connected)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;
	gboolean temp;

	treeview = GTK_TREE_VIEW (gui.server_tree);
	select = gtk_tree_view_get_selection (treeview);
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 2, &s, 6, &temp, -1);
		if (connected)
			*connected = temp;
		return s;
	}
	return NULL;
}

static void
navigation_tree_update_refs (NavTree *navtree)
{
	GtkTreeModel*	model = GTK_TREE_MODEL (navtree->model->sorted);
	GtkTreeIter	iter;
	GtkTreePath*	path;

	/* If there is nothing in the tree, set the references to NULL. */
	if (!gtk_tree_model_get_iter_first (model, &iter)) {
		navtree->last_server = NULL;
		navtree->last_channel = NULL;
		return;
	}

	path = gtk_tree_path_new_first ();

	/*** Find the last network in the treeview. ***/
	if (navtree->last_server)
		/* Free the current reference if there is one. */
		gtk_tree_row_reference_free (navtree->last_server);

	/* Find the last server. */
	while (gtk_tree_model_iter_next (model, &iter))
		gtk_tree_path_next (path);

	/* Set the reference. */
	navtree->last_server = gtk_tree_row_reference_new (model, path);

	/*** Find the last visible channel in the treeview. ***/

	if (navtree->last_channel)
		/* Free the current reference if there is one. */
		gtk_tree_row_reference_free (navtree->last_channel);

	/* Find the server furthest down the list with visible children. */
	do {
		gint	children = 0;

		gtk_tree_model_get_iter (model, &iter, path);
		children = gtk_tree_model_iter_n_children (model, &iter);

		/* When we find a server with visible children, append the index
		 * to the path and break out of the loop.
		 */
		if (children > 0 && gtk_tree_view_row_expanded (GTK_TREE_VIEW (navtree), path)) {
			gtk_tree_path_append_index (path, children - 1);
			break;
		}
	} while (gtk_tree_path_prev (path));

	if (gtk_tree_path_get_depth (path) == 1)
		/* If the path is still pointing to servers, we must not be
		 * connected to any channels. Set this reference to NULL.
		 */
		navtree->last_channel = NULL;
	else
		/* At this point, path refers to the last channel in the
		 * treeview.
		 */
		navtree->last_channel = gtk_tree_row_reference_new (model, path);

	gtk_tree_path_free (path);
}

/* Misc. Functions. */
static gboolean
navigation_tree_set_channel_name_iterate (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	/* Iterator function for set_channel_name. */
	gpointer s;
	gtk_tree_model_get (model, iter, 2, &s, -1);
	if (s == data) {
		struct session *sess = s;
		gtk_tree_store_set (GTK_TREE_STORE (model), iter, 1, (sess->channel), 4, NULL, 6, TRUE, -1);
		return TRUE;
	}
	return FALSE;
}

void
navigation_tree_set_channel_name (NavTree *navtree, struct session *sess)
{
	GtkTreeModel *store, *model;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
	store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));

	gtk_tree_model_foreach (store, navigation_tree_set_channel_name_iterate, sess);
}

gboolean
navigation_tree_server_is_connected (NavTree *navtree, gchar *name)
{
	GtkTreeModel *model, *store;
	GtkTreeIter iter;
	GtkTreePath *path;
	gboolean connected;
	gchar *network;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (navtree));
	store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));

	path = gtk_tree_path_new_first ();

	while (gtk_tree_model_get_iter (store, &iter, path)) {
		gtk_tree_model_get (store, &iter, 1, &network, 6, &connected, -1);
		if (strcmp (network, name) == 0) {
			return connected;
		}
		gtk_tree_path_next (path);
	}

	return FALSE;
}


/***** Context Menus *****/
static GtkWidget *
navigation_context (GtkWidget *treeview, session *selected)
{
	switch (selected->type) {
	case SESS_SERVER:
		return server_context (treeview, selected);
	case SESS_CHANNEL:
		return channel_context (treeview, selected);
	case SESS_DIALOG:
		return dialog_context (treeview, selected);
	default:
		g_return_val_if_reached (NULL);
	}
}

static GtkWidget *
server_context (GtkWidget *treeview, session *selected)
{
	GtkWidget *menu;
	GtkAction *action;

	menu = gtk_ui_manager_get_widget (gui.manager, "/ServerPopup");
	g_return_val_if_fail (menu != NULL, NULL);

	if (((ircnet *)selected->server->network) == NULL) {
		/* disable the auto-connect action.
		 * FIXME: it would be really nice to be able to configure this server
		 * as part of a network here.
		 */
		action = gtk_action_group_get_action (action_group, "ServerAutoConnect");
		gtk_action_set_sensitive (action, FALSE);
	} else if (selected != NULL) {
		/* check if the network is in the auto-connect list */
		action = gtk_action_group_get_action (action_group, "ServerAutoConnect");
		gtk_action_set_sensitive (action, TRUE);
		if (((ircnet *)selected->server->network)->flags & FLAG_AUTO_CONNECT)
			gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
		else
			gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), FALSE);
	}
	return menu;
}

static GtkWidget *
channel_context (GtkWidget *treeview, session *selected)
{
	GtkWidget *menu;
	GtkAction *action;
	ircnet *network;

	if (selected->total) {
		menu = gtk_ui_manager_get_widget (gui.manager, "/ChannelJoinedPopup");
	} else {
		menu = gtk_ui_manager_get_widget (gui.manager, "/ChannelUnjoinedPopup");
	}
	g_return_val_if_fail (menu != NULL, NULL);

	/* check if the channel is in the auto-join list */
	action = gtk_action_group_get_action (action_group, "ChannelAutoJoin");
	network = selected->server->network;

	if (network == NULL) {
		gtk_action_set_sensitive (action, FALSE);
	} else {
		gtk_action_set_sensitive (action, TRUE);
		if (channel_is_autojoin (network, selected->channel))
			gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
		else
			gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), FALSE);
	}

	return menu;
}

static GtkWidget *
dialog_context (GtkWidget *treeview, session *selected)
{
	GtkWidget *menu;

	menu = gtk_ui_manager_get_widget (gui.manager, "/DialogPopup");
	g_return_val_if_fail (menu != NULL, NULL);

	return menu;
}

static void
show_context_menu (GtkWidget *treeview, GdkEventButton *event)
{
	GtkTreePath *path;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkWidget *menu;
	session *s;

	if (NAVTREE (treeview)->current_rowref == NULL)
		return;

	model = gtk_tree_row_reference_get_model (NAVTREE (treeview)->current_rowref);
	path = gtk_tree_row_reference_get_path (NAVTREE (treeview)->current_rowref);
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_path_free(path);
	gtk_tree_model_get (model, &iter, 2, &s, -1);

	if (s == NULL)
		return;

	menu = navigation_context (treeview, s); /* FIXME */	
	g_return_if_fail (menu != NULL);

	if (event != NULL) {
		gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
				event->button, event->time);
	} else {
		gtk_menu_popup (GTK_MENU (menu), NULL, NULL,
				menu_position_under_tree_view, treeview,
				0, gtk_get_current_event_time ());

		gtk_menu_shell_select_first (GTK_MENU_SHELL (menu), FALSE);
	}
}

/***** Callbacks *****/
static gboolean
click (GtkWidget *treeview, GdkEventButton *event, gpointer data)
{
	GtkTreePath *path;
	GtkTreeSelection *select;

	if (!event)
		return FALSE;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	if (gtk_tree_selection_get_selected (select, NULL, NULL) == FALSE)
		return FALSE;

	if (event->button == 3) {
		if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (treeview), event->x, event->y, &path, 0, 0, 0)) {
			gtk_tree_selection_unselect_all (select);
			gtk_tree_selection_select_path (select, path);
			gtk_tree_path_free (path);
		}

		show_context_menu (treeview, event);
		return TRUE;
	}

	g_object_set (G_OBJECT (treeview), "can-focus", FALSE, NULL);
	return FALSE;
}

static gboolean
declick (GtkWidget *treeview, GdkEventButton *e, gpointer data)
{
	gint position;

	position = gtk_editable_get_position (GTK_EDITABLE (gui.text_entry));
	gtk_widget_grab_focus (gui.text_entry);
	gtk_editable_set_position (GTK_EDITABLE (gui.text_entry), position);
	g_object_set (G_OBJECT (treeview), "can-focus", TRUE, NULL);
	return FALSE;
}

static gboolean
popup_menu (GtkWidget *treeview, gpointer user_data)
{
	show_context_menu (treeview, NULL);
	return TRUE;
}

static void
navigation_selection_changed (GtkTreeSelection *treeselection, gpointer user_data)
{
	GtkTreeIter   iter;
	GtkTreeIter   newiter;
	GtkTreeModel *model;
	GtkTreeModel *store;
	GtkWidget    *button;
	gpointer     *s;
	session      *sess;

	if (gui.server_tree == NULL)
		return;


	if (gui.server_tree->current_rowref != NULL)
		navigation_model_rowref_deref (gui.server_tree->current_rowref);

	/* If find bar is open, hide it */
	find_bar_close (FIND_BAR (gui.find_bar));

	/* XXX: This sets model to be the GtkTreeModelSort used by the NavTree, it is
	 *      not a GtkTreeModel. The iter is for that ModelSort.
	 */
	if (gtk_tree_selection_get_selected (treeselection, &model, &iter) && gui.current_session) {
		GtkWidget *menuitem;
		GtkTreePath *path;

		path = gtk_tree_model_get_path (model, &iter);

		/* Update current_rowref. */
		if (gui.server_tree->current_rowref) {
			gtk_tree_row_reference_free (gui.server_tree->current_rowref);
		}

		gui.server_tree->current_rowref = gtk_tree_row_reference_new(model, path);
		navigation_model_rowref_ref (gui.server_tree->current_rowref);
		gtk_tree_selection_get_selected (treeselection, &model, &iter);

		/* Get the session for the new selection. */
		gtk_tree_model_get (model, &iter, 2, &s, -1);
		sess = (session *) s;

		/* Clear the icons. */
		sess->nick_said = FALSE;
		sess->msg_said = FALSE;
		sess->new_data = FALSE;

		/* Make this our current session. */
		gui.current_session = sess;

		/* Change the model in the userlist to the one for this session. */
		gtk_tree_view_set_model (GTK_TREE_VIEW (gui.userlist),
				         GTK_TREE_MODEL (userlist_get_store (u, sess)));

		/* Set our nick. */
		set_nickname (sess->server, NULL);

		/* Change the window name. */
		if (sess->server->network == NULL) {
			rename_main_window (NULL, sess->channel);
		} else {
			ircnet *net = sess->server->network;
			rename_main_window (net->name, sess->channel);
		}

		/* Set widget sensitivity based on type of the current "tab" */
		menuitem = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/DiscussionMenu/DiscussionChangeTopicItem");
		if (menuitem == NULL)
			g_warning ("can't disable topic change menu item");
		else
			gtk_widget_set_sensitive (menuitem, sess->type == SESS_CHANNEL);
		button = glade_xml_get_widget (gui.xml, "close discussion");
		gtk_widget_set_sensitive (button, sess->type != SESS_SERVER);

		menuitem = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/DiscussionMenu/DiscussionUsersItem");
		gtk_widget_set_sensitive (menuitem, sess->type != SESS_SERVER);
		menuitem = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/DiscussionMenu/DiscussionLeaveItem");
		gtk_widget_set_sensitive (menuitem, sess->type != SESS_SERVER);
		menuitem = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/DiscussionMenu/DiscussionCloseItem");
		gtk_widget_set_sensitive (menuitem, sess->type != SESS_SERVER);
		gtk_widget_set_sensitive (gui.userlist_toggle, sess->type == SESS_CHANNEL);

		/* remove any icon that exists */
		store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));
		gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model), &newiter, &iter);
		gtk_tree_store_set (GTK_TREE_STORE (store), &newiter, 0, NULL, 3, 0, -1);

		conversation_panel_set_current (CONVERSATION_PANEL (gui.conversation_panel), sess);
		topic_label_set_current        (TOPIC_LABEL        (gui.topic_label),        sess);
		text_entry_set_current         (TEXT_ENTRY         (gui.text_entry),         sess);
		status_bar_set_current         (STATUS_BAR         (gui.status_bar),         sess->server);

		gtk_widget_grab_focus (gui.text_entry);
		gtk_editable_set_position (GTK_EDITABLE (gui.text_entry), -1);

		/* Set the label of the user list button */
		userlist_set_user_button (u, sess);

		/* Emit "focus tab" event */
		plugin_emit_dummy_print (sess, "Focus Tab");
	}
}

static void
row_expanded (GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data)
{
	GtkTreePath *current_path;

	if (NAVTREE (treeview)->current_rowref == NULL)
		return;
	current_path = gtk_tree_row_reference_get_path (NAVTREE (treeview)->current_rowref);

	/* If we had something selected before the row was collapsed make sure it gets selected. */
	if (current_path && gtk_tree_path_is_ancestor (path, current_path)) {
		GtkTreeSelection *selection;
		selection = gtk_tree_view_get_selection (treeview);
		g_signal_handler_block ((gpointer) selection, NAVTREE (treeview)->selection_changed_id);
		gtk_tree_selection_select_path (selection, current_path);
		g_signal_handler_unblock ((gpointer) selection, NAVTREE (treeview)->selection_changed_id);
	}

	navigation_tree_update_refs (NAVTREE (treeview));
	gtk_tree_path_free(current_path);
}

static void
row_collapsed (GtkTreeView * treeview, GtkTreeIter * iter, GtkTreePath * path, gpointer user_data)
{
	navigation_tree_update_refs (NAVTREE (treeview));
}

/********** NavModel **********/

static void navigation_model_init       (NavModel * navmodel);
static void navigation_model_class_init (NavModelClass * klass);
static void navigation_model_dispose    (GObject * object);
static void navigation_model_finalize   (GObject * object);

GType
navigation_model_get_type (void)
{
	static GType navigation_model_type = 0;
	if (!navigation_model_type) {
		static const GTypeInfo navigation_model_info = {
			sizeof (NavTreeClass),
			NULL,		/* base init. */
			NULL,		/* base finalize. */
			(GClassInitFunc) navigation_model_class_init,
			NULL,		/* class_finalize. */
			NULL,		/* class_data. */
			sizeof (NavModel),
			0,			/* n_preallocs. */
			(GInstanceInitFunc) navigation_model_init,
		};

		navigation_model_type = g_type_register_static (G_TYPE_OBJECT, "NavModel", &navigation_model_info, 0);
	}

	return navigation_model_type;
}

static void
navigation_model_init (NavModel *navmodel)
{
	navmodel->store = gtk_tree_store_new (7, GDK_TYPE_PIXBUF, /* status image */
	                                         G_TYPE_STRING,   /* name */
	                                         G_TYPE_POINTER,  /* session pointer */
	                                         G_TYPE_INT,      /* status # (for tracking highest state) */
	                                         GDK_TYPE_COLOR,  /* status color (disconnected, etc) */
	                                         G_TYPE_INT,      /* reference count */
	                                         G_TYPE_BOOLEAN); /* connected */
	navmodel->sorted = gtk_tree_model_sort_new_with_model (GTK_TREE_MODEL (navmodel->store));
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (navmodel->sorted), 1, GTK_SORT_ASCENDING);
	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (navmodel->sorted), 1, gtk_tree_iter_sort_func_nocase, NULL, NULL);

	navmodel->servers = 0;

	/* Set up our hash table for direct hashing. */
	navmodel->session_rows = g_hash_table_new (g_direct_hash, g_direct_equal);
}

static void
navigation_model_class_init (NavModelClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;
	object_class->dispose = navigation_model_dispose;
	object_class->finalize = navigation_model_finalize;
}

static void
destroy_rowref (session *sess, GtkTreeRowReference *rowref, gpointer data)
{
	gtk_tree_row_reference_free (rowref);
}

static void
navigation_model_dispose (GObject *object)
{
	NavModel *navmodel = NAVMODEL (object);
	g_object_unref ((gpointer) navmodel->store);
	g_object_unref ((gpointer) navmodel->sorted);
	g_hash_table_foreach_remove (navmodel->session_rows, (GHRFunc) destroy_rowref, NULL);
}

static void
navigation_model_finalize (GObject *object)
{
	NavModel *model = (NavModel *) object;
	g_hash_table_destroy (model->session_rows);
}

/* New NavModel. */
NavModel *
navigation_model_new ()
{
	return NAVMODEL (g_object_new (navigation_model_get_type (), NULL));
}

void
navigation_model_add_new_network (NavModel *model, struct session *sess)
{
	GtkTreeIter          iter;
	GtkTreePath         *path;
	GtkTreeRowReference *rowref;

	gtk_tree_store_append (model->store, &iter, NULL);

	/* Add the new server to the model. By default connected is false. */
	gtk_tree_store_set (model->store, &iter, 1, _("<none>"), 2, sess, 3, 0, 4, NULL, 5, 0, 6, FALSE, -1);

	/* Insert a row reference to the new entry in the unsorted store in the
	 * hash table. This allows us to look up entries in the navigation model
	 * by session.
	 */
	path = gtk_tree_model_get_path (GTK_TREE_MODEL (model->store), &iter);
	rowref = gtk_tree_row_reference_new (GTK_TREE_MODEL (model->store), path);
	g_hash_table_insert (model->session_rows, (gpointer) sess, (gpointer) rowref);

	model->servers++;
	gtk_tree_path_free (path);
}

static gboolean
navigation_model_create_new_channel_entry_iterate (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, struct session *data)
{
	struct session *s;
	gint            position;

	gtk_tree_model_get (model, iter, 2, &s, -1);
	if (s->type == SESS_SERVER && s->server == data->server) {
		GtkTreeIter child;
		GtkTreeRowReference *rowref;
		GtkTreePath *path;

		gtk_tree_store_append (GTK_TREE_STORE (model), &child, iter);
		/* Add the new channel to the store. By default connected is true. */
		gtk_tree_store_set (GTK_TREE_STORE (model), &child, 1, data->channel, 2, data, 3, 0, 4, NULL, 5, 0, 6, TRUE, -1);

		path = gtk_tree_model_get_path (model, &child);
		rowref = gtk_tree_row_reference_new (model, path);
		g_hash_table_insert (gui.tree_model->session_rows, (gpointer) data, (gpointer) rowref);

		position = gtk_editable_get_position (GTK_EDITABLE (gui.text_entry));
		gtk_widget_grab_focus (gui.text_entry);
		gtk_editable_set_position (GTK_EDITABLE (gui.text_entry), position);

		gtk_tree_path_free (path);
		return TRUE;
	}
	return FALSE;
}

void
navigation_model_add_new_channel (NavModel *model, struct session *sess)
{
	gtk_tree_model_foreach (GTK_TREE_MODEL (model->store), (GtkTreeModelForeachFunc) navigation_model_create_new_channel_entry_iterate, (gpointer) sess);
}

void
navigation_model_remove (NavModel *model, struct session *sess)
{
	GtkTreeIter *iter = navigation_model_get_unsorted_iter (model, sess);
	GtkTreePath *path = gtk_tree_model_get_path (GTK_TREE_MODEL (model->store), iter);

	if (gtk_tree_path_get_depth (path) == 1)
		model->servers--;

	gtk_tree_store_remove (model->store, iter);

	gtk_tree_path_free (path);
	gtk_tree_iter_free (iter);
}

GtkTreeIter *
navigation_model_get_sorted_iter (NavModel *model, struct session *sess)
{
	GtkTreeIter *iter = NULL;
	GtkTreePath *path, *sorted;
	GtkTreeRowReference *row;

	row = (GtkTreeRowReference *) g_hash_table_lookup (model->session_rows, (gpointer) sess);
	if (row) {
		iter = g_new (GtkTreeIter, 1);
		path = gtk_tree_row_reference_get_path (row);
		sorted = gtk_tree_model_sort_convert_child_path_to_path (GTK_TREE_MODEL_SORT (model->sorted), path);

		gtk_tree_model_get_iter (model->sorted, iter, sorted);

		gtk_tree_path_free (sorted);
		gtk_tree_path_free (path);
	}

	return iter;
}

GtkTreeIter *
navigation_model_get_unsorted_iter (NavModel * model, struct session * sess)
{
	GtkTreeIter *iter = g_new (GtkTreeIter, 1);
	GtkTreePath *path;

	path = gtk_tree_row_reference_get_path ((GtkTreeRowReference *) g_hash_table_lookup (model->session_rows, (gpointer) sess));
	gtk_tree_model_get_iter (GTK_TREE_MODEL (model->store), iter, path);

	gtk_tree_path_free (path);
	return iter;
}

static gboolean
navigation_model_set_disconn_iterate (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
	gpointer s;
	gtk_tree_model_get (model, iter, 2, &s, -1);
	if (s == data) {
		/* Change the color and set connected to false. */
		gtk_tree_store_set (GTK_TREE_STORE (model), iter, 4, &colors[40], 6, FALSE, -1);
		return TRUE;
	}
	return FALSE;
}

void
navigation_model_set_disconn (NavModel * model, struct session *sess)
{
	gtk_tree_model_foreach (GTK_TREE_MODEL (model->store), navigation_model_set_disconn_iterate, (gpointer) sess);
}

static gboolean
navigation_model_set_hilight_iterate (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
	gpointer s;
	gint e, ref;

	gtk_tree_model_get (model, iter, 2, &s, 3, &e, 5, &ref, -1);

	if (s == data && ref == 0) {
		struct session *sess = s;

		if (sess->nick_said) {
			gtk_tree_store_set (GTK_TREE_STORE (model), iter, 0, pix_nicksaid, 3, 3, -1);
			return TRUE;
		}
		if (sess->msg_said && e < 2) {
			if (sess->type != SESS_DIALOG)
				gtk_tree_store_set (GTK_TREE_STORE (model), iter, 0, pix_msgsaid, 3, 2, -1);
			else
				gtk_tree_store_set (GTK_TREE_STORE (model), iter, 0, pix_nicksaid, 3, 3, -1);
			return TRUE;
		}
		if (sess->new_data && e < 1) {
			gtk_tree_store_set (GTK_TREE_STORE (model), iter, 0, pix_newdata, 3, 1, -1);
			return TRUE;
		}
	}
	return FALSE;
}

void
navigation_model_set_hilight (NavModel * model, struct session *sess)
{
	if (sess == gui.current_session) {
		sess->nick_said = FALSE;
		sess->msg_said = FALSE;
		sess->new_data = FALSE;
		return;
	}
	gtk_tree_model_foreach (GTK_TREE_MODEL (model->store), navigation_model_set_hilight_iterate, (gpointer) sess);
}

void
navigation_model_rowref_ref (GtkTreeRowReference * rowref)
{
	gint ref_count;
	GtkTreeIter iter, childiter;
	GtkTreeModel *model, *childmodel;
	GtkTreePath *path;

	model = gtk_tree_row_reference_get_model (rowref);
	path = gtk_tree_row_reference_get_path (rowref);
	childmodel = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));

	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter, 5, &ref_count, -1);
	gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model), &childiter, &iter);
	gtk_tree_store_set (GTK_TREE_STORE (childmodel), &childiter, 5, ref_count + 1, -1);
	gtk_tree_path_free (path);
}

void
navigation_model_rowref_deref (GtkTreeRowReference *rowref)
{
	gint ref_count;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreePath *path;

	model = gtk_tree_row_reference_get_model (rowref);
	path = gtk_tree_row_reference_get_path (rowref);

	if (gtk_tree_model_get_iter (model, &iter, path) == FALSE) {
		g_critical ("path is invalid in navigation_model_rowref_deref\n");
		return;
	}

	gtk_tree_model_get (model, &iter, 5, &ref_count, -1);

	if (ref_count > 0) {
		GtkTreeModel *childmodel;
		GtkTreeIter childiter;

		childmodel = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));
		gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model), &childiter, &iter);
		gtk_tree_store_set (GTK_TREE_STORE (childmodel), &childiter, 5, ref_count - 1, -1);
	}

	gtk_tree_path_free (path);
}

void
navigation_model_sorted_iter_ref (NavModel * model, GtkTreeIter * iter)
{
	gint ref_count;
	GtkTreeIter unsorted;
	gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model->sorted), &unsorted, iter);
	gtk_tree_model_get (model->sorted, iter, 5, &ref_count, -1);

	gtk_tree_store_set (model->store, &unsorted, 5, ref_count + 1, -1);
}

void
navigation_model_sorted_iter_unref (NavModel * model, GtkTreeIter * iter)
{
	gint ref_count;
	GtkTreeIter unsorted;

	gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model->sorted), &unsorted, iter);
	gtk_tree_model_get (model->sorted, iter, 5, &ref_count, -1);

	if (ref_count > 0)
		gtk_tree_store_set (model->store, &unsorted, 5, ref_count - 1, -1);
}

struct server *
navigation_model_get_server (NavModel *model, ircnet *network)
{
	struct session *sess;
	struct server *serv;
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (model->store), &iter) == FALSE)
		return NULL;

	do {
		gtk_tree_model_get (GTK_TREE_MODEL (model->store), &iter, 2, &sess, -1);
		if (sess) {
			serv = sess->server;
			if (serv->network == network)
				return serv;
		}
	} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (model->store), &iter));
	return NULL;
}

static void
on_server_information (GtkAction * action, gpointer data)
{
}

static void
on_server_reconnect (GtkAction * action, gpointer data)
{
	session *s;

	s = navigation_tree_get_selected_session (NULL);
	if (s)
		s->server->auto_reconnect (s->server, FALSE, -1);
}

static void
on_server_disconnect (GtkAction * action, gpointer data)
{
	GtkTreeView      *treeview;
	GtkTreeSelection *select;
	GtkTreeModel     *model;
	GtkTreeModel     *store;
	GtkTreeIter       iter;
	GtkTreeIter       newiter;
	session          *s;

	treeview = GTK_TREE_VIEW (gui.server_tree);
	select = gtk_tree_view_get_selection (treeview);
	if (!gtk_tree_selection_get_selected (select, &model, &iter))
		return;

	gtk_tree_model_get (model, &iter, 2, &s, -1);
	if (s) {
		s->server->disconnect (s, TRUE, -1);

		/* disconnect call may have changed tree model, so we have to refresh
		 * the model/iter here */
		gtk_tree_selection_get_selected (select, &model, &iter);
	}

	/* Mark as disconnected */
	store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));
	gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model), &newiter, &iter);
	gtk_tree_store_set (GTK_TREE_STORE (store), &newiter, 6, FALSE, -1);
}

static void
on_server_channel_list (GtkAction * action, gpointer data)
{
	session *s;

	s = navigation_tree_get_selected_session (NULL);
	if (s)
		create_channel_list (s);
}

static void
on_save (GtkAction * action, gpointer data)
{
	conversation_panel_save_current (CONVERSATION_PANEL (gui.conversation_panel));
}

static void
on_close (GtkAction * action, gpointer data)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	session *s;
	gboolean joined = FALSE;

	treeview = GTK_TREE_VIEW (gui.server_tree);
	select = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 2, &s, 6, &joined, -1);

		if (s->type == SESS_SERVER) {
			status_bar_remove_server (STATUS_BAR (gui.status_bar), s->server);
			s->server->disconnect (s, TRUE, -1);
		}
		conversation_panel_remove_session (CONVERSATION_PANEL (gui.conversation_panel), s);
		topic_label_remove_session        (TOPIC_LABEL        (gui.topic_label),        s);
		text_entry_remove_session         (TEXT_ENTRY         (gui.text_entry),         s);
		fe_close_window (s);
	}
}

static void
on_find (GtkAction * action, gpointer data)
{
	find_bar_open (FIND_BAR (gui.find_bar));
}

static void
on_channel_leave (GtkAction * action, gpointer data)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model, *store;
	GtkTreeIter iter, newiter;
	session *s;

	treeview = GTK_TREE_VIEW (gui.server_tree);
	select = gtk_tree_view_get_selection (treeview);
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 2, &s, -1);
		store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));
		if ((s->type == SESS_CHANNEL) && (s->channel[0] != '\0')) {
			GConfClient *client;
			gchar *text;

			client = gconf_client_get_default ();
			text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
			if (text == NULL)
				text = g_strdup (_("Ex-Chat"));
			s->server->p_part (s->server, s->channel, text);
			g_object_unref (client);
			g_free (text);
		}
		gtk_tree_model_sort_convert_iter_to_child_iter (GTK_TREE_MODEL_SORT (model), &newiter, &iter);
		gtk_tree_store_set (GTK_TREE_STORE (store), &newiter, 4, &colors[40], 6, FALSE, -1);
	}
}

static void
on_channel_bans (GtkAction * action, gpointer data)
{
}

static void
on_channel_join (GtkAction *action, gpointer data)
{
	GtkTreeView *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model, *store;
	GtkTreeIter iter;
	gchar *channel;
	session *s;

	treeview = GTK_TREE_VIEW (gui.server_tree);
	select = gtk_tree_view_get_selection (treeview);
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 1, &channel, 2, &s, -1);
		store = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));
		if ((s->type == SESS_CHANNEL)) {
			s->server->p_join(s->server, channel, "");
		}
		g_free (channel);
	}
}

static void
on_server_autoconnect (GtkAction *action, gpointer data)
{
	session *sess;
	gboolean autoconnect, active;
	ircnet *network;

	sess = navigation_tree_get_selected_session (NULL);
	network = sess->server->network;
	autoconnect = network->flags & FLAG_AUTO_CONNECT;
	active = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action) );

	if (sess != NULL) {
		if (autoconnect && !active) {
			/* remove server from autoconnect list */
			network->flags &= !FLAG_AUTO_CONNECT;
			servlist_save ();
		}
		if (!autoconnect && active) {
			/* add server to autoconnect list */
			network->flags |= FLAG_AUTO_CONNECT;
			servlist_save ();
		}
	}
}

static void
on_channel_autojoin (GtkAction *action, gpointer data)
{
	session *sess;
	gboolean autojoin, active;
	ircnet *network;
	gchar **autojoins = NULL;
	gchar *tmp;

	sess = navigation_tree_get_selected_session (NULL);
	network = sess->server->network;
	autojoin = channel_is_autojoin (network, sess->channel);
	active = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action) );

	if (sess != NULL) {
		if (autojoin && !active) {
			/* remove channel from autojoin list */
			gchar **channels, **keys;
			gchar *new_channels, *new_keys;
			gint i;
			gboolean keys_done;

			new_channels = new_keys = NULL;
			autojoins = g_strsplit (network->autojoin, " ", 0);
			channels = g_strsplit (autojoins[0], ",", 0);

			if (autojoins[1]) {
				keys =  g_strsplit (autojoins[1], ",", 0);
				keys_done = FALSE;
			} else {
				keys = NULL;
				keys_done = TRUE;
			}

			g_free (network->autojoin);
			network->autojoin = NULL;

			for (i=0; channels[i]; i++) {
				if (strcmp (channels[i], sess->channel) != 0) {
					if (new_channels == NULL)
						new_channels = g_strdup (channels[i]);
					else {
						tmp = new_channels;
						new_channels = g_strdup_printf ("%s,%s", new_channels, channels[i]);
						g_free (tmp);
					}

					if (!keys_done && keys[i]) {
						if (new_keys == NULL)
							new_keys = g_strdup (keys[i]);
						else {
							tmp = new_keys;
							new_keys = g_strdup_printf ("%s,%s", new_keys, keys[i]);
							g_free (tmp);
						}
					}
					else
						keys_done = TRUE;
				}
				else if (!keys_done && !keys[i])
					keys_done = TRUE;
			}

			if (new_keys) {
				network->autojoin = g_strdup_printf ("%s %s", new_channels, new_keys);
				g_free (new_channels);
				g_free (new_keys);
			}
			else
				network->autojoin = new_channels;

			servlist_save ();
			g_strfreev (channels);
			if (keys) g_strfreev (keys);
		}

		if (!autojoin && active) {
			/* add channel to autojoin list */
			/* FIXME: we should save the key of the channel is there is one */
			if (network->autojoin == NULL) {
				network->autojoin = g_strdup (sess->channel);
			}
			else {
				autojoins = g_strsplit (network->autojoin, " ", 0);
				tmp = network->autojoin;

				if (autojoins[1])
					network->autojoin = g_strdup_printf ("%s,%s %s", autojoins[0], sess->channel, autojoins[1]);
				else
					network->autojoin = g_strdup_printf ("%s,%s", autojoins[0], sess->channel);

				g_free (tmp);
			}

			servlist_save ();
		}
		
		if (autojoins) g_strfreev (autojoins);
	}
}

static gboolean
channel_is_autojoin (ircnet *network, gchar *chan)
{
	gchar **autojoin, **channels;
	gint i;
	gboolean find = FALSE;

	if (network->autojoin == NULL) return FALSE;

	autojoin = g_strsplit (network->autojoin, " ", 0);
	channels = g_strsplit (autojoin[0], ",", 0);
	for (i=0; channels[i] && !find; i++)
		if (!strcmp (channels[i], chan))
			find = TRUE;

	g_strfreev (autojoin);
	g_strfreev (channels);

	return find;
}
