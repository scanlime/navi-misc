/*
 * main_window.c - main GUI window functions
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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

#include <libgnome/libgnome.h>
#include <gconf/gconf-client.h>
#include "main_window.h"
#include "connect_dialog.h"
#include "about.h"
#include "userlist_gui.h"
#include "../common/xchatc.h"
#include "../common/outbound.h"
#include "gui.h"
#include "channel_list.h"
#include "preferences.h"
#include "navigation_tree.h"
#include "textgui.h"
#include "palette.h"
#include "transfers.h"

#ifdef HAVE_GTKSPELL
#include <gtkspell/gtkspell.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static void on_main_window_close (GtkWidget *widget, GdkEvent *event, gpointer data);
static void on_irc_quit_menu_activate (GtkWidget *widget, gpointer data);
static void on_irc_file_transfers_menu_activate (GtkWidget *widget, gpointer data);
static void on_irc_connect_menu_activate (GtkWidget *widget, gpointer data);
static void on_edit_cut_menu_activate (GtkWidget *widget, gpointer data);
static void on_edit_copy_menu_activate (GtkWidget *widget, gpointer data);
static void on_edit_paste_menu_activate (GtkWidget *widget, gpointer data);
static void on_edit_clear_menu_activate (GtkWidget *widget, gpointer data);
static void on_edit_preferences_menu_activate (GtkWidget *widget, gpointer data);
static void on_insert_color_code_menu_activate (GtkWidget *widget, gpointer data);
static void on_network_information_menu_activate (GtkWidget *widget, gpointer data);
static void on_network_reconnect_menu_activate (GtkWidget *widget, gpointer data);
static void on_network_disconnect_menu_activate (GtkWidget *widget, gpointer data);
static void on_network_channels_menu_activate (GtkWidget *widget, gpointer data);
static void on_network_users_menu_activate (GtkWidget *widget, gpointer data);
static void on_network_collapse_expand_activate (GtkWidget *widget, gpointer data);
static void on_discussion_save_activate (GtkWidget *widget, gpointer data);
static void on_discussion_save_as_activate (GtkWidget *widget, gpointer data);
static void on_discussion_leave_activate (GtkWidget *widget, gpointer data);
static void on_discussion_close_activate (GtkWidget *widget, gpointer data);
static void on_discussion_find_activate (GtkWidget *widget, gpointer data);
static void on_discussion_find_next_activate (GtkWidget *widget, gpointer data);
static void on_discussion_clear_window_activate (GtkWidget *widget, gpointer data);
static void on_discussion_bans_activate (GtkWidget *widget, gpointer data);
static void on_go_previous_network_activate (GtkWidget *widget, gpointer data);
static void on_go_next_network_activate (GtkWidget *widget, gpointer data);
static void on_go_previous_discussion_activate (GtkWidget *widget, gpointer data);
static void on_go_next_discussion_activate (GtkWidget *widget, gpointer data);
static void on_discussion_jump_activate (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void on_discussion_plus_activate (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void on_discussion_minus_activate (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void on_pgup (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void on_pgdn (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void on_help_about_menu_activate (GtkWidget *widget, gpointer data);
static void on_topic_change (GtkButton *widget, gpointer data);

#if (GTK_CHECK_VERSION(2,5,0))
static void on_expand_topic (GtkExpander *expander, gpointer data);
#endif

static void on_text_entry_activate (GtkWidget *widget, gpointer data);
static gboolean on_text_entry_key (GtkWidget *widget, GdkEventKey *key, gpointer data);

static void on_topic_entry_activate (GtkEntry *entry, gpointer user_data);

static gboolean on_resize (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
static gboolean on_vpane_move (GtkPaned *widget, GParamSpec *param_spec, gpointer data);
static gboolean on_hpane_move (GtkPaned *widget, GParamSpec *param_spec, gpointer data);

static void entry_context (GtkEntry *entry, GtkMenu *menu, gpointer user_data);

void
initialize_main_window ()
{
	GtkWidget *entry, *topicbox, *topicchange;

	gui.main_window = GNOME_APP (glade_xml_get_widget (gui.xml, "xchat-gnome"));
	g_signal_connect (G_OBJECT (gui.main_window), "delete-event", G_CALLBACK (on_main_window_close), NULL);
	/* hook up the menus */
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "connect1")), "activate", G_CALLBACK (on_irc_connect_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "downloads1")), "activate", G_CALLBACK (on_irc_file_transfers_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "quit1")), "activate", G_CALLBACK (on_irc_quit_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "cut1")), "activate", G_CALLBACK (on_edit_cut_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "copy1")), "activate", G_CALLBACK (on_edit_copy_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "paste1")), "activate", G_CALLBACK (on_edit_paste_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "clear2")), "activate", G_CALLBACK (on_edit_clear_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "settings1")), "activate", G_CALLBACK (on_edit_preferences_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "insert_color_code1")), "activate", G_CALLBACK (on_insert_color_code_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "information1")), "activate", G_CALLBACK (on_network_information_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "reconnect1")), "activate", G_CALLBACK (on_network_reconnect_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "disconnect1")), "activate", G_CALLBACK (on_network_disconnect_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "channel_list1")), "activate", G_CALLBACK (on_network_channels_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "users1")), "activate", G_CALLBACK (on_network_users_menu_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "collapse_expand")), "activate", G_CALLBACK (on_network_collapse_expand_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "save_transcript1")), "activate", G_CALLBACK (on_discussion_save_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "save_as1")), "activate", G_CALLBACK (on_discussion_save_as_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "leave1")), "activate", G_CALLBACK (on_discussion_leave_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "close1")), "activate", G_CALLBACK (on_discussion_close_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "find1")), "activate", G_CALLBACK (on_discussion_find_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "find_next1")), "activate", G_CALLBACK (on_discussion_find_next_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "clear_window1")), "activate", G_CALLBACK (on_discussion_clear_window_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "ban_list1")), "activate", G_CALLBACK (on_discussion_bans_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "previous_network1")), "activate", G_CALLBACK (on_go_previous_network_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "next_network1")), "activate", G_CALLBACK (on_go_next_network_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "previous1")), "activate", G_CALLBACK (on_go_previous_discussion_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "next1")), "activate", G_CALLBACK (on_go_next_discussion_activate), NULL);
	g_signal_connect (G_OBJECT (glade_xml_get_widget (gui.xml, "about1")), "activate", G_CALLBACK (on_help_about_menu_activate), NULL);

	entry = glade_xml_get_widget (gui.xml, "text entry");
	g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (on_text_entry_activate), NULL);
	g_signal_connect_after (G_OBJECT (entry), "key_press_event", G_CALLBACK (on_text_entry_key), NULL);
	g_signal_connect (G_OBJECT (entry), "populate-popup", G_CALLBACK (entry_context), NULL);

  /* XXX: Is this a leak?? */
	topicchange = glade_xml_get_widget (gui.xml, "topic change");
	g_signal_connect (G_OBJECT (topicchange), "clicked", G_CALLBACK (on_topic_change), NULL);
	topicbox = glade_xml_get_widget (gui.xml, "topic hbox");
	gui.topic_label = GTK_LABEL (gtk_label_new(""));
	gtk_box_pack_start (GTK_BOX (topicbox), GTK_WIDGET (gui.topic_label), TRUE, TRUE, 0);
	gtk_box_reorder_child (GTK_BOX (topicbox), GTK_WIDGET (gui.topic_label), 1);
	gtk_label_set_selectable (gui.topic_label, TRUE);
#if (GTK_CHECK_VERSION(2,5,0))
	gui.topic_expander = GTK_EXPANDER (gtk_expander_new (NULL));
	gtk_box_pack_start (GTK_BOX (topicbox), GTK_WIDGET (gui.topic_expander), FALSE, TRUE, 0);
	gtk_box_reorder_child (GTK_BOX (topicbox), GTK_WIDGET (gui.topic_expander), 0);
	gtk_expander_set_expanded (GTK_EXPANDER (gui.topic_expander), FALSE);
	g_signal_connect (G_OBJECT (gui.topic_expander), "activate", G_CALLBACK (on_expand_topic), NULL);
	gtk_label_set_ellipsize (gui.topic_label, PANGO_ELLIPSIZE_END);
#else
	gtk_label_set_line_wrap (gui.topic_label, TRUE);
#endif
	// FIXME
//	g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(on_topic_entry_activate), NULL);

	/* Hook up accelerators for pgup/pgdn */
	{
		GtkAccelGroup *pg_accel;
		GClosure *closure;

		/* Create our accelerator group */
		pg_accel = gtk_accel_group_new ();

		/* Add the two accelerators */
		closure = g_cclosure_new (G_CALLBACK (on_pgup), NULL, NULL);
		gtk_accel_group_connect (pg_accel, GDK_Page_Up, 0, GTK_ACCEL_VISIBLE, closure);
		g_closure_unref (closure);

		closure = g_cclosure_new (G_CALLBACK (on_pgdn), NULL, NULL);
		gtk_accel_group_connect (pg_accel, GDK_Page_Down, 0, GTK_ACCEL_VISIBLE, closure);
		g_closure_unref (closure);

		/* Add the accelgroup to the main window. */
		gtk_window_add_accel_group (GTK_WINDOW (gui.main_window), pg_accel);
	}

	/* Hook up accelerators for alt-#. */
	{
		GtkAccelGroup *discussion_accel;
		GClosure *closure;
		int i;
		gchar num[2] = {0,0}; /* Will be used to help determine the keyval. */

		/* Create our accelerator group. */
		discussion_accel = gtk_accel_group_new ();

		/* For alt-1 through alt-9 we just loop to set up the accelerators.
		 * We want the data passed with the callback to be one less then the
		 * button pressed (e.g. alt-1 requests the channel who's path is 0:0)
		 * so we loop from 0 <= i < 1. We use i for the user data and the ascii
		 * value of i+1 to get the keyval.
		 */
		for (i = 0; i < 9; i++)
		{
			/* num is a string containing the ascii value of i+1. */
			num[0] = i + '1';

			/* Set up our GClosure with user data set to i. */
			closure = g_cclosure_new (G_CALLBACK (on_discussion_jump_activate), GINT_TO_POINTER (i), NULL);

			/* Connect up the accelerator. */
			gtk_accel_group_connect (discussion_accel, gdk_keyval_from_name (num), GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);

			/* Delete the reference to the GClosure. */
			g_closure_unref (closure);
		}

		/* Now we set up keypress alt-0 with user data 9. */
		closure = g_cclosure_new (G_CALLBACK (on_discussion_jump_activate), GUINT_TO_POINTER (9), NULL);
		gtk_accel_group_connect (discussion_accel, gdk_keyval_from_name ("0"), GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);
		g_closure_unref (closure);

		/* alt-+ */
		closure = g_cclosure_new (G_CALLBACK (on_discussion_plus_activate), NULL, NULL);
		gtk_accel_group_connect (discussion_accel, gdk_keyval_from_name ("equal"), GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);
		g_closure_unref (closure);

		/* alt-- */
		closure = g_cclosure_new (G_CALLBACK (on_discussion_minus_activate), NULL, NULL);
		gtk_accel_group_connect (discussion_accel, gdk_keyval_from_name ("minus"), GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);
		g_closure_unref (closure);

		/* Add the accelgroup to the main window. */
		gtk_window_add_accel_group (GTK_WINDOW (gui.main_window), discussion_accel);
	}

#ifdef HAVE_GTKSPELL
#if 0
	gtkspell_new_attach(GTK_TEXT_VIEW(entry), NULL, NULL);
#endif
#endif
}

void
run_main_window ()
{
	GtkWidget *pane;
	int width, height;
	int v, h;

	width = gnome_config_get_int_with_default ("/xchat-gnome/main_window/width", 0);
	height = gnome_config_get_int_with_default ("/xchat-gnome/main_window/height", 0);
	if(width == 0 || height == 0)
		gtk_window_set_default_size (GTK_WINDOW (gui.main_window), 800, 550);
	else
		gtk_window_set_default_size (GTK_WINDOW (gui.main_window), width, height);
	v = gnome_config_get_int_with_default ("/xchat-gnome/main_window/vpane", 0);
	h = gnome_config_get_int_with_default ("/xchat-gnome/main_window/hpane", 0);
	if(h != 0) {
		GtkWidget *hpane = glade_xml_get_widget (gui.xml, "HPane");
		gtk_paned_set_position (GTK_PANED (hpane), h);
	}
	if(v != 0) {
		GtkWidget *vpane = glade_xml_get_widget (gui.xml, "VPane");
		gtk_paned_set_position (GTK_PANED (vpane), v);
	}
	g_signal_connect (G_OBJECT (gui.main_window), "configure-event", G_CALLBACK (on_resize), NULL);
	pane = glade_xml_get_widget (gui.xml, "VPane");
	g_signal_connect (G_OBJECT (pane), "notify::position", G_CALLBACK (on_vpane_move), NULL);
	pane = glade_xml_get_widget (gui.xml, "HPane");
	g_signal_connect (G_OBJECT (pane), "notify::position", G_CALLBACK (on_hpane_move), NULL);

	gtk_widget_show_all (GTK_WIDGET (gui.main_window));
}

void
rename_main_window (gchar *server, gchar *channel)
{
	gchar *new_title;

	if (server == NULL)
	{
		gtk_window_set_title (GTK_WINDOW (gui.main_window), channel);
		return;
	}
	new_title = g_strconcat (server, ": ", channel, NULL);
	gtk_window_set_title (GTK_WINDOW (gui.main_window), new_title);

	g_free (new_title);
}

static void
on_irc_connect_menu_activate (GtkWidget *widget, gpointer data)
{
	display_connection_dialog ();
}

static void
on_main_window_close (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	hide_transfers_window ();
	gui.quit = TRUE;
	xchat_exit ();
}

static void
on_irc_quit_menu_activate (GtkWidget *widget, gpointer data)
{
	hide_transfers_window ();
	gtk_widget_hide (GTK_WIDGET (gui.main_window));
	gui.quit = TRUE;
	xchat_exit ();
}

static void
on_edit_cut_menu_activate (GtkWidget *widget, gpointer data)
{
	GtkWidget *text_entry = glade_xml_get_widget(gui.xml, "text entry");
	gtk_editable_cut_clipboard(GTK_EDITABLE(text_entry));
}

static void
on_edit_copy_menu_activate (GtkWidget *widget, gpointer data)
{
	GtkWidget *text_entry = glade_xml_get_widget(gui.xml, "text entry");
	gtk_editable_copy_clipboard(GTK_EDITABLE(text_entry));
}

static void
on_edit_paste_menu_activate (GtkWidget *widget, gpointer data)
{
	GtkWidget *text_entry = glade_xml_get_widget(gui.xml, "text entry");
	gtk_editable_paste_clipboard(GTK_EDITABLE(text_entry));
}

static void
on_edit_clear_menu_activate (GtkWidget *widget, gpointer data)
{
	GtkWidget *text_entry = glade_xml_get_widget(gui.xml, "text entry");
	gtk_editable_delete_selection(GTK_EDITABLE(text_entry));
}

static void
on_edit_preferences_menu_activate (GtkWidget *widget, gpointer data)
{
	gtk_widget_show_all (GTK_WIDGET (gui.preferences_dialog));
}

static void
on_insert_color_code_menu_activate (GtkWidget *widget, gpointer data)
{
	/* FIXME: implement */
}

static void
on_network_information_menu_activate (GtkWidget *widget, gpointer data)
{
	/* FIXME: implement */
}

static void
on_network_reconnect_menu_activate (GtkWidget *widget, gpointer data)
{
	handle_command (gui.current_session, "reconnect", FALSE);
}

static void
on_network_disconnect_menu_activate (GtkWidget *widget, gpointer data)
{
	session *s = gui.current_session;
	s->server->disconnect (s, TRUE, -1);
}

static void
on_irc_file_transfers_menu_activate (GtkWidget *widget, gpointer data)
{
	show_transfers_window ();
}

static void
on_network_channels_menu_activate (GtkWidget *widget, gpointer data)
{
	create_channel_list (gui.current_session);
}

static void
on_network_users_menu_activate (GtkWidget *widget, gpointer data)
{
	/* FIXME: implement */
}

static void
on_network_collapse_expand_activate (GtkWidget *widget, gpointer data)
{
	GtkTreeView *view;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter current;

	view = GTK_TREE_VIEW (glade_xml_get_widget (gui.xml, "server channel list"));
	selection = gtk_tree_view_get_selection (view);

	if (gtk_tree_selection_get_selected (selection, &model, &current) && gui.current_session)
	{
		GtkTreePath *path;

		if (!gtk_tree_model_iter_has_child (model, &current))
		{
			GtkTreeIter parent;
			gtk_tree_model_iter_parent (model, &parent, &current);
			current = parent;
		}

		path = gtk_tree_model_get_path (model, &current);
		if (gtk_tree_view_row_expanded (view, path))
			gtk_tree_view_collapse_row (view, path);
		else
			gtk_tree_view_expand_row (view, path, FALSE);
	}
}

static void
on_discussion_save_activate (GtkWidget *widget, gpointer data)
{
	/* FIXME: implement */
}

static void
on_discussion_save_as_activate (GtkWidget *widget, gpointer data)
{
	/* FIXME: implement */
}

static void
on_discussion_leave_activate (GtkWidget *widget, gpointer data)
{
	session *s = gui.current_session;
	if (s->type == SESS_CHANNEL)
	{
		gchar *text;
		GConfClient *client;

		client = gconf_client_get_default ();
		text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
		s->server->p_part (s->server, s->channel, text);
		g_free (text);
	}
}

static void
on_discussion_close_activate (GtkWidget *widget, gpointer data)
{
	session *s = gui.current_session;
	if (s->type == SESS_CHANNEL)
	{
		gchar *text;
		GConfClient *client;

		client = gconf_client_get_default ();
		text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
		s->server->p_part (s->server, s->channel, text);
		g_free (text);
	}
	navigation_tree_remove (gui.server_tree, s);
	text_gui_remove_text_buffer (s);
}

static void
on_discussion_find_activate (GtkWidget *widget, gpointer data)
{
	/* FIXME: implement */
}

static void
on_discussion_find_next_activate (GtkWidget *widget, gpointer data)
{
	/* FIXME: implement */
}

static void
on_discussion_clear_window_activate (GtkWidget *widget, gpointer data)
{
	session *s = gui.current_session;
	clear_buffer (s);
}

static void
on_discussion_bans_activate (GtkWidget *widget, gpointer data)
{
	/* FIXME: implement */
}

static void
on_go_previous_network_activate (GtkWidget *widget, gpointer data)
{
	navigation_tree_select_prev_network (gui.server_tree);
}

static void
on_go_next_network_activate(GtkWidget *widget, gpointer data)
{
	navigation_tree_select_next_network (gui.server_tree);
}

static void
on_go_previous_discussion_activate (GtkWidget *widget, gpointer data)
{
	navigation_tree_select_prev_channel (gui.server_tree, TRUE);
}

static void
on_go_next_discussion_activate (GtkWidget *widget, gpointer data)
{
	navigation_tree_select_next_channel (gui.server_tree, TRUE);
}

static void
on_discussion_jump_activate (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data)
{
	navigation_tree_select_nth_channel (gui.server_tree, GPOINTER_TO_INT (data));
}

static void
on_discussion_plus_activate(GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data)
{
	navigation_tree_select_next_channel (gui.server_tree, FALSE);
}

static void
on_discussion_minus_activate (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data)
{
	navigation_tree_select_prev_channel (gui.server_tree, FALSE);
}

static void
on_pgup (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data)
{
	GtkWidget *scrollbar;
	GtkAdjustment *adj;
	int value, end;

	scrollbar = glade_xml_get_widget (gui.xml, "text area scrollbar");
	adj = GTK_RANGE (scrollbar)->adjustment;
	end = adj->upper - adj->lower - adj->page_size;
	value = adj->value - (adj->page_size - 1);
	if (value < 0)
		value = 0;
	if (value > end)
		value = end;
	gtk_adjustment_set_value (adj, value);
}

static void
on_pgdn (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data)
{
	GtkWidget *scrollbar;
	GtkAdjustment *adj;
	int value, end;

	scrollbar = glade_xml_get_widget (gui.xml, "text area scrollbar");
	adj = GTK_RANGE (scrollbar)->adjustment;
	end = adj->upper - adj->lower - adj->page_size;
	value = adj->value + (adj->page_size - 1);
	if (value < 0)
		value = 0;
	if (value > end)
		value = end;
	gtk_adjustment_set_value (adj, value);
}

static void
on_help_about_menu_activate (GtkWidget *widget, gpointer data)
{
	show_about_dialog ();
}

static void
on_text_entry_activate (GtkWidget *widget, gpointer data)
{
	char *entry_text = g_strdup (gtk_entry_get_text (GTK_ENTRY (widget)));
	gtk_entry_set_text (GTK_ENTRY (widget), "");
	handle_multiline (gui.current_session, (char *) entry_text, TRUE, FALSE);
	g_free (entry_text);
}

static void
on_topic_entry_activate (GtkEntry *entry, gpointer user_data)
{
	char *text = entry->text;
	session *sess = gui.current_session;
	GtkWidget *text_entry = glade_xml_get_widget(gui.xml, "text entry");

	if (sess->channel[0] && sess->server->connected)
	{
		if (text[0] == 0)
			text = NULL;
		sess->server->p_topic(sess->server, sess->channel, text);
	}
	else
		gtk_entry_set_text(entry, "");
	gtk_widget_grab_focus(text_entry);
}

static void
history_key_down (GtkEntry *entry)
{
	char *new_line;
	new_line = history_down (&gui.current_session->history);
	if (new_line)
	{
		gtk_entry_set_text (entry, new_line);
		gtk_editable_set_position (GTK_EDITABLE (entry), -1);
	}
}

static void
history_key_up (GtkEntry *entry)
{
	char *new_line;
	new_line = history_up (&gui.current_session->history, (char *)entry->text);
	if (new_line)
	{
		gtk_entry_set_text (entry, new_line);
		gtk_editable_set_position (GTK_EDITABLE (entry), -1);
	}
}

static gboolean
tab_complete_nickname (GtkEntry *entry, int start)
{
	GCompletion *completion;
	int cursor, length;
	char *text;
	GList *list;
	char *printtext, *npt;
	session_gui *tgui;
	GList *options;
	gchar *new_prefix;
	gchar *prefix;

	completion = userlist_get_completion (u, gui.current_session);
	g_completion_set_compare (completion, (GCompletionStrncmpFunc) strncasecmp);
	text = g_strdup (gtk_entry_get_text (entry));
	length = strlen (text);
	cursor = gtk_editable_get_position (GTK_EDITABLE (entry));

	prefix = g_new0 (char, cursor - start + 1);
	strncpy (prefix, &text[start], cursor - start);
	options = g_completion_complete (completion, prefix, &new_prefix);

	if (g_list_length (options) == 0)
	{
		/* no matches */
		g_free (text);
		g_free (prefix);
		return TRUE;
	}

	if (g_list_length (options) == 1)
	{
		int pos;

		/* one match */
		if (length - cursor == 0)
		{
			/* at the end of the entry, just insert */

			if (start != 0)
			{
				text[start] = '\0';
				npt = g_strdup_printf ("%s%s", text, (char *) options->data);
				pos = strlen ((char *) options->data) + start;
			}
			else
			{
				npt = g_strdup_printf ("%s: ", (char *) options->data);
				pos = strlen ((char *) options->data) + 2;
			}
		}
		else
		{
			/* somewhere in the middle of the entry */

			if (start != 0)
			{
				text[start] = '\0';
				npt = g_strdup_printf ("%s%s%s", text, (char *) options->data, &text[cursor]);
				pos = strlen ((char *) options->data) + start;
			}
			else
			{
				npt = g_strdup_printf ("%s: %s", (char *) options->data, &text[cursor]);
				pos = strlen ((char *) options->data) + 2;
			}
		}
		gtk_entry_set_text (entry, npt);
		gtk_editable_set_position (GTK_EDITABLE (entry), pos);
		g_free (npt);
		g_free (text);
		g_free (prefix);
		return TRUE;
	}
	else
	{
		/* more than one match - print a list of options
		 * to the window and update the prefix
		 */
		list = options;
		printtext = g_strdup ((char *) list->data);
		for (list = list->next; list; list = list->next)
		{
			npt = g_strdup_printf ("%s %s", printtext, (char *) list->data);
			g_free (printtext);
			printtext = npt;
		}
		tgui = (session_gui *) gui.current_session->gui;
		text_gui_print (tgui->buffer, printtext, TRUE);
		g_free (printtext);
		if (strcasecmp (prefix, new_prefix) != 0)
		{
			/* insert the new prefix into the entry */
			text[start] = '\0';
			npt = g_strdup_printf ("%s%s%s", text, new_prefix, &text[cursor]);
			gtk_entry_set_text (entry, npt);
			g_free (npt);
			gtk_editable_set_position (GTK_EDITABLE(entry), start + strlen(new_prefix));
		}
		g_free(text);
		g_free (prefix);
		return TRUE;
	}
}

static gboolean
tab_complete (GtkEntry *entry)
{
	const char *text;
	int start, cursor_pos;

	text = gtk_entry_get_text (entry);
	cursor_pos = gtk_editable_get_position (GTK_EDITABLE (entry));

	if (cursor_pos == 0)
		return TRUE;

	/* search backwards to find /, #, ' ' or start */
	for (start = cursor_pos - 1; start >= 0; --start) {
		/* check if we can match a channel */
#if 0 /* <- (fails for all non-constant values of zero) */
		if(text[start] == '#') {
			if(start == 0 || text[start - 1] == ' ') {
				tab_complete_channel(entry, start);
				return;
			}
		}
#endif

		/* check if we can match a command */
		if (start == 0 && text[0] == '/')
		{
		    /* TODO: Something? */
		}

		/* check if we can match a nickname */
		if (start == 0 || text[start] == ' ')
		{
			return tab_complete_nickname (entry, start == 0 ? start : start + 1);
		}
	}
	return TRUE;
}

static gboolean
on_text_entry_key (GtkWidget *widget, GdkEventKey *key, gpointer data)
{
	if (key->keyval == GDK_Down)
	{
		history_key_down (GTK_ENTRY (widget));
		return TRUE;
	}
	if (key->keyval == GDK_Up)
	{
		history_key_up (GTK_ENTRY (widget));
		return TRUE;
	}
	if (key->keyval == GDK_Tab)
	{
		return tab_complete (GTK_ENTRY (widget));
	}
	return FALSE;
}

static gboolean
on_resize (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	gnome_config_set_int ("/xchat-gnome/main_window/width", event->width);
	gnome_config_set_int ("/xchat-gnome/main_window/height", event->height);
	gnome_config_sync ();
	return FALSE;
}

static gboolean
on_vpane_move (GtkPaned *widget, GParamSpec *param_spec, gpointer data)
{
	int pos = gtk_paned_get_position (widget);
	gnome_config_set_int ("/xchat-gnome/main_window/vpane", pos);
	gnome_config_sync ();
	return FALSE;
}

static gboolean
on_hpane_move (GtkPaned *widget, GParamSpec *param_spec, gpointer data)
{
	int pos = gtk_paned_get_position (widget);
	gnome_config_set_int ("/xchat-gnome/main_window/hpane", pos);
	gnome_config_sync ();
	return FALSE;
}

static void
on_topic_change (GtkButton *widget, gpointer data)
{
	GladeXML *xml;
	GtkWidget *dialog;
	GtkWidget *entry;
	gint response;
	GtkTextBuffer *buffer;
	gchar *title;

	xml = glade_xml_new ("topic-change.glade", NULL, NULL);
	if (!xml)
		xml = glade_xml_new (XCHATSHAREDIR "/topic-change.glade", NULL, NULL);
	if (!xml)
		/* FIXME - should do some error handling here */
		return;

	dialog = glade_xml_get_widget (xml, "topic change");
	entry = glade_xml_get_widget (xml, "topic entry box");

	title = g_strdup_printf ("Changing topic for %s", gui.current_session->channel);
	gtk_window_set_title (GTK_WINDOW (dialog), title);
	g_free (title);

	buffer = gtk_text_buffer_new (NULL);
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (entry), buffer);
	gtk_text_buffer_set_text (buffer, gui.current_session->topic, -1);

	response = gtk_dialog_run (GTK_DIALOG (dialog));
	g_print ("got response %d\n", response);

	if (response == GTK_RESPONSE_OK)
	{
		GtkTextIter start;
		GtkTextIter end;
		gchar *newtopic;

		gtk_text_buffer_get_start_iter (buffer, &start);
		gtk_text_buffer_get_end_iter (buffer, &end);
		newtopic = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
		/* FIXME - set new topic */
		g_free (newtopic);
	}

	gtk_widget_destroy (dialog);
	g_object_unref (xml);
}

void
set_statusbar ()
{
	GtkWidget *appbar;
	session_gui *tgui;
	char *text;

	if (gui.current_session == NULL)
		return;
	appbar = glade_xml_get_widget (gui.xml, "appbar1");
	tgui = (session_gui *) gui.current_session->gui;
	text = g_strdup_printf ("%s%s%s", tgui->lag_text ? tgui->lag_text : "", (tgui->queue_text && tgui->lag_text) ? ", " : "", tgui->queue_text ? tgui->queue_text : "");
	gnome_appbar_set_status (GNOME_APPBAR (appbar), text);
	g_free (text);
}

static GtkWidget*
get_color_icon (int c, GtkStyle *s)
{
	GtkWidget *image;
	GdkPixmap *pixmap;
	GdkGC *color;

	pixmap = gdk_pixmap_new (NULL, 16, 16, 24);

	color = gdk_gc_new (GDK_DRAWABLE (pixmap));
	gdk_gc_set_foreground (color, &s->dark[GTK_STATE_NORMAL]);
	gdk_draw_rectangle (GDK_DRAWABLE (pixmap), color, TRUE, 0, 0, 16, 16);
	gdk_gc_set_foreground (color, &colors[c]);
	gdk_draw_rectangle (GDK_DRAWABLE (pixmap), color, TRUE, 1, 1, 14, 14);

	image = gtk_image_new_from_pixmap (pixmap, NULL);
	gdk_pixmap_unref (pixmap);
	return image;
}

static void
color_code_activate (GtkMenuItem *item, gpointer data)
{
	int color = (int) data;
	GtkWidget *entry = glade_xml_get_widget (gui.xml, "text entry");
	char *code = g_strdup_printf ("%%C%d", color);
	int position = gtk_editable_get_position (GTK_EDITABLE (entry));
	gtk_editable_insert_text (GTK_EDITABLE (entry), code, strlen (code), &position);
	gtk_editable_set_position (GTK_EDITABLE (entry), position + strlen (code));
	g_free (code);
}

static void
entry_context (GtkEntry *entry, GtkMenu *menu, gpointer user_data)
{
	GtkWidget *item;
	GtkWidget *submenu;

	item = gtk_menu_item_new_with_mnemonic ("I_nsert Color Code");
	gtk_widget_show (item);

	submenu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), submenu);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

	item = gtk_image_menu_item_new_with_label ("Black");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (1, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 1);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Dark Blue");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (2, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 2);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Dark Green");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (3, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 3);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Red");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (4, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 4);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Brown");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (5, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 5);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Purple");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (6, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 6);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Orange");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (7, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 7);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Yellow");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (8, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 8);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Light Green");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (9, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 9);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Aqua");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (10, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 10);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Light Blue");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (11, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 11);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Blue");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (12, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 12);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Violet");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (13, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 13);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Grey");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (14, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 14);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("Light Grey");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (15, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 15);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label ("White");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (0, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 0);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);

	gtk_widget_show_all (submenu);
}

#if (GTK_CHECK_VERSION(2,5,0))
static void
on_expand_topic (GtkExpander *expander, gpointer data)
{
  if (gtk_expander_get_expanded (gui.topic_expander))
  {
    gtk_label_set_ellipsize(gui.topic_label, PANGO_ELLIPSIZE_END);
    gtk_label_set_line_wrap (gui.topic_label, FALSE);
  }
  else
  {
    gtk_label_set_ellipsize(gui.topic_label, PANGO_ELLIPSIZE_NONE);
    gtk_label_set_line_wrap (gui.topic_label, TRUE);
  }
}
#endif
