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
#include "../common/util.h"
#include "gui.h"
#include "navigation_tree.h"
#include "textgui.h"
#include "main_window.h"
#include "userlist_gui.h"
#include "preferences.h"
#include "setup_druid.h"
#include "palette.h"
#include "preferences_plugins_page.h"
#include "channel_list.h"
#include "transfers.h"

int fe_args(int argc, char *argv[]) {
	if(argc > 1) {
		if(!strcasecmp(argv[1], "--version") || !strcasecmp(argv[1], "-v")) {
			puts(VERSION);
		return 0;
		}
	}
	gnome_program_init("xchat test", "0.1", LIBGNOMEUI_MODULE, argc, argv, NULL);

	/* FIXME: this is kind of a silly place to put this, but it seems to want to
		  follow gnome_program_init */
	gnome_window_icon_set_default_from_file ("data/xchat-gnome-small.png");
	return 1;
}

void fe_init(void) {
	u = userlist_new ();
	gui.quit = FALSE;
	servlist_init();
	palette_init();
	initialize_gui_1();
	if(!preferences_exist()) {
		run_setup_druid();
	}
	load_preferences();
	initialize_gui_2();
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
	return channel_list_exists(serv);
}

void fe_add_chan_list(struct server *serv, char *chan, char *users, char *topic) {
	channel_list_append(serv, chan, users, topic);
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
	kill_session_callback(sess);
}

void fe_progressbar_start(struct session *sess) {
	/* FIXME: implement */
}

void fe_progressbar_end(struct server *serv) {
	/* FIXME: implement */
}

void fe_print_text(struct session *sess, char *text) {
	session_gui *tgui = (session_gui *) sess->gui;
	if(tgui == NULL)
		return;
	text_gui_print(tgui->buffer, text, TRUE);
	sess->new_data = TRUE;
	navigation_tree_set_hilight(sess);
}

void fe_userlist_insert(struct session *sess, struct User *newuser, int row, int sel) {
	userlist_insert (u, sess, newuser, row, sel);
}

int fe_userlist_remove(struct session *sess, struct User *user) {
	return userlist_remove (u, sess, user);
}

void fe_userlist_rehash(struct session *sess, struct User *user) {
	userlist_update (u, sess, user);
}

void fe_userlist_move(struct session *sess, struct User *user, int new_row) {
	userlist_move (u, sess, user, new_row);
}

void fe_userlist_numbers(struct session *sess) {
	/* FIXME: implement */
}

void fe_userlist_clear(struct session *sess) {
	userlist_clear (u, sess);
}

void fe_dcc_add(struct DCC *dcc) {
	switch(dcc->type) {
		case TYPE_RECV:
		case TYPE_SEND:
			add_transfer(dcc);
			break;

		default:
	}
}

void fe_dcc_update(struct DCC *dcc) {
	switch(dcc->type) {
		case TYPE_RECV:
		case TYPE_SEND:
			update_transfer(dcc);
			break;

		default:
	}
}

void fe_dcc_remove(struct DCC *dcc) {
	g_print("fe_dcc_remove()\n");
	switch(dcc->type) {
		case TYPE_RECV:
		case TYPE_SEND:
			remove_transfer(dcc);
			break;

		default:
	}
}

int fe_dcc_open_recv_win(int passive) {
	return TRUE;
}

int fe_dcc_open_send_win(int passive) {
	return TRUE;
}

int fe_dcc_open_chat_win(int passive) {
	return TRUE;
}

void fe_clear_channel(struct session *sess) {
	navigation_tree_set_disconn(sess);
}

void fe_session_callback(struct session *sess) {
	/* this frees things */
	/* FIXME: implement */
}

void fe_server_callback(struct server *serv) {
	/* this frees things */
	/* FIXME: implement */
}

void fe_url_add(const char *text) {
	/* FIXME: implement */
}

void fe_pluginlist_update(void) {
	preferences_plugins_page_populate();
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
	if(sess == gui.current_session)
		rename_main_window(sess->server->networkname, sess->channel);
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
	GSList *list = sess_list;
	session *sess;
	gdouble per;
	char tip[64];
	unsigned long nowtim;
	session_gui *tgui;

	if(lag == -1) {
		if(!serv->lag_sent)
			return;
		nowtim = make_ping_time();
		lag = (nowtim - serv->lag_sent) / 100000;
	}

	per = (double)((double)lag / 40.0);
	if(per > 1.0)
		per = 1.0;
	snprintf(tip, sizeof(tip) - 1, "%s%d.%ds lag", serv->lag_sent ? "+" : "", lag / 10, lag % 10);
	while(list) {
		sess = list->data;
		if(sess->server == serv) {
			tgui = (session_gui*) sess->gui;
			tgui->lag_value = per;
			if(tgui->lag_text)
				free(tgui->lag_text);
			tgui->lag_text = strdup(tip);
		}
		list = list->next;
	}
	if(serv == gui.current_session->server) {
		set_statusbar();
	}
}

void fe_set_throttle (server *serv) {
	GSList *list = sess_list;
	session *sess;
	gdouble per;
	char tip[64];
	session_gui *tgui;

	per = (gdouble) serv->sendq_len / 1024.0;
	if(per > 1.0)
		per = 1.0;

	snprintf(tip, sizeof(tip) - 1, "%d bytes buffered", serv->sendq_len);
	while(list) {
		sess = list->data;
		if(sess->server == serv) {
			tgui = (session_gui*) sess->gui;
			if (tgui)
			{
				tgui->queue_value = per;
				if(tgui->queue_text)
					free(tgui->queue_text);
				if(per != 0) {
					tgui->queue_text = strdup(tip);
				} else {
					tgui->queue_text = NULL;
				}
			}
		}
		list = list->next;
	}
	if(serv == gui.current_session->server) {
		set_statusbar();
	}
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
