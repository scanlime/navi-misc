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
#include "userlist.h"
#include "../common/xchatc.h"
#include "../common/outbound.h"
#include "gui.h"
#include "channel_list.h"
#include "preferences.h"
#include "navigation_tree.h"

#ifdef HAVE_GTKSPELL
#include <gtkspell/gtkspell.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void on_main_window_close(GtkWidget *widget, GdkEvent *event, gpointer data);
void on_irc_quit_menu_activate(GtkWidget *widget, gpointer data);
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
void on_network_file_transfers_menu_activate(GtkWidget *widget, gpointer data);
void on_network_channels_menu_activate(GtkWidget *widget, gpointer data);
void on_network_users_menu_activate(GtkWidget *widget, gpointer data);
void on_discussion_save_activate(GtkWidget *widget, gpointer data);
void on_discussion_save_as_activate(GtkWidget *widget, gpointer data);
void on_discussion_leave_activate(GtkWidget *widget, gpointer data);
void on_discussion_find_activate(GtkWidget *widget, gpointer data);
void on_discussion_find_next_activate(GtkWidget *widget, gpointer data);
void on_discussion_clear_window_activate(GtkWidget *widget, gpointer data);
void on_discussion_bans_activate(GtkWidget *widget, gpointer data);
void on_go_previous_network_activate(GtkWidget *widget, gpointer data);
void on_go_next_network_activate(GtkWidget *widget, gpointer data);
void on_go_previous_discussion_activate(GtkWidget *widget, gpointer data);
void on_go_next_discussion_activate(GtkWidget *widget, gpointer data);
void on_help_about_menu_activate(GtkWidget *widget, gpointer data);

void on_text_entry_activate(GtkWidget *widget, gpointer data);
gboolean on_text_entry_key(GtkWidget *widget, GdkEventKey *key, gpointer data);

gboolean on_resize(GtkWidget *widget, GdkEventConfigure *event, gpointer data);

void initialize_main_window() {
	GtkWidget *entry;

	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	g_signal_connect(G_OBJECT(gui.main_window), "delete-event", G_CALLBACK(on_main_window_close), NULL);
	/* hook up the menus */
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "connect1")), "activate", G_CALLBACK(on_irc_connect_menu_activate), NULL);
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
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "downloads1")), "activate", G_CALLBACK(on_network_file_transfers_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "channel_list1")), "activate", G_CALLBACK(on_network_channels_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "users1")), "activate", G_CALLBACK(on_network_users_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "save_transcript1")), "activate", G_CALLBACK(on_discussion_save_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "save_as1")), "activate", G_CALLBACK(on_discussion_save_as_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "leave1")), "activate", G_CALLBACK(on_discussion_leave_activate), NULL);
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

#ifdef HAVE_GTKSPELL
#if 0
	gtkspell_new_attach(GTK_TEXT_VIEW(entry), NULL, NULL);
#endif
#endif
}

void run_main_window() {
	int width, height;

	preferences_get_main_window_size(&width, &height);
	gtk_widget_show_all(GTK_WIDGET(gui.main_window));
	if(!(width == 0 || height == 0))
		gtk_window_set_default_size(GTK_WINDOW(gui.main_window), width, height);
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
	gui.quit = TRUE;
	xchat_exit();
}

void on_irc_quit_menu_activate(GtkWidget *widget, gpointer data) {
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
	/* FIXME: implement */
}

void on_network_disconnect_menu_activate(GtkWidget *widget, gpointer data) {
	session *s = gui.current_session;
	s->server->disconnect(s, TRUE, -1);
}

void on_network_file_transfers_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_network_channels_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_network_users_menu_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
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
	navigation_tree_remove(s);
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
	/* FIXME: implement */
}

void on_go_next_network_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_go_previous_discussion_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_go_next_discussion_activate(GtkWidget *widget, gpointer data) {
	/* FIXME: implement */
}

void on_help_about_menu_activate(GtkWidget *widget, gpointer data) {
	show_about_dialog();
}

void on_text_entry_activate(GtkWidget *widget, gpointer data) {
	char *entry_text = gtk_entry_get_text(GTK_ENTRY(widget));
	handle_multiline(gui.current_session, entry_text, TRUE, FALSE);
	gtk_entry_set_text(GTK_ENTRY(widget), "");
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

static void tab_complete_nickname(GtkEntry *entry, int start) {
	GCompletion *completion;
	int cursor, length;
	const char *text;
	GList *list;


	completion = userlist_get_nick_completion();
	g_completion_set_compare(completion, (GCompletionStrncmpFunc) strncasecmp);
	text = gtk_entry_get_text(entry);
	length = strlen(text);
	cursor = gtk_editable_get_position(GTK_EDITABLE(entry));
	g_print("trying to complete!\nstart  = %d\ncursor = %d\n\n", start, cursor);
	if(length - cursor != 1) {
		/* we're at the end of the entry, just complete from start to cursor*/
		GList *options;
		gchar *new_prefix;
		gchar prefix[cursor - start];

		strncpy(prefix, text, cursor - start);
		prefix[cursor - start] = '\0';

		options = g_completion_complete(completion, prefix, &new_prefix);
		g_print("new prefix=\"%s\"\navailable items:\n", new_prefix);
		for(list = options; list; list = list->next) {
			g_print("  %s\n", list->data);
		}
		g_list_free(options);
		g_print("\n\n");
	}
}

static void tab_complete(GtkEntry *entry) {
	const char *text;
	int start, cursor_pos;

	text = gtk_entry_get_text(entry);
	cursor_pos = gtk_editable_get_position(GTK_EDITABLE(entry));

	if(cursor_pos == 0)
		return;

	/* search backwards to find /, #, ' ' or start */
	for(start = cursor_pos; start >= 0; --start) {
		/* check if we can match a channel */
#if 0
		if(text[start] == '#') {
			if(start == 0 || text[start - 1] == ' ') {
				tab_complete_channel(entry, start);
				return;
			}
		}
#endif

		/* check if we can match a command */
		if(start == 0 && text[0] == '/') {
		}

		/* check if we can match a nickname */
		if(start == 0 || text[start] == ' ') {
			tab_complete_nickname(entry, start == 0 ? start : start + 1);
			return;
		}
	}
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
		tab_complete(GTK_ENTRY(widget));
		return TRUE;
	}
	return FALSE;
}

gboolean on_resize(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
	preferences_set_main_window_size(event->width, event->height);
	return FALSE;
}
