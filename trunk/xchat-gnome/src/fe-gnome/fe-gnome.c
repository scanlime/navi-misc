/*
 * fe-gnome.c - main frontend implementation
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
#include <glib.h>
#include <gnome.h>
#include <libgnomevfs/gnome-vfs.h>
#include <gconf/gconf-client.h>
#include "gui.h"
#include "navigation-tree.h"
#include "main-window.h"
#include "userlist-gui.h"
#include "preferences.h"
#include "setup-dialog.h"

#include "conversation-panel.h"
#include "status-bar.h"
#include "topic-label.h"

#include "palette.h"
#include "preferences-page-plugins.h"
#include "channel-list.h"
#include "util.h"
#include "plugins.h"
#include "../common/xchat.h"
#include "../common/xchatc.h"
#include "../common/servlist.h"
#include "../common/fe.h"
#include "../common/util.h"
#include "../common/cfgfiles.h"

static gboolean opt_version = FALSE;
static gboolean opt_noauto = FALSE;
static gboolean opt_noplugins = FALSE;
static gchar *opt_cfgdir = NULL;

static GOptionEntry entries[] = {
	{"cfgdir",     'd', 0, G_OPTION_ARG_FILENAME, &opt_cfgdir,           N_("Use directory instead of the default config dir"), "directory"},
	{"no-auto",    'a', 0, G_OPTION_ARG_NONE,     &arg_dont_autoconnect, N_("Don't auto-connect to servers"),                   NULL},
	{"no-plugins", 'n', 0, G_OPTION_ARG_NONE,     &opt_noplugins,        N_("Don't auto-load plugins"),                         NULL},
	{"url",        'u', 0, G_OPTION_ARG_STRING,   &arg_url,              N_("Open an irc:// url"),                              "irc://server:port/channel"},
	{"version",    'v', 0, G_OPTION_ARG_NONE,     &opt_version,          N_("Show version information"),                        NULL},
	{ NULL }
};

static void setup_sm (gint argc, gchar *argv[]);
static gint save_session (GnomeClient *client, gint phase, GnomeSaveStyle save_style,
                          gint is_shutdown, GnomeInteractStyle interact_style,
                          gint is_fast, gpointer client_data);
static void kill_session (GnomeClient* client, gpointer client_data);

int
fe_args (int argc, char *argv[])
{
	GError *error = NULL;
	GOptionContext *context;
	GnomeProgram *program;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	context = g_option_context_new ("");

#ifdef HAVE_LIBGNOME_214
	g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
	program = gnome_program_init (PACKAGE, VERSION,
	                              LIBGNOMEUI_MODULE, argc, argv,
	                              GNOME_PARAM_GOPTION_CONTEXT, context,
	                              GNOME_PARAM_HUMAN_READABLE_NAME, _("IRC Chat"),
	                              GNOME_PROGRAM_STANDARD_PROPERTIES,
	                              NULL);
#else
	g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	g_option_context_parse (context, &argc, &argv, &error);
#endif

	if (error) {
		fprintf (stderr, _("xchat-gnome: %s\nTry `xchat-gnome --help' for more information\n"), error->message);
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

	setup_sm (argc, argv);

	return -1;
}

void
fe_init (void)
{
	gchar *accel_map;
	GConfClient *client;

	gnome_vfs_init ();

	client = gconf_client_get_default ();
	gconf_client_add_dir (client, "/apps/xchat", GCONF_CLIENT_PRELOAD_NONE, NULL);
	g_object_unref (client);

	u = userlist_new ();
	gui.quit = FALSE;
	servlist_init ();
	palette_init ();
	if (!preferences_exist ())
		run_setup_dialog ();
	else
		set_version ();
	load_preferences ();
	initialize_gui_1 ();
	initialize_gui_2 ();
	run_main_window ();

	/* Force various window-related options to match our interaction model */
	prefs.use_server_tab = TRUE;
	prefs.notices_tabs = FALSE;
	prefs.servernotice = TRUE;

	/* If we don't have a specific DCC IP address, force get-from-server */
	if (strlen (prefs.dcc_ip_str) == 0)
		prefs.ip_from_server = TRUE;

	/* Don't allow the core to autoload plugins. We use our own
	 * method for autoloading.
	 */
	arg_skip_plugins = 1;

	accel_map = g_strdup_printf ("%s%c%s", get_xdir_fs (), G_DIR_SEPARATOR, "accelerator_map");
	if (g_file_test (accel_map, G_FILE_TEST_EXISTS))
		gtk_accel_map_load (accel_map);
	g_free (accel_map);

#ifdef USE_PLUGIN
	plugins_initialize ();
#endif
}

void
fe_main (void)
{
	gtk_main ();
	/* sleep for 3 seconds so any QUIT messages are not lost. The  */
	/* GUI is closed at this point, so the user doesn't even know! */

	/* FIXME: this is a crappy hack copied from fe-gtk. There's got
	 * to be a way to ensure that the quit messages get sent before
	 * we finish */
	if (prefs.wait_on_exit)
		sleep (3);
}

void
fe_cleanup (void)
{
	gchar *accel_map;

	accel_map = g_strdup_printf ("%s%c%s", get_xdir_fs (), G_DIR_SEPARATOR, "accelerator_map");
	gtk_accel_map_save (accel_map);
	g_free (accel_map);
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
	g_source_remove (tag);
}

void
fe_new_window (struct session *sess, int focus)
{
	static gboolean loaded = FALSE;

	if (focus) {
		gui.current_session = sess;
		topic_label_set_topic (TOPIC_LABEL (gui.topic_label), sess, sess->topic);
	}

	conversation_panel_add_session (CONVERSATION_PANEL (gui.conversation_panel), sess, (gboolean) focus);
	if (sess->type == SESS_SERVER)
		navigation_tree_create_new_network_entry (gui.server_tree, sess);
	else if (sess->type == SESS_CHANNEL || sess->type == SESS_DIALOG)
		navigation_tree_create_new_channel_entry (gui.server_tree, sess, (gboolean) focus);
#ifdef USE_PLUGIN
	if (!(opt_noplugins || loaded)) {
		loaded = TRUE;
		autoload_plugins ();
	}
#endif
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
	topic_label_set_topic (TOPIC_LABEL (gui.topic_label), sess, topic);
}

void
fe_set_hilight (struct session *sess)
{
	navigation_model_set_hilight (gui.tree_model, sess);
	if (!gtk_window_is_active (GTK_WINDOW (gui.main_window)))
		gtk_window_set_urgency_hint (GTK_WINDOW (gui.main_window), TRUE);
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
	conversation_panel_clear (CONVERSATION_PANEL (gui.conversation_panel), sess);
}

void
fe_close_window (struct session *sess)
{
	if (!gui.quit) {
		if (sess->type == SESS_CHANNEL)
			navigation_tree_remove_channel (gui.server_tree, sess);
		else
			navigation_tree_remove_server (gui.server_tree, sess);
	}

	session_free (sess);
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
	conversation_panel_print (CONVERSATION_PANEL (gui.conversation_panel), sess, text, prefs.indent_nicks);
	sess->new_data = TRUE;
	navigation_model_set_hilight (gui.tree_model, sess);
	if (sess->nick_said)
		if (!gtk_window_is_active (GTK_WINDOW (gui.main_window)))
			gtk_window_set_urgency_hint (GTK_WINDOW (gui.main_window), TRUE);
}

void
fe_userlist_insert (struct session *sess, struct User *newuser, int row, int sel)
{
	userlist_insert (u, sess, newuser, row, sel);
}

int
fe_userlist_remove (struct session *sess, struct User *user)
{
	return userlist_remove_user (u, sess, user);
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
	userlist_clear_all (u, sess);
}

void
fe_dcc_add (struct DCC *dcc)
{
	/* FIXME: chats? */
	dcc_window_add (gui.dcc, dcc);
}

void
fe_dcc_update (struct DCC *dcc)
{
	/* FIXME: chats? */
	dcc_window_update (gui.dcc, dcc);
}

void
fe_dcc_remove (struct DCC *dcc)
{
	/* FIXME: chats? */
	dcc_window_remove (gui.dcc, dcc);
}

int
fe_dcc_open_recv_win (int passive)
{
	/* FIXME: implement? */
	return TRUE;
}

int
fe_dcc_open_send_win (int passive)
{
	/* FIXME: implement? */
	return TRUE;
}

int
fe_dcc_open_chat_win (int passive)
{
	/* FIXME: implement? */
	return TRUE;
}

void
fe_clear_channel (struct session *sess)
{
	navigation_model_set_disconn (gui.tree_model, sess);
}

void
fe_session_callback (struct session *sess)
{
	/* this frees things */
	/* FIXME: implement */
}

void
fe_server_callback (struct server *serv)
{
	/* this frees things */
	/* FIXME: implement */
}

void
fe_url_add (const char *text)
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
fe_set_title (struct session *sess)
{
	if (sess == gui.current_session) {
		if (sess->server->network == NULL) {
			rename_main_window (NULL, sess->channel);
		} else {
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
	gdk_beep ();
}

void
fe_lastlog (session * sess, session * lastlog_sess, char *sstr)
{
	conversation_panel_lastlog (CONVERSATION_PANEL (gui.conversation_panel), sess, lastlog_sess, sstr);
}

void
fe_set_lag (server *serv, int lag)
{
	unsigned long now;
	float seconds;

	if (gui.quit)
		return;

	if (lag == -1) {
		if (!serv->lag_sent)
			return;
		now = make_ping_time ();
		seconds = (now - serv->lag_sent) / 1000000.0f;
	} else {
		seconds = lag / 10.0f;
	}

	status_bar_set_lag (STATUS_BAR (gui.status_bar), serv, seconds, (serv->lag_sent != 0.0f));
}

void
fe_set_throttle (server * serv)
{
	if (gui.quit)
		return;

	status_bar_set_queue (STATUS_BAR (gui.status_bar), serv, serv->sendq_len);
}

void
fe_set_away (server * serv)
{
	set_nickname_color (serv);
}

void
fe_serverlist_open (session * sess)
{
	/* FIXME: implement */
}

void
fe_ctrl_gui (session * sess, int action, int arg)
{
	switch (action) {
	case 0:
		gtk_widget_hide (gui.main_window);
		break;

	case 1:
		gtk_widget_show (gui.main_window);
		gtk_window_present (GTK_WINDOW (gui.main_window));
		break;

	case 2:
		break;

	case 3:
		break;

	case 4:
		break;

	case 5:
		gtk_window_iconify (GTK_WINDOW (gui.main_window));
		break;
	}
}

void
fe_confirm (const char *message, void (*yesproc) (void *), void (*noproc) (void *), void *ud)
{
}

int
fe_gui_info (session * sess, int info_type)
{
	switch (info_type) {
	case 0:
		if (!GTK_WIDGET_VISIBLE (GTK_WINDOW (gui.main_window)))
			return 2;
		if (gtk_window_is_active (GTK_WINDOW (gui.main_window)))
			return 1;
		return 0;
		break;
	}

	return -1;
}

void *
fe_gui_info_ptr (session *sess, int info_type)
{
	return NULL;
	/* FIXME implement? */
}

void
fe_set_inputbox_cursor (session * sess, int delta, int pos)
{
	/* FIXME: implement? */
}

void
fe_set_inputbox_contents (session * sess, char *text)
{
	/* FIXME: implement? */
}

char *
fe_get_inputbox_contents (session * sess)
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
get_str_response (GtkDialog * dialog, gint arg1, gpointer entry)
{
	void (*callback) (int cancel, char *text, void *user_data);
	char *text;
	void *user_data;

	text = (char *) gtk_entry_get_text (GTK_ENTRY (entry));
	callback = g_object_get_data (G_OBJECT (dialog), "cb");
	user_data = g_object_get_data (G_OBJECT (dialog), "ud");

	switch (arg1) {
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
str_enter (GtkWidget * entry, GtkWidget * dialog)
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
get_number_response (GtkDialog * dialog, gint arg1, gpointer spin)
{
	void (*callback) (int cancel, int value, void *user_data);
	int num;
	void *user_data;

	num = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spin));
	callback = g_object_get_data (G_OBJECT (dialog), "cb");
	user_data = g_object_get_data (G_OBJECT (dialog), "ud");

	switch (arg1) {
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
		gchar *message = g_strdup_printf (_("Unable to show '%s'"), url);
		error_dialog (message, err->message);
		g_free (message);
		g_error_free (err);
	}
}

void
fe_menu_del (menu_entry *entry)
{
	/* FIXME: implement? */
}

void
fe_menu_add (menu_entry *entry)
{
	/* FIXME: implement? */
}

void
fe_menu_update (menu_entry *entry)
{
	/* FIXME: implement? */
}

void
fe_uselect (session *sess, char *word[], int do_clear, int scroll_to)
{
	/* FIXME: implement? */
}

void
fe_server_event (server *serv, int type, int arg)
{
	/* FIXME: implement? */
}

void
fe_userlist_set_selected (struct session *sess)
{
	/* FIXME: implement? */
}

static void
setup_sm (gint argc, gchar *argv[])
{
	GnomeClient *client;
	const gchar *prefix;

	client = gnome_master_client ();

	g_signal_connect (G_OBJECT (client), "save_yourself", G_CALLBACK (save_session), argv[0]);
	g_signal_connect (G_OBJECT (client), "die", G_CALLBACK (kill_session), NULL);

	prefix = gnome_client_get_config_prefix (client);
	/* FIXME: add discard command to remove saved state */
}

static gint
save_session (GnomeClient *client, gint phase, GnomeSaveStyle save_style,
              gint is_shutdown, GnomeInteractStyle interact_style,
              gint is_fast, gpointer client_data)
{
	gchar **argv;
	gint argc;
	const gchar *prefix;

	argv = g_new0 (gchar *, 3);
	argv[0] = client_data;
	argv[1] = "--no-auto";
	argc = 2;

	gnome_client_set_restart_style (client, GNOME_RESTART_IF_RUNNING);
	gnome_client_set_restart_command (client, argc, argv);
	gnome_client_set_clone_command (client, argc, argv);

	prefix = gnome_client_get_config_prefix (client);
	/* FIXME: save state */

	g_free (argv);

	return TRUE;
}

static void
kill_session (GnomeClient* client, gpointer client_data)
{
	gtk_widget_hide (GTK_WIDGET (gui.main_window));
	gtk_widget_hide (GTK_WIDGET (gui.dcc));
	userlist_gui_hide ();
	gui.quit = TRUE;
	xchat_exit ();
}
