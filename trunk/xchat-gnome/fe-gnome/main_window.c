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

#ifdef HAVE_GTKSPELL
#include <gtkspell/gtkspell.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void on_main_window_close(GtkWidget *widget, GdkEvent *event, gpointer data);
void on_irc_quit_menu_activate(GtkWidget *widget, gpointer data);
void on_irc_file_transfers_menu_activate(GtkWidget *widget, gpointer data);
void on_irc_connect_menu_activate(GtkWidget *widget, gpointer data);
void on_edit_cut_menu_activate(GtkWidget *widget, gpointer data);
void on_edit_copy_menu_activate(GtkWidget *widget, gpointer data);
void on_edit_paste_menu_activate(GtkWidget *widget, gpointer data);
void on_edit_clear_menu_activate(GtkWidget *widget, gpointer data);
void on_edit_preferences_menu_activate(GtkWidget *widget, gpointer data);
void on_insert_color_code_menu_activate(GtkWidget *widget, gpointer data);
void on_network_information_menu_activate(GtkWidget *widget, gpointer data);
void on_network_reconnect_menu_activate(GtkWidget *widget, gpointer data);
void on_network_disconnect_menu_activate(GtkWidget *widget, gpointer data);
void on_network_channels_menu_activate(GtkWidget *widget, gpointer data);
void on_network_users_menu_activate(GtkWidget *widget, gpointer data);
void on_network_collapse_expand_activate(GtkWidget *widget, gpointer data);
void on_discussion_save_activate(GtkWidget *widget, gpointer data);
void on_discussion_save_as_activate(GtkWidget *widget, gpointer data);
void on_discussion_leave_activate(GtkWidget *widget, gpointer data);
void on_discussion_close_activate(GtkWidget *widget, gpointer data);
void on_discussion_find_activate(GtkWidget *widget, gpointer data);
void on_discussion_find_next_activate(GtkWidget *widget, gpointer data);
void on_discussion_clear_window_activate(GtkWidget *widget, gpointer data);
void on_discussion_bans_activate(GtkWidget *widget, gpointer data);
void on_go_previous_network_activate(GtkWidget *widget, gpointer data);
void on_go_next_network_activate(GtkWidget *widget, gpointer data);
void on_go_previous_discussion_activate(GtkWidget *widget, gpointer data);
void on_go_next_discussion_activate(GtkWidget *widget, gpointer data);
void on_discussion_jump_activate(GtkAccelGroup *accelgroup, GObject *arg1, guint arg2,GdkModifierType arg3, gpointer data);
void on_help_about_menu_activate(GtkWidget *widget, gpointer data);

void on_text_entry_activate(GtkWidget *widget, gpointer data);
gboolean on_text_entry_key(GtkWidget *widget, GdkEventKey *key, gpointer data);

void on_topic_entry_activate(GtkEntry *entry, gpointer user_data);

gboolean on_resize(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
gboolean on_vpane_move(GtkPaned *widget, GParamSpec *param_spec, gpointer data);
gboolean on_hpane_move(GtkPaned *widget, GParamSpec *param_spec, gpointer data);

static void entry_context(GtkEntry *entry, GtkMenu *menu, gpointer user_data);

void initialize_main_window() {
	GtkWidget *entry, *pane;

	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	g_signal_connect(G_OBJECT(gui.main_window), "delete-event", G_CALLBACK(on_main_window_close), NULL);
	/* hook up the menus */
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "connect1")), "activate", G_CALLBACK(on_irc_connect_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "downloads1")), "activate", G_CALLBACK(on_irc_file_transfers_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "quit1")), "activate", G_CALLBACK(on_irc_quit_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "cut1")), "activate", G_CALLBACK(on_edit_cut_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "copy1")), "activate", G_CALLBACK(on_edit_copy_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "paste1")), "activate", G_CALLBACK(on_edit_paste_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "clear2")), "activate", G_CALLBACK(on_edit_clear_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "settings1")), "activate", G_CALLBACK(on_edit_preferences_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "insert_color_code1")), "activate", G_CALLBACK(on_insert_color_code_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "information1")), "activate", G_CALLBACK(on_network_information_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "reconnect1")), "activate", G_CALLBACK(on_network_reconnect_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "disconnect1")), "activate", G_CALLBACK(on_network_disconnect_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "channel_list1")), "activate", G_CALLBACK(on_network_channels_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "users1")), "activate", G_CALLBACK(on_network_users_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "collapse_expand")), "activate", G_CALLBACK(on_network_collapse_expand_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "save_transcript1")), "activate", G_CALLBACK(on_discussion_save_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "save_as1")), "activate", G_CALLBACK(on_discussion_save_as_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "leave1")), "activate", G_CALLBACK(on_discussion_leave_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "close1")), "activate", G_CALLBACK(on_discussion_close_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "find1")), "activate", G_CALLBACK(on_discussion_find_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "find_next1")), "activate", G_CALLBACK(on_discussion_find_next_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "clear_window1")), "activate", G_CALLBACK(on_discussion_clear_window_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "ban_list1")), "activate", G_CALLBACK(on_discussion_bans_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "previous_network1")), "activate", G_CALLBACK(on_go_previous_network_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "next_network1")), "activate", G_CALLBACK(on_go_next_network_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "previous1")), "activate", G_CALLBACK(on_go_previous_discussion_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "next1")), "activate", G_CALLBACK(on_go_next_discussion_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "about1")), "activate", G_CALLBACK(on_help_about_menu_activate), NULL);

	entry = glade_xml_get_widget(gui.xml, "text entry");
	g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(on_text_entry_activate), NULL);
	g_signal_connect_after(G_OBJECT(entry), "key_press_event", G_CALLBACK(on_text_entry_key), NULL);
	g_signal_connect(G_OBJECT(entry), "populate-popup", G_CALLBACK(entry_context), NULL);

  /* XXX: Is this a leak?? */
  entry = glade_xml_get_widget(gui.xml, "topic entry");
  g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(on_topic_entry_activate), NULL);

	pane = glade_xml_get_widget(gui.xml, "VPane");
	g_signal_connect(G_OBJECT(pane), "notify::position", G_CALLBACK(on_vpane_move), NULL);
	pane = glade_xml_get_widget(gui.xml, "HPane");
	g_signal_connect(G_OBJECT(pane), "notify::position", G_CALLBACK(on_hpane_move), NULL);

	/* Hook up accelerators for alt-#. */
	{
		GtkAccelGroup *discussion_accel;
		GClosure *closure;
		int i;
		gchar num[2] = {0,0}; /* Will be used to help determine the keyval. */

		/* Create our accelerator group. */
		discussion_accel = gtk_accel_group_new();

		/* For alt-1 through alt-9 we just loop to set up the accelerators.
		 * We want the data passed with the callback to be one less then the
		 * button pressed (e.g. alt-1 requests the channel who's path is 0:0)
		 * so we loop from 0 <= i < 1. We use i for the user data and the ascii
		 * value of i+1 to get the keyval.
		 */ 
		for(i = 0; i < 9; i++) {
			/* num is a string containing the ascii value of i+1. */
			num[0] = i + '1';

			/* Set up our GClosure with user data set to i. */
			closure = g_cclosure_new(G_CALLBACK(on_discussion_jump_activate),
					GINT_TO_POINTER(i), NULL);

			/* Connect up the accelerator. */
			gtk_accel_group_connect(discussion_accel, gdk_keyval_from_name(num),
					GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);

			/* Delete the reference to the GClosure. */
			g_closure_unref(closure);
		}

		/* Now we set up keypress alt-0 with user data 9. */
		closure = g_cclosure_new(G_CALLBACK(on_discussion_jump_activate),
				GUINT_TO_POINTER(9), NULL);
		gtk_accel_group_connect(discussion_accel, gdk_keyval_from_name("0"),
				GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);
		g_closure_unref(closure);

		/* Add the accelgroup to the main window. */
		gtk_window_add_accel_group(gui.main_window, discussion_accel);
	}

#ifdef HAVE_GTKSPELL
#if 0
	gtkspell_new_attach(GTK_TEXT_VIEW(entry), NULL, NULL);
#endif
#endif
}

void run_main_window() {
	int width, height;
	int v, h;

	preferences_get_main_window_size(&width, &height);
	gtk_widget_show_all(GTK_WIDGET(gui.main_window));
	if(!(width == 0 || height == 0))
		gtk_window_set_default_size(GTK_WINDOW(gui.main_window), width, height);
	preferences_get_main_window_positions(&v, &h);
	if(h != 0) {
		GtkWidget *hpane = glade_xml_get_widget(gui.xml, "HPane");
		gtk_paned_set_position(GTK_PANED(hpane), h);
	}
	if(v != 0) {
		GtkWidget *vpane = glade_xml_get_widget(gui.xml, "VPane");
		gtk_paned_set_position(GTK_PANED(vpane), v);
	}
	g_signal_connect(G_OBJECT(gui.main_window), "configure-event", G_CALLBACK(on_resize), NULL);
}

void rename_main_window(gchar *server, gchar *channel) {
	gchar *new_title;

	new_title = g_strconcat (server, ": ", channel, NULL);
	gtk_window_set_title(GTK_WINDOW(gui.main_window), new_title);

	g_free(new_title);
}

void on_irc_connect_menu_activate(GtkWidget *widget, gpointer data) {
	display_connection_dialog();
}

void on_main_window_close(GtkWidget *widget, GdkEvent *event, gpointer data) {
	hide_transfers_window();
	gui.quit = TRUE;
	xchat_exit();
}

void on_irc_quit_menu_activate(GtkWidget *widget, gpointer data) {
	hide_transfers_window();
	gui.quit = TRUE;
	xchat_exit();
}

void on_edit_cut_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_edit_copy_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_edit_paste_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_edit_clear_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_edit_preferences_menu_activate(GtkWidget *widget, gpointer data) {
	gtk_widget_show_all(GTK_WIDGET(gui.preferences_dialog));
}

void on_insert_color_code_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_network_information_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_network_reconnect_menu_activate(GtkWidget *widget, gpointer data) {
	handle_command(gui.current_session, "reconnect", FALSE);
}

void on_network_disconnect_menu_activate(GtkWidget *widget, gpointer data) {
	session *s = gui.current_session;
	s->server->disconnect(s, TRUE, -1);
}

void on_irc_file_transfers_menu_activate(GtkWidget *widget, gpointer data) {
	show_transfers_window();
}

void on_network_channels_menu_activate(GtkWidget *widget, gpointer data) {
	create_channel_list(gui.current_session);
}

void on_network_users_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_network_collapse_expand_activate(GtkWidget *widget, gpointer data) {
	GtkTreeView *view;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter current;

	view = GTK_TREE_VIEW(glade_xml_get_widget(gui.xml, "server channel list"));
	selection = gtk_tree_view_get_selection(view);

	if(gtk_tree_selection_get_selected(selection, &model, &current) &&
			gui.current_session) {
		GtkTreePath *path;

		if(!gtk_tree_model_iter_has_child(model, &current)) {
			GtkTreeIter parent;
			gtk_tree_model_iter_parent(model, &parent, &current);
			current = parent;
		}

		path = gtk_tree_model_get_path(model, &current);
		if(gtk_tree_view_row_expanded(view, path))
			gtk_tree_view_collapse_row(view, path);
		else
			gtk_tree_view_expand_row(view, path, FALSE);
	}
}

void on_discussion_save_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_discussion_save_as_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_discussion_leave_activate(GtkWidget *widget, gpointer data) {
	session *s = gui.current_session;
	if(s->type == SESS_CHANNEL) {
		s->server->p_part(s->server, s->channel, "ex-chat");
		/* FIXME: part reason */
	}
}

void on_discussion_close_activate(GtkWidget *widget, gpointer data) {
	session *s = gui.current_session;
	if(s->type == SESS_CHANNEL) {
		s->server->p_part(s->server, s->channel, "ex-chat");
		/* FIXME: part reason */
	}
	navigation_tree_remove(gui.server_tree, s);
	text_gui_remove_text_buffer(s);
}

void on_discussion_find_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_discussion_find_next_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_discussion_clear_window_activate(GtkWidget *widget, gpointer data) {
	session *s = gui.current_session;
	clear_buffer(s);
}

void on_discussion_bans_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_go_previous_network_activate(GtkWidget *widget, gpointer data) {
	navigation_tree_select_prev_network(gui.server_tree);
}

void on_go_next_network_activate(GtkWidget *widget, gpointer data) {
  navigation_tree_select_next_network(gui.server_tree);
}

void on_go_previous_discussion_activate(GtkWidget *widget, gpointer data) {
  navigation_tree_select_prev_channel(gui.server_tree);
}

void on_go_next_discussion_activate(GtkWidget *widget, gpointer data) {
  navigation_tree_select_next_channel(gui.server_tree);
}

void on_discussion_jump_activate(GtkAccelGroup *accelgroup, GObject *arg1,
		guint arg2,GdkModifierType arg3, gpointer data) {
  navigation_tree_select_nth_channel(gui.server_tree, GPOINTER_TO_INT(data));
}

void on_help_about_menu_activate(GtkWidget *widget, gpointer data) {
	show_about_dialog();
}

void on_text_entry_activate(GtkWidget *widget, gpointer data) {
	char *entry_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));
	gtk_entry_set_text(GTK_ENTRY(widget), "");
	handle_multiline(gui.current_session, (const char *) entry_text, TRUE, FALSE);
	g_free(entry_text);
}

void on_topic_entry_activate (GtkEntry *entry, gpointer user_data)
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
static void history_key_down(GtkEntry *entry) {
	char *new_line;
	new_line = history_down(&(gui.current_session->history));
	if(new_line) {
		gtk_entry_set_text(entry, new_line);
		gtk_editable_set_position(GTK_EDITABLE(entry), -1);
	}
}

static void history_key_up(GtkEntry *entry) {
	char *new_line;
	new_line = history_up(&(gui.current_session->history), (char *)entry->text);
	if(new_line) {
		gtk_entry_set_text(entry, new_line);
		gtk_editable_set_position(GTK_EDITABLE(entry), -1);
	}
}

static gboolean tab_complete_nickname(GtkEntry *entry, int start) {
	GCompletion *completion;
	int cursor, length;
	char *text;
	GList *list;
	char *printtext, *npt;
	session_gui *tgui;

	completion = userlist_get_completion(u, gui.current_session);
	g_completion_set_compare(completion, (GCompletionStrncmpFunc) strncasecmp);
	text = g_strdup(gtk_entry_get_text(entry));
	length = strlen(text);
	cursor = gtk_editable_get_position(GTK_EDITABLE(entry));

	if(length - cursor != 1) {
		/* we're at the end of the entry, just complete from start to cursor*/
		GList *options;
		gchar *new_prefix;
		gchar prefix[cursor - start];

		strncpy(prefix, &text[start], cursor - start);
		prefix[cursor - start] = '\0';

		options = g_completion_complete(completion, prefix, &new_prefix);

		/* No matches */
		if(g_list_length(options) == 0) {
			g_free(text);
			return TRUE;
		}

		/* One match */
		if(g_list_length(options) == 1) {
			int pos;

			text[start] = '\0';
			if(start != 0) {
				npt = g_strdup_printf("%s%s%s", text, (char *) options->data, &text[start]);
				pos = strlen((char *) options->data) + start;
			} else {
				npt = g_strdup_printf("%s: %s", (char *) options->data, text);
				pos = strlen((char *) options->data) + 2;
			}
			gtk_entry_set_text(entry, npt);
			gtk_editable_set_position(GTK_EDITABLE(entry), pos);
			g_free(npt);
			g_free(text);
			return TRUE;
		}

		/* we have more than one match - print a list of options to the window */
		list = options;
		printtext = g_strdup((char *) list->data);
		for(list = list->next; list; list = list->next) {
			npt = g_strdup_printf("%s %s", printtext, (char *) list->data);
			g_free(printtext);
			printtext = npt;
		}
		tgui = (session_gui *) gui.current_session->gui;
		text_gui_print(tgui->buffer, printtext, TRUE);
		g_free(printtext);
		if(strcasecmp(prefix, new_prefix) != 0) {
			/* insert the new prefix into the entry */
			text[start] = '\0';
			npt = g_strdup_printf("%s%s%s", text, new_prefix, &text[cursor]);
			gtk_entry_set_text(entry, npt);
			g_free(npt);
			gtk_editable_set_position(GTK_EDITABLE(entry), start + strlen(new_prefix));
		}
		g_free(text);
		return TRUE;
	}
}

static gboolean tab_complete(GtkEntry *entry) {
	const char *text;
	int start, cursor_pos;

	text = gtk_entry_get_text(entry);
	cursor_pos = gtk_editable_get_position(GTK_EDITABLE(entry));

	if(cursor_pos == 0)
		return TRUE;

	/* search backwards to find /, #, ' ' or start */
	for(start = cursor_pos - 1; start >= 0; --start) {
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
		if(start == 0 && text[0] == '/') {
		    /* TODO: Something? */
		}

		/* check if we can match a nickname */
		if(start == 0 || text[start] == ' ') {
			return tab_complete_nickname(entry, start == 0 ? start : start + 1);
		}
	}
	return TRUE;
}

gboolean on_text_entry_key(GtkWidget *widget, GdkEventKey *key, gpointer data) {
	if(key->keyval == GDK_Down) {
		history_key_down(GTK_ENTRY(widget));
		return TRUE;
	}
	if(key->keyval == GDK_Up) {
		history_key_up(GTK_ENTRY(widget));
		return TRUE;
	}
	if(key->keyval == GDK_Tab) {
		return tab_complete(GTK_ENTRY(widget));
	}
	return FALSE;
}

gboolean on_resize(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
	preferences_set_main_window_size(event->width, event->height);
	return FALSE;
}

gboolean on_vpane_move(GtkPaned *widget, GParamSpec *param_spec, gpointer data) {
	int pos = gtk_paned_get_position(widget);
	preferences_set_main_window_v_position(pos);
	return FALSE;
}

gboolean on_hpane_move(GtkPaned *widget, GParamSpec *param_spec, gpointer data) {
	int pos = gtk_paned_get_position(widget);
	preferences_set_main_window_h_position(pos);
	return FALSE;
}

void set_statusbar() {
	GtkWidget *appbar;
	session_gui *tgui;
	char *text;

	if(gui.current_session == NULL)
		return;
	appbar = glade_xml_get_widget(gui.xml, "appbar1");
	tgui = (session_gui *) gui.current_session->gui;
	text = g_strdup_printf("%s%s%s", tgui->lag_text ? tgui->lag_text : "", (tgui->queue_text && tgui->lag_text) ? ", " : "", tgui->queue_text ? tgui->queue_text : "");
	gnome_appbar_set_status(GNOME_APPBAR(appbar), text);
	g_free(text);
}

static GtkWidget *get_color_icon(int c, GtkStyle *s) {
	GtkWidget *image;
	GdkPixmap *pixmap;
	GdkGC *color;

	pixmap = gdk_pixmap_new(NULL, 16, 16, 24);

	color = gdk_gc_new(GDK_DRAWABLE(pixmap));
	gdk_gc_set_foreground(color, &s->dark[GTK_STATE_NORMAL]);
	gdk_draw_rectangle(GDK_DRAWABLE(pixmap), color, TRUE, 0, 0, 16, 16);
	gdk_gc_set_foreground(color, &colors[c]);
	gdk_draw_rectangle(GDK_DRAWABLE(pixmap), color, TRUE, 1, 1, 14, 14);

	image = gtk_image_new_from_pixmap(pixmap, NULL);
	gdk_pixmap_unref(pixmap);
	return image;
}

static void color_code_activate(GtkMenuItem *item, gpointer data) {
	int color = (int) data;
	GtkWidget *entry = glade_xml_get_widget(gui.xml, "text entry");
	char *code = g_strdup_printf("%%C%d", color);
	int position = gtk_editable_get_position(GTK_EDITABLE(entry));
	gtk_editable_insert_text(GTK_EDITABLE(entry), code, strlen(code), &position);
	gtk_editable_set_position(GTK_EDITABLE(entry), position + strlen(code));
	g_free(code);
}

static void entry_context(GtkEntry *entry, GtkMenu *menu, gpointer user_data) {
	GtkWidget *item;
	GtkWidget *submenu;

	item = gtk_menu_item_new_with_mnemonic("I_nsert Color Code");
	gtk_widget_show(item);

	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

	item = gtk_image_menu_item_new_with_label("Black");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(1, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 1);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Dark Blue");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(2, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 2);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Dark Green");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(3, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 3);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Red");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(4, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 4);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Brown");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(5, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 5);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Purple");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(6, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 6);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Orange");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(7, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 7);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Yellow");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(8, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 8);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Light Green");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(9, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 9);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Aqua");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(10, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 10);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Light Blue");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(11, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 11);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Blue");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(12, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 12);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Violet");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(13, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 13);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Grey");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(14, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 14);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("Light Grey");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(15, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 15);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
	item = gtk_image_menu_item_new_with_label("White");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), get_color_icon(0, gtk_widget_get_style(item)));
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(color_code_activate), (gpointer) 0);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);

	gtk_widget_show_all(submenu);
}
