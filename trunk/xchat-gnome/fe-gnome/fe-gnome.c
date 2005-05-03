/*
 * fe-gnome.c - main frontend implementation
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

#include <glib.h>
#include <gnome.h>
#include <config.h>
#include "../common/xchat.h"
#include "../common/xchatc.h"
#include "../common/servlist.h"
#include "../common/fe.h"
#include "../common/util.h"
#include "../common/cfgfiles.h"
#include "gui.h"
#include "navigation-tree.h"
#include "textgui.h"
#include "main-window.h"
#include "userlist-gui.h"
#include "preferences.h"
#include "setup-dialog.h"
#include "palette.h"
#include "preferences-page-plugins.h"
#include "channel-list.h"
#include "util.h"
#include "plugins.h"

static gboolean opt_version = FALSE;
static gchar *opt_cfgdir = NULL;

static GOptionEntry entries[] =
{
	{ "cfgdir",  'd', 0, G_OPTION_ARG_STRING, &opt_cfgdir,  "Use directory instead of the default config dir", "directory"},
	{ "version", 'v', 0, G_OPTION_ARG_NONE,   &opt_version, "Show version information",                        NULL},
};

int
fe_args (int argc, char *argv[])
{
	GError *error = NULL;
	GOptionContext *context;

	context = g_option_context_new ("");
	g_option_context_add_main_entries (context, entries, NULL); /* FIXME - set translation domain */
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	g_option_context_parse (context, &argc, &argv, &error);

	if (error) {
		fprintf (stderr, "xchat-gnome: %s\nTry `xchat-gnome --help' for more information\n", error->message);
		g_error_free (error);
		return 0;
	}

	g_option_context_free (context);

	if (opt_version) {
		g_print ("xchat-gnome %s\n", VERSION);
		return 0;
	}

	if (opt_cfgdir)
		xdir_fs = opt_cfgdir;

	gnome_program_init ("xchat-gnome", VERSION, LIBGNOMEUI_MODULE, argc, argv, NULL);

	/* FIXME: this is kind of a silly place to put this, but it seems to want to
		  follow gnome_program_init */
	gnome_window_icon_set_default_from_file (XCHATSHAREDIR"/xchat-gnome-small.png");
	return 1;
}

void
fe_init (void)
{
	u = userlist_new ();
	gui.quit = FALSE;
	servlist_init ();
	palette_init ();
	initialize_gui_1 ();
	if (!preferences_exist ())
		run_setup_dialog ();
	else
		set_version ();
	load_preferences ();
	initialize_gui_2 ();
	run_main_window ();

	/* Force various window-related options to match our interaction model */
	prefs.use_server_tab = TRUE;
	prefs.notices_tabs = FALSE;

	/* Don't allow the core to autoload plugins. We use our own
	 * method for autoloading.
	 */
	skip_plugins = 1;

	plugins_initialize ();
	autoload_plugins ();
}

void
fe_main (void)
{
	gtk_main ();
	/* sleep for 3 seconds so any QUIT messages are not lost. The  */
	/* GUI is closed at this point, so the user doesn't even know! */
	if (prefs.wait_on_exit)
		sleep (3);
}

void
fe_cleanup (void)
{
	/* FIXME: implement */
}

void
fe_exit (void)
{
	gtk_main_quit ();
}

int
fe_timeout_add (int interval, void *callback, void *userdata)
{
	return g_timeout_add (interval, (GSourceFunc) callback, userdata);
}

void
fe_timeout_remove (int tag)
{
	g_source_remove(tag);
}

void
fe_new_window (struct session *sess, int focus)
{
	if (sess->type == SESS_SERVER)
		navigation_tree_create_new_network_entry (gui.server_tree, sess);
	else if (sess->type == SESS_CHANNEL || sess->type == SESS_DIALOG)
		navigation_tree_create_new_channel_entry (gui.server_tree, sess);
	text_gui_add_text_buffer (sess);
}

void
fe_new_server (struct server *serv)
{
	/* FIXME: implement */
}

void
fe_add_rawlog (struct server *serv, char *text, int len, int outbound)
{
	/* FIXME: implement */
}

void
fe_message (char *msg, int wait)
{
	/* FIXME: implement */
}

int
fe_input_add (int sok, int flags, void *func, void *data)
{
	int tag, type = 0;
	GIOChannel *channel;

	channel = g_io_channel_unix_new (sok);

	if (flags & FIA_READ)
		type |= G_IO_IN | G_IO_HUP | G_IO_ERR;
	if (flags & FIA_WRITE)
		type |= G_IO_OUT | G_IO_ERR;
	if (flags & FIA_EX)
		type |= G_IO_PRI;

	tag = g_io_add_watch (channel, type, (GIOFunc) func, data);
	g_io_channel_unref (channel);

	return tag;
}

void
fe_input_remove (int tag)
{
	g_source_remove (tag);
}

void
fe_idle_add (void *func, void *data)
{
	g_idle_add (func, data);
}

void
fe_set_topic (struct session *sess, char *topic)
{
	set_gui_topic (sess, topic);
}

void
fe_set_hilight (struct session *sess)
{
	navigation_model_set_hilight (gui.tree_model, sess);
}

void
fe_set_tab_color (struct session *sess, int col, int flash)
{
	/* FIXME: implement */
}

void
fe_update_mode_buttons (struct session *sess, char mode, char sign)
{
	/* FIXME: implement */
}

void
fe_update_channel_key (struct session *sess)
{
	/* FIXME: implement */
}

void
fe_update_channel_limit (struct session *sess)
{
	/* FIXME: implement */
}

int
fe_is_chanwindow (struct server *serv)
{
	return channel_list_exists (serv);
}

void
fe_add_chan_list (struct server *serv, char *chan, char *users, char *topic)
{
	channel_list_append (serv, chan, users, topic);
}

void
fe_chan_list_end (struct server *serv)
{
	/* FIXME: implement */
}

int
fe_is_banwindow (struct session *sess)
{
	/* FIXME: implement */
	return 0;
}

void
fe_add_ban_list (struct session *sess, char *mask, char *who, char *when, int is_exemption)
{
	/* FIXME: implement */
}

void
fe_ban_list_end (struct session *sess, int is_exemption)
{
	/* FIXME: implement */
}

void
fe_notify_update (char *name)
{
	/* FIXME: implement */
}

void
fe_text_clear (struct session *sess)
{
	clear_buffer (sess);
}

void
fe_close_window (struct session *sess)
{
	if (!gui.quit)
  	navigation_tree_remove (gui.server_tree, sess);
	kill_session_callback (sess);
}

void
fe_progressbar_start (struct session *sess)
{
	/* FIXME: implement */
}

void
fe_progressbar_end (struct server *serv)
{
	/* FIXME: implement */
}

void
fe_print_text (struct session *sess, char *text)
{
	session_gui *tgui = (session_gui *) sess->gui;
	if (tgui == NULL)
		return;
	text_gui_print (tgui->buffer, text, TRUE);
	sess->new_data = TRUE;
	navigation_model_set_hilight (gui.tree_model, sess);
}

void
fe_userlist_insert (struct session *sess, struct User *newuser, int row, int sel)
{
	userlist_insert (u, sess, newuser, row, sel);
}

int
fe_userlist_remove (struct session *sess, struct User *user)
{
	return userlist_remove (u, sess, user);
}

void
fe_userlist_rehash (struct session *sess, struct User *user)
{
	userlist_update (u, sess, user);
}

void
fe_userlist_move (struct session *sess, struct User *user, int new_row)
{
	userlist_move (u, sess, user, new_row);
}

void
fe_userlist_numbers (struct session *sess)
{
	/* FIXME: implement */
}

void
fe_userlist_clear (struct session *sess)
{
	userlist_clear (u, sess);
}

void
fe_dcc_add (struct DCC *dcc)
{
}

void
fe_dcc_update(struct DCC *dcc)
{
}

void
fe_dcc_remove (struct DCC *dcc)
{
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

void
fe_clear_channel(struct session *sess)
{
	navigation_model_set_disconn (gui.tree_model, sess);
}

void
fe_session_callback(struct session *sess)
{
	/* this frees things */
	/* FIXME: implement */
}

void
fe_server_callback(struct server *serv)
{
	/* this frees things */
	/* FIXME: implement */
}

void
fe_url_add(const char *text)
{
	/* FIXME: implement */
}

void
fe_pluginlist_update (void)
{
}

void
fe_buttons_update (struct session *sess)
{
	/* FIXME: implement */
}

void
fe_dlgbuttons_update (struct session *sess)
{
	/* FIXME: implement */
}

void
fe_dcc_send_filereq (struct session *sess, char *nick, int maxcps, int passive)
{
	/* FIXME: implement */
}

void
fe_set_channel (struct session *sess)
{
	navigation_tree_set_channel_name (gui.server_tree, sess);
}

void
fe_set_title(struct session *sess)
{
	if (sess == gui.current_session)
	{
		if (sess->server->network == NULL)
		{
			rename_main_window (NULL, sess->channel);
		}
		else
		{
			ircnet *net = sess->server->network;
			rename_main_window (net->name, sess->channel);
		}
	}
}

void
fe_set_nonchannel (struct session *sess, int state)
{
	/* stub? */
}

void
fe_set_nick (struct server *serv, char *newnick)
{
	set_nickname (serv, newnick);
}

void
fe_ignore_update (int level)
{
	/* FIXME: implement */
}

void
fe_beep (void)
{
	gdk_beep();
}

typedef struct
{
	session *sess;
	unsigned char *sstr;
} fe_lastlog_info;

static void
fe_lastlog_foreach (GtkXText *xtext, unsigned char *text, fe_lastlog_info *info)
{
	session_gui *tgui = (session_gui *) info->sess->gui;

	if (nocasestrstr (text, info->sstr))
		text_gui_print (tgui->buffer, text, TRUE);
}

void
fe_lastlog (session *sess, session *lastlog_sess, char *sstr)
{
	session_gui *tgui = (session_gui *) sess->gui;
	session_gui *lgui = (session_gui *) lastlog_sess->gui;
	if (gtk_xtext_is_empty (tgui->buffer))
	{
		text_gui_print (lgui->buffer, _("Search buffer is empty.\n"), TRUE);
	}
	else
	{
		fe_lastlog_info info;
		info.sess = lastlog_sess;
		info.sstr = sstr;
		gtk_xtext_foreach (tgui->buffer, (GtkXTextForeach) fe_lastlog_foreach, &info);
	}
}

void
fe_set_lag (server *serv, int lag)
{
	GSList *list = sess_list;
	session *sess;
	gdouble per;
	char tip[64];
	unsigned long nowtim;
	session_gui *tgui;

	if (gui.quit)
		return;

	if (lag == -1)
	{
		if (!serv->lag_sent)
			return;
		nowtim = make_ping_time ();
		lag = (nowtim - serv->lag_sent) / 100000;
	}

	per = (double)((double)lag / 40.0);
	if (per > 1.0)
		per = 1.0;
	snprintf (tip, sizeof (tip) - 1, _("%s%d.%ds lag"), serv->lag_sent ? "+" : "", lag / 10, lag % 10);
	while (list)
	{
		sess = list->data;
		if (sess->server == serv)
		{
			tgui = (session_gui*) sess->gui;
			if (tgui)
			{
				tgui->lag_value = per;
				if (tgui->lag_text)
					free (tgui->lag_text);
				tgui->lag_text = strdup(tip);
			}
		}
		list = list->next;
	}
	if (serv == gui.current_session->server)
	{
		set_statusbar ();
	}
}

void
fe_set_throttle (server *serv)
{
	GSList *list = sess_list;
	session *sess;
	gdouble per;
	char tip[64];
	session_gui *tgui;

	if (gui.quit)
		return;

	per = (gdouble) serv->sendq_len / 1024.0;
	if (per > 1.0)
		per = 1.0;

	snprintf (tip, sizeof (tip) - 1, _("%d bytes buffered"), serv->sendq_len);
	while (list)
	{
		sess = list->data;
		if (sess->server == serv)
		{
			tgui = (session_gui*) sess->gui;
			if (tgui)
			{
				tgui->queue_value = per;
				if (tgui->queue_text)
					free(tgui->queue_text);
				if (per != 0)
				{
					tgui->queue_text = strdup(tip);
				} else {
					tgui->queue_text = NULL;
				}
			}
		}
		list = list->next;
	}
	if (serv == gui.current_session->server)
	{
		set_statusbar ();
	}
}

void
fe_set_away (server *serv)
{
	/* FIXME: implement */
}

void
fe_serverlist_open (session *sess)
{
	/* FIXME: implement */
}

void
fe_ctrl_gui(session *sess, int action, int arg)
{
	GtkWidget *window = glade_xml_get_widget (gui.xml, "xchat-gnome");

	switch (action) {
		case 0:
			gtk_window_iconify (GTK_WINDOW (window));
			gtk_widget_hide (window);
			break;

		case 1:
			gtk_window_deiconify (GTK_WINDOW (window));
			gtk_widget_show (window);
			gtk_window_present (GTK_WINDOW (window));
			break;

		case 2:
			break;

		case 3:
			break;

		case 4:
			break;

		case 5:
			gtk_window_iconify (GTK_WINDOW (window));
			break;
	}
}

void
fe_confirm (const char *message, void (*yesproc)(void *), void (*noproc)(void *), void *ud)
{
}

int
fe_gui_info (session *sess, int info_type)
{
	GtkWidget *window = glade_xml_get_widget (gui.xml, "xchat-gnome");

	switch (info_type) {
		case 0:
			if (!GTK_WIDGET_VISIBLE (GTK_WINDOW (window)))
				return 2;
			if (gtk_window_is_active (GTK_WINDOW (window)))
				return 1;
			return 0;
			break;
	}

	return -1;
}

void
fe_set_inputbox_cursor (session *sess, int delta, int pos)
{
	/* FIXME: implement? */
}

void
fe_set_inputbox_contents (session *sess, char *text)
{
	/* FIXME: implement? */
}

char *
fe_get_inputbox_contents (session *sess)
{
	/* FIXME: implement? */
	return NULL;
}

int
fe_get_inputbox_cursor (struct session *sess)
{
	/* FIXME: implement? */
	return 0;
}

static void
get_str_response (GtkDialog *dialog, gint arg1, gpointer entry)
{
	void (*callback) (int cancel, char *text, void *user_data);
	char *text;
	void *user_data;

	text = (char *) gtk_entry_get_text (GTK_ENTRY (entry));
	callback = g_object_get_data (G_OBJECT (dialog), "cb");
	user_data = g_object_get_data (G_OBJECT (dialog), "ud");

	switch (arg1)
	{
	case GTK_RESPONSE_REJECT:
		callback (TRUE, text, user_data);
		gtk_widget_destroy (GTK_WIDGET (dialog));
		break;
	case GTK_RESPONSE_ACCEPT:
		callback (FALSE, text, user_data);
		gtk_widget_destroy (GTK_WIDGET (dialog));
		break;
	}
}
static void
str_enter (GtkWidget *entry, GtkWidget *dialog)
{
	gtk_dialog_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);
}

void
fe_get_str (char *msg, char *def, void *callback, void *userdata)
{
	GtkWidget *dialog;
	GtkWidget *entry;
	GtkWidget *hbox;
	GtkWidget *label;

	dialog = gtk_dialog_new_with_buttons (msg, NULL, 0, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	gtk_box_set_homogeneous (GTK_BOX (GTK_DIALOG (dialog)->vbox), TRUE);
	gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);
	hbox = gtk_hbox_new (TRUE, 0);

	g_object_set_data (G_OBJECT (dialog), "cb", callback);
	g_object_set_data (G_OBJECT (dialog), "ud", userdata);

	entry = gtk_entry_new ();
	g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (str_enter), dialog);
	gtk_entry_set_text (GTK_ENTRY (entry), def);
	gtk_box_pack_end (GTK_BOX (hbox), entry, 0, 0, 0);

	label = gtk_label_new (msg);
	gtk_box_pack_end (GTK_BOX (hbox), label, 0, 0, 0);

	g_signal_connect (G_OBJECT (dialog), "response", G_CALLBACK (get_str_response), entry);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), hbox);

	gtk_widget_show_all (dialog);
}

static void
get_number_response (GtkDialog *dialog, gint arg1, gpointer spin)
{
	void (*callback) (int cancel, int value, void *user_data);
	int num;
	void *user_data;

	num = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spin));
	callback = g_object_get_data (G_OBJECT (dialog), "cb");
	user_data = g_object_get_data (G_OBJECT (dialog), "ud");

	switch (arg1)
	{
	case GTK_RESPONSE_REJECT:
		callback (TRUE, num, user_data);
		gtk_widget_destroy (GTK_WIDGET (dialog));
		break;
	case GTK_RESPONSE_ACCEPT:
		callback (FALSE, num, user_data);
		gtk_widget_destroy (GTK_WIDGET (dialog));
		break;
	}
}

void
fe_get_int (char *msg, int def, void *callback, void *userdata)
{
	GtkWidget *dialog;
	GtkWidget *spin;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkAdjustment *adj;

	dialog = gtk_dialog_new_with_buttons (msg, NULL, 0, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	gtk_box_set_homogeneous (GTK_BOX (GTK_DIALOG (dialog)->vbox), TRUE);
	gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);
	hbox = gtk_hbox_new (TRUE, 0);

	g_object_set_data (G_OBJECT (dialog), "cb", callback);
	g_object_set_data (G_OBJECT (dialog), "ud", userdata);

	spin = gtk_spin_button_new (NULL, 1, 0);
	adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (spin));
	adj->lower = 0;
	adj->upper = 1024;
	adj->step_increment = 1;
	gtk_adjustment_changed (adj);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin), def);
	gtk_box_pack_end (GTK_BOX (hbox), spin, 0, 0, 0);

	label = gtk_label_new (msg);
	gtk_box_pack_end (GTK_BOX (hbox), label, 0, 0, 0);

	g_signal_connect (G_OBJECT (dialog), "response", G_CALLBACK (get_number_response), spin);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), hbox);

	gtk_widget_show_all (dialog);
}

void
fe_open_url (const char *url)
{
	GError *err = NULL;

	if (strstr (url, "://") == NULL) {
		gchar *newword = g_strdup_printf ("http://%s", url);
		gnome_url_show (newword, &err);
		g_free (newword);
	} else {
		gnome_url_show (url, &err);
	}

	if (err != NULL) {
		gchar *message = g_strdup_printf ("Unable to show '%s'", url);
		error_dialog (_(message), _(err->message));
		g_free (message);
		g_error_free (err);
	}
}
