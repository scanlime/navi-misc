/*
 * fe-gnome.c - main frontend implementation
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

#include <gnome.h>
#include "../common/xchat.h"
#include "../common/xchatc.h"
#include "../common/servlist.h"
#include "../common/fe.h"
#include "gui.h"
#include "navigation_tree.h"
#include "textgui.h"
#include "main_window.h"
#include "userlist.h"
#include "preferences.h"

int fe_args(int argc, char *argv[]) {
	if(argc > 1) {
		if(!strcasecmp(argv[1], "--version") || !strcasecmp(argv[1], "-v")) {
			puts(VERSION);
		return 0;
		}
	}
	gnome_program_init("xchat test", "0.1", LIBGNOMEUI_MODULE, argc, argv, NULL);
	return 1;
}

void fe_init(void) {
	servlist_init();
	initialize_gui();
	if(!preferences_exist()) {
		run_setup_druid();
	} else {
		strcpy(prefs.nick1, "flobidob");
	}
	run_main_window();
	prefs.use_server_tab = TRUE;
}

void fe_main(void) {
	gtk_main();
	/* sleep for 3 seconds so any QUIT messages are not lost. The  */
	/* GUI is closed at this point, so the user doesn't even know! */
	/* FIXME: pref this? */
	sleep(3);
}

void fe_cleanup(void) {
	/* FIXME: implement */
}

void fe_exit(void) {
	xchat_exit();
	gtk_main_quit();
}

int fe_timeout_add(int interval, void *callback, void *userdata) {
	return g_timeout_add(interval, (GSourceFunc) callback, userdata);
}

void fe_timeout_remove(int tag) {
	g_source_remove(tag);
}

void fe_new_window(struct session *sess) {
	if(sess->type == SESS_SERVER)
		navigation_tree_create_new_network_entry(sess);
	else if(sess->type == SESS_CHANNEL || sess->type == SESS_DIALOG)
		navigation_tree_create_new_channel_entry(sess);
	text_gui_add_text_buffer(sess);
	create_userlist(sess);
}

void fe_new_server(struct server *serv) {
	/* FIXME: implement */
}

void fe_add_rawlog(struct server *serv, char *text, int len, int outbound) {
	/* FIXME: implement */
}

void fe_message(char *msg, int wait) {
	/* FIXME: implement */
}

int fe_input_add(int sok, int flags, void *func, void *data) {
	int tag, type = 0;
	GIOChannel *channel;

	channel = g_io_channel_unix_new(sok);

	if(flags & FIA_READ)
		type |= G_IO_IN | G_IO_HUP | G_IO_ERR;
	if(flags & FIA_WRITE)
		type |= G_IO_OUT | G_IO_ERR;
	if(flags & FIA_EX)
		type |= G_IO_PRI;

	tag = g_io_add_watch(channel, type, (GIOFunc) func, data);
	g_io_channel_unref(channel);

	return tag;
}

void fe_input_remove(int tag) {
	g_source_remove(tag);
}

void fe_idle_add(void *func, void *data) {
	g_idle_add(func, data);
}

void fe_set_topic(struct session *sess, char *topic) {
	set_gui_topic(sess, topic);
}

void fe_set_hilight(struct session *sess) {
	navigation_tree_set_hilight(sess);
}

void fe_set_tab_color(struct session *sess, int col, int flash) {
	/* FIXME: implement */
}

void fe_update_mode_buttons(struct session *sess, char mode, char sign) {
	/* FIXME: implement */
}

void fe_update_channel_key(struct session *sess) {
	/* FIXME: implement */
}

void fe_update_channel_limit(struct session *sess) {
	/* FIXME: implement */
}

int fe_is_chanwindow(struct server *serv) {
	/* FIXME: implement */
	return 0;
}

void fe_add_chan_list(struct server *serv, char *chan, char *users, char *topic) {
	/* FIXME: implement */
}

void fe_chan_list_end(struct server *serv) {
	/* FIXME: implement */
}

int fe_is_banwindow(struct session *sess) {
	/* FIXME: implement */
	return 0;
}

void fe_add_ban_list(struct session *sess, char *mask, char *who, char *when) {
	/* FIXME: implement */
}

void fe_ban_list_end(struct session *sess) {
	/* FIXME: implement */
}

void fe_notify_update(char *name) {
	/* FIXME: implement */
}

void fe_text_clear(struct session *sess) {
	clear_buffer(sess);
}

void fe_close_window(struct session *sess) {
	navigation_tree_remove(sess);
}

void fe_progressbar_start(struct session *sess) {
	/* FIXME: implement */
}

void fe_progressbar_end(struct server *serv) {
	/* FIXME: implement */
}

void fe_print_text(struct session *sess, char *text) {
	session_gui *tgui = sess->gui;
	if(tgui == NULL)
		return;
	text_gui_print(tgui->buffer, text, TRUE);
	sess->new_data = TRUE;
	navigation_tree_set_hilight(sess);
}

void fe_userlist_insert(struct session *sess, struct User *newuser, int row, int sel) {
	userlist_insert(sess, newuser, row, sel);
}

int fe_userlist_remove(struct session *sess, struct User *user) {
	return userlist_remove(sess, user);
}

void fe_userlist_rehash(struct session *sess, struct User *user) {
	userlist_change(sess, user);
}

void fe_userlist_move(struct session *sess, struct User *user, int new_row) {
	int sel = userlist_remove(sess, user);
	fe_userlist_insert(sess, user, new_row, sel);
}

void fe_userlist_numbers(struct session *sess) {
	/* FIXME: implement */
}

void fe_userlist_clear(struct session *sess) {
	session_gui *s = sess->gui;
	if(s == NULL)
		return;
	gtk_list_store_clear(GTK_LIST_STORE(s->userlist_model));
}

void fe_dcc_add(struct DCC *dcc) {
	/* FIXME: implement */
}

void fe_dcc_update(struct DCC *dcc) {
	/* FIXME: implement */
}

void fe_dcc_remove(struct DCC *dcc) {
	/* FIXME: implement */
}

int fe_dcc_open_recv_win(int passive) {
	/* FIXME: implement */
	return 0;
}

int fe_dcc_open_send_win(int passive) {
	/* FIXME: implement */
	return 0;
}

int fe_dcc_open_chat_win(int passive) {
	/* FIXME: implement */
	return 0;
}

void fe_clear_channel(struct session *sess) {
	/* FIXME: implement */
}

void fe_session_callback(struct session *sess) {
	g_print("fe_session_callback()\n");
	/* FIXME: implement */
}

void fe_server_callback(struct server *serv) {
	g_print("fe_server_callback()\n");
	/* FIXME: implement */
}

void fe_url_add(const char *text) {
	/* FIXME: implement */
}

void fe_pluginlist_update(void) {
	/* FIXME: implement */
}

void fe_buttons_update(struct session *sess) {
	/* FIXME: implement */
}

void fe_dlgbuttons_update(struct session *sess) {
	/* FIXME: implement */
}

void fe_dcc_send_filereq(struct session *sess, char *nick, int maxcps, int passive) {
	/* FIXME: implement */
}

void fe_set_channel(struct session *sess) {
	navigation_tree_set_channel_name(sess);
}

void fe_set_title(struct session *sess) {
	/* FIXME: implement */
}

void fe_set_nonchannel(struct session *sess, int state) {
	/* stub? */
}

void fe_set_nick(struct server *serv, char *newnick) {
	set_nickname(serv, newnick);
}

void fe_ignore_update(int level) {
	/* FIXME: implement */
}

void fe_beep(void) {
	gdk_beep();
}

void fe_lastlog(session *sess, session *lastlog_sess, char *sstr) {
	/* FIXME: implement */
}

void fe_set_lag(server *serv, int lag) {
	/* FIXME: implement */
}

void fe_set_throttle (server *serv) {
	/* FIXME: implement */
}

void fe_set_away(server *serv) {
	/* FIXME: implement */
}

void fe_serverlist_open (session *sess) {
	/* FIXME: implement */
}

void fe_get_str(char *prompt, char *def, void *callback, void *ud) {
	/* FIXME: implement */
}

void fe_get_int(char *prompt, int def, void *callback, void *ud) {
	/* FIXME: implement */
}

void fe_ctrl_gui(session *sess, int action, int arg) {
	/* FIXME: implement */
}

void
fe_confirm (const char *message, void (*yesproc)(void *), void (*noproc)(void *), void *ud) {
}
