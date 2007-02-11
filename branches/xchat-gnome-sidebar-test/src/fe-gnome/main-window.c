/*
 * main-window.c - main GUI window functions
 *
 * Copyright (C) 2004-2007 xchat-gnome team
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
#include <libgnome/libgnome.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "about.h"
#include "channel-list-window.h"
#include "connect-dialog.h"
#include "gui.h"
#include "main-window.h"
#include "navigation-tree.h"
#include "palette.h"
#include "preferences.h"
#include "userlist-gui.h"
#include "taco-bar.h"
#include "util.h"

#include "conversation-panel.h"
#include "find-bar.h"
#include "text-entry.h"
#include "topic-label.h"

#include "../common/xchatc.h"
#include "../common/outbound.h"
#include "../common/fe.h"

static void on_main_window_close (GtkWidget *widget, GdkEvent *event, gpointer data);
static void on_pgup (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void on_pgdn (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);

/* action callbacks */
static void on_irc_connect_activate (GtkAction *action, gpointer data);
static void on_irc_downloads_activate (GtkAction *action, gpointer data);
static void on_irc_quit_activate (GtkAction *action, gpointer data);
static void on_edit_cut_activate (GtkAction *action, gpointer data);
static void on_edit_copy_activate (GtkAction *action, gpointer data);
static void on_edit_paste_activate (GtkAction *action, gpointer data);
static void on_edit_preferences_activate (GtkAction *action, gpointer data);
static void on_network_reconnect_activate (GtkAction *action, gpointer data);
static void on_network_disconnect_activate (GtkAction *action, gpointer data);
static void on_network_channels_activate (GtkAction *action, gpointer data);
static void on_discussion_save_activate (GtkAction *action, gpointer data);
static void on_discussion_leave_activate (GtkAction *action, gpointer data);
static void on_close_activate (GtkAction *action, gpointer data);
static void on_discussion_find_activate (GtkAction *action, gpointer data);
static void on_discussion_bans_activate (GtkAction *action, gpointer data);
static void on_discussion_topic_change_activate (GtkButton *widget, gpointer data);
static void on_discussion_users_activate (GtkAction *action, gpointer data);
static void on_help_contents_activate (GtkAction *action, gpointer data);
static void on_help_about_activate (GtkAction *action, gpointer data);
static void on_nickname_clicked (GtkButton *widget, gpointer user_data);

static void on_add_widget (GtkUIManager *manager, GtkWidget *menu, GtkWidget *menu_vbox);

static gboolean on_resize (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
static gboolean on_hpane_move (GtkPaned *widget, GParamSpec *param_spec, gpointer data);

static gboolean on_main_window_focus_in (GtkWidget *widget, GdkEventFocus *event, gpointer data);
static gboolean on_main_window_configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data);

void initialize_taco_bar ();
static void nickname_style_set (GtkWidget *button, GtkStyle *previous_style, gpointer data);

static GtkActionEntry action_entries [] = {

	/* Toplevel */
	{ "IRC",         NULL, N_("_IRC") },
	{ "Edit",        NULL, N_("_Edit") },
	{ "Insert",      NULL, N_("In_sert") },
	{ "Network",     NULL, N_("_Network") },
	{ "Discussion",  NULL, N_("_Discussion") },
	{ "Go",          NULL, N_("_Go") },
	{ "Help",        NULL, N_("_Help") },
	{ "PopupAction", NULL, "" },

	/* IRC menu */
	{ "IRCConnect",   NULL,           N_("_Connect..."),        "<control>N", NULL, G_CALLBACK (on_irc_connect_activate) },
	{ "IRCDownloads", NULL,           N_("_File Transfers"), "<alt>F",     NULL, G_CALLBACK (on_irc_downloads_activate) },
	{ "IRCQuit",      GTK_STOCK_QUIT, N_("_Quit"),           "<control>Q", NULL, G_CALLBACK (on_irc_quit_activate) },

	/* Edit menu */
	{ "EditCut",         GTK_STOCK_CUT,         N_("Cu_t"),         "<control>X", NULL, G_CALLBACK (on_edit_cut_activate) },
	{ "EditCopy",        GTK_STOCK_COPY,        N_("_Copy"),        "<control>C", NULL, G_CALLBACK (on_edit_copy_activate) },
	{ "EditPaste",       GTK_STOCK_PASTE,       N_("_Paste"),       "<control>V", NULL, G_CALLBACK (on_edit_paste_activate) },
	{ "EditPreferences", GTK_STOCK_PREFERENCES, N_("Prefere_nces"), "",           NULL, G_CALLBACK (on_edit_preferences_activate) },

	/* Network menu */
	{ "NetworkReconnect",   GTK_STOCK_REFRESH,     N_("_Reconnect"),   "<control>R",        NULL, G_CALLBACK (on_network_reconnect_activate) },
	{ "NetworkDisconnect",  GTK_STOCK_DISCONNECT,  N_("_Disconnect"),  "",                  NULL, G_CALLBACK (on_network_disconnect_activate) },
	{ "NetworkClose",       GTK_STOCK_CLOSE,       N_("_Close"),       "<shift><control>W", NULL, G_CALLBACK (on_close_activate) },
	{ "NetworkChannels",    NULL,                  N_("_Channels..."), "<alt>C",            NULL, G_CALLBACK (on_network_channels_activate) },

	/* Discussion menu */
	{ "DiscussionSave",        GTK_STOCK_SAVE,           N_("_Save Transcript"), "<control>S", NULL, G_CALLBACK (on_discussion_save_activate) },
	{ "DiscussionLeave",       GTK_STOCK_QUIT,           N_("_Leave"),           "",           NULL, G_CALLBACK (on_discussion_leave_activate) },
	{ "DiscussionClose",       GTK_STOCK_CLOSE,          N_("Cl_ose"),           "<control>W", NULL, G_CALLBACK (on_close_activate) },
	{ "DiscussionFind",        GTK_STOCK_FIND,           N_("_Find"),            "<control>F", NULL, G_CALLBACK (on_discussion_find_activate) },
	{ "DiscussionChangeTopic", NULL,                     N_("Change _Topic"),    "<alt>T",     NULL, G_CALLBACK (on_discussion_topic_change_activate) },
	{ "DiscussionBans",        GTK_STOCK_DIALOG_WARNING, N_("_Bans..."),         "<alt>B",     NULL, G_CALLBACK (on_discussion_bans_activate) },
	{ "DiscussionUsers",       NULL,                     N_("_Users"),           "<control>U", NULL, G_CALLBACK (on_discussion_users_activate) },

	/* Help menu */
	{ "HelpContents", GTK_STOCK_HELP,  N_("_Contents"), "F1", NULL, G_CALLBACK (on_help_contents_activate) },
	{ "HelpAbout",    GTK_STOCK_ABOUT, N_("_About"),    NULL, NULL, G_CALLBACK (on_help_about_activate) },
};

void
initialize_taco_bar ()
{
    	/* FIXME: The sidebar stuff still needs to be i18nified */
    	GtkWidget *widget;
	GtkWidget *icon;

	// Servers
	widget = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (widget),
					     GTK_SHADOW_IN);
	icon = gtk_image_new_from_icon_name ("xchat-gnome-servers",
					 GTK_ICON_SIZE_BUTTON);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (widget),
				        GTK_POLICY_AUTOMATIC,
				        GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (widget),
			   GTK_WIDGET (gui.server_tree));
	taco_bar_pack_page (TACO_BAR (gui.server_bar), "connections",
			    "_Connections", icon, widget, TACO_BAR_TOP);
	gtk_widget_show (GTK_WIDGET (gui.server_tree));

	// File transfers
	widget = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (widget),
					     GTK_SHADOW_IN);
	icon = gtk_image_new_from_icon_name ("xchat-gnome-file-transfer",
					 GTK_ICON_SIZE_BUTTON);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (widget),
				        GTK_POLICY_AUTOMATIC,
				        GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (widget),
			   gtk_tree_view_new ());
	taco_bar_pack_page (TACO_BAR (gui.server_bar), "transfers",
			    "_File Transfers", icon, widget, TACO_BAR_TOP);
//	gtk_widget_show (GTK_WIDGET (gui.server_tree));

	// User list page
	widget = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (widget),
					     GTK_SHADOW_IN);
	icon = gtk_image_new_from_icon_name ("xchat-gnome-users",
					     GTK_ICON_SIZE_BUTTON);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (widget),
				        GTK_POLICY_NEVER,
				        GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (widget), gui.userlist);
	taco_bar_pack_page (TACO_BAR (gui.discussion_bar), "users",
			    "_Users", icon, widget, TACO_BAR_TOP);
	taco_bar_set_page_sensitive (TACO_BAR (gui.discussion_bar), "users", FALSE);

	// Dummy history page
	widget = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (widget),
					     GTK_SHADOW_IN);
	icon = gtk_image_new_from_icon_name ("xchat-gnome-log",
					     GTK_ICON_SIZE_BUTTON);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (widget),
				        GTK_POLICY_NEVER,
				        GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (widget), gtk_tree_view_new());
	taco_bar_pack_page (TACO_BAR (gui.discussion_bar), "history",
			    "_History", icon, widget, TACO_BAR_TOP);
//	taco_bar_set_page_sensitive (TACO_BAR (gui.taco_bar), "events", FALSE);
	

	// It's all good to go now
	taco_bar_set_visible_page (TACO_BAR (gui.discussion_bar), "users");
	taco_bar_set_visible_page (TACO_BAR (gui.discussion_bar), "users");
	taco_bar_set_default_page (TACO_BAR (gui.server_bar), "connections");
	taco_bar_set_default_page (TACO_BAR (gui.server_bar), "connections");
	gtk_widget_show (gui.server_bar);
	gtk_widget_show (gui.discussion_bar);
}

void
initialize_main_window (void)
{
	GtkWidget *close, *menu_vbox, *widget;
	GtkSizeGroup *group;
	GtkAction *action;
	gchar *path;

	gui.main_window = glade_xml_get_widget (gui.xml, "xchat-gnome");
	g_signal_connect (G_OBJECT (gui.main_window), "delete-event",    G_CALLBACK (on_main_window_close),     NULL);
	g_signal_connect (G_OBJECT (gui.main_window), "focus-in-event",  G_CALLBACK (on_main_window_focus_in),  NULL);
	g_signal_connect (G_OBJECT (gui.main_window), "configure-event", G_CALLBACK (on_main_window_configure), NULL);

	/* hook up the menus */
	gui.action_group = gtk_action_group_new ("MenuAction");
	gtk_action_group_set_translation_domain (gui.action_group, NULL);
	gtk_action_group_add_actions (gui.action_group, action_entries,
	                              G_N_ELEMENTS (action_entries), NULL);

	gtk_ui_manager_insert_action_group (gui.manager, gui.action_group, 0);

	menu_vbox = glade_xml_get_widget (gui.xml, "menu_vbox");
	g_signal_connect (gui.manager, "add-widget", G_CALLBACK (on_add_widget), menu_vbox);

	/* load the menus */
	path = locate_data_file ("xchat-gnome-ui.xml");
	gtk_ui_manager_add_ui_from_file (gui.manager, path, NULL);
	g_free (path);

	/* hook up accelerators */
	gtk_window_add_accel_group (GTK_WINDOW (gui.main_window), gtk_ui_manager_get_accel_group (gui.manager));

	close = glade_xml_get_widget (gui.xml, "close discussion");
	action = gtk_action_group_get_action (gui.action_group, "DiscussionClose");
	gtk_action_connect_proxy (action, close);

#define GW(name) ((gui.name) = glade_xml_get_widget (gui.xml, #name))
	GW(conversation_panel);
	GW(find_bar);
	GW(status_bar);
	GW(text_entry);
	GW(topic_hbox);
	GW(topic_label);
	GW(nick_button);
	GW(server_bar);
	GW(discussion_bar);
#undef GW

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

	navigation_tree_add_accels (gui.server_tree, GTK_WINDOW (gui.main_window));

	/* Size group between users button and entry field */
	group = gtk_size_group_new (GTK_SIZE_GROUP_VERTICAL);
	widget = glade_xml_get_widget (gui.xml, "entry hbox");
	gtk_size_group_add_widget (group, widget);
	g_object_unref (group);

	/* connect nickname button */
	gtk_button_set_use_underline (GTK_BUTTON (gui.nick_button), FALSE);
	g_signal_connect (G_OBJECT (gui.nick_button), "clicked",   G_CALLBACK (on_nickname_clicked), NULL);
	g_signal_connect (G_OBJECT (GTK_BIN (gui.nick_button)->child), "style-set", G_CALLBACK (nickname_style_set),  NULL);

	/* And finally, set up all the stuff in the taco bar */
	initialize_taco_bar ();
}

void
run_main_window ()
{
	GtkWidget *pane;
	GConfClient *client;
	int width, height;
	int x, y;
	int h;

	client = gconf_client_get_default ();
	width  = gconf_client_get_int (client, "/apps/xchat/main_window/width",  NULL);
	height = gconf_client_get_int (client, "/apps/xchat/main_window/height", NULL);
	if (width == 0 || height == 0) {
		gtk_window_set_default_size (GTK_WINDOW (gui.main_window), 800, 550);
	} else {
		gtk_window_set_default_size (GTK_WINDOW (gui.main_window), width, height);
	}
	x = gconf_client_get_int (client, "/apps/xchat/main_window/x", NULL);
	y = gconf_client_get_int (client, "/apps/xchat/main_window/y", NULL);
	if (!(x == 0 || y == 0)) {
		gtk_window_move (GTK_WINDOW (gui.main_window), x, y);
	}
	h = gconf_client_get_int (client, "/apps/xchat/main_window/hpane", NULL);
	if(h != 0) {
		GtkWidget *hpane = glade_xml_get_widget (gui.xml, "HPane");
		gtk_paned_set_position (GTK_PANED (hpane), h);
	}
	g_signal_connect (G_OBJECT (gui.main_window), "configure-event", G_CALLBACK (on_resize), NULL);
	pane = glade_xml_get_widget (gui.xml, "HPane");
	g_signal_connect (G_OBJECT (pane), "notify::position", G_CALLBACK (on_hpane_move), NULL);
	g_object_unref (client);

	gtk_widget_show (gui.main_window);
}

void
rename_main_window (gchar *server, gchar *channel)
{
	gchar *new_title;

	if (server == NULL) {
		if (channel && strlen (channel) != 0) {
			gtk_window_set_title (GTK_WINDOW (gui.main_window), channel);
		} else {
			gtk_window_set_title (GTK_WINDOW (gui.main_window), "XChat-GNOME");
		}
		return;
	}
	new_title = g_strconcat (server, ": ", channel, NULL);
	gtk_window_set_title (GTK_WINDOW (gui.main_window), new_title);

	g_free (new_title);
}

static void on_add_widget (GtkUIManager *manager, GtkWidget *menu, GtkWidget *menu_vbox)
{
	gtk_box_pack_start (GTK_BOX (menu_vbox), menu, FALSE, FALSE, 0);
}

static void
on_irc_connect_activate (GtkAction *action, gpointer data)
{
	ConnectDialog *dialog = connect_dialog_new ();
	gtk_widget_show_all (GTK_WIDGET (dialog));
}

void
save_main_window ()
{
	gint x, y;
	GConfClient *client;

	gtk_window_get_position (GTK_WINDOW (gui.main_window), &x, &y);
	client = gconf_client_get_default ();
	gconf_client_set_int (client, "/apps/xchat/main_window/x", x, NULL);
	gconf_client_set_int (client, "/apps/xchat/main_window/y", y, NULL);
	g_object_unref (client);
}

static void
on_main_window_close (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	save_main_window ();
	gui.quit = TRUE;

	gtk_widget_hide (GTK_WIDGET (gui.dcc));
	xchat_exit ();
}

static void
on_irc_quit_activate (GtkAction *action, gpointer data)
{
	save_main_window ();
	gtk_widget_hide (GTK_WIDGET (gui.main_window));
	gtk_widget_hide (GTK_WIDGET (gui.dcc));
	gui.quit = TRUE;
	xchat_exit ();
}

static void
on_edit_cut_activate (GtkAction *action, gpointer data)
{
	gtk_editable_cut_clipboard (GTK_EDITABLE (gui.text_entry));
}

static void
on_edit_copy_activate (GtkAction *action, gpointer data)
{
	if (gtk_editable_get_selection_bounds (GTK_EDITABLE (gui.text_entry), NULL, NULL)) {
		/* There is something selected in the text_entry */
		gtk_editable_copy_clipboard (GTK_EDITABLE (gui.text_entry));
	} else {
		/* Nothing selected, we copy from the conversation panel */
		conversation_panel_copy_selection (CONVERSATION_PANEL (gui.conversation_panel));
	}
}

static void
on_edit_paste_activate (GtkAction *action, gpointer data)
{
	gtk_editable_paste_clipboard (GTK_EDITABLE (gui.text_entry));
}

static void
on_edit_preferences_activate (GtkAction *action, gpointer data)
{
	if (!gui.prefs_dialog) {
		gui.prefs_dialog = preferences_dialog_new ();
		g_object_add_weak_pointer (G_OBJECT (gui.prefs_dialog),
		                           (gpointer *) (&gui.prefs_dialog));
	}

	preferences_dialog_show (gui.prefs_dialog);
}

static void
on_network_reconnect_activate (GtkAction *action, gpointer data)
{
	handle_command (gui.current_session, "reconnect", FALSE);
}

static void
on_network_disconnect_activate (GtkAction *action, gpointer data)
{
	session *s = gui.current_session;
	if (s) {
		s->server->disconnect (s, TRUE, -1);
	}
}

static void
on_irc_downloads_activate (GtkAction *action, gpointer data)
{
	gtk_window_present (GTK_WINDOW (gui.dcc));
}

static void
on_network_channels_activate (GtkAction *action, gpointer data)
{
	create_channel_list_window (gui.current_session, TRUE);
}

static void
on_discussion_users_activate (GtkAction *action, gpointer data)
{
	taco_bar_toggle_page_state (gui.discussion_bar, "users");
}

static void
on_discussion_save_activate (GtkAction *action, gpointer data)
{
	conversation_panel_save_current (CONVERSATION_PANEL (gui.conversation_panel));
}

static void
on_discussion_leave_activate (GtkAction *action, gpointer data)
{
	session *s = gui.current_session;
	if ((s != NULL) && (s->type == SESS_CHANNEL) && (s->channel[0] != '\0')) {
		gchar *text;
		GConfClient *client;

		client = gconf_client_get_default ();
		text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
		if (text == NULL) {
			text = g_strdup (_("Ex-Chat"));
		}
		s->server->p_part (s->server, s->channel, text);
		g_object_unref (client);
		g_free (text);
	}
}

static void
on_close_activate (GtkAction *action, gpointer data)
{
	session *s = gui.current_session;
	if (s == NULL) {
		return;
	}

	switch (s->type) {
	case SESS_CHANNEL:
	{
		GConfClient *client = gconf_client_get_default ();
		gchar *text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
		if (text == NULL) {
			text = g_strdup (_("Ex-Chat"));
		}

		s->server->p_part (s->server, s->channel, text);

		g_object_unref (client);
		g_free (text);

		break;
	}

	case SESS_SERVER:
		s->server->disconnect (s, TRUE, -1);
		break;
	}

	fe_close_window (s);
}

static void
on_discussion_find_activate (GtkAction *action, gpointer data)
{
	find_bar_open (FIND_BAR (gui.find_bar));
}

static void
on_discussion_bans_activate (GtkAction *action, gpointer data)
{
	/* FIXME: implement */
}

static void
on_pgup (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data)
{
	conversation_panel_page_up (CONVERSATION_PANEL (gui.conversation_panel));
}

static void
on_pgdn (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data)
{
	conversation_panel_page_down (CONVERSATION_PANEL (gui.conversation_panel));
}

static void
on_help_contents_activate (GtkAction *action, gpointer data)
{
	GError *error = NULL;

	gnome_help_display_with_doc_id (NULL, "xchat-gnome", "xchat-gnome.xml", NULL, &error);
	if (error) {
		error_dialog (_("Error showing help"), error->message);
		g_error_free (error);
	}
}

static void
on_help_about_activate (GtkAction *action, gpointer data)
{
	show_about_dialog ();
}

static void
nickname_dialog_entry_activated (GtkEntry *entry, GtkDialog *dialog)
{
	gtk_dialog_response (dialog, GTK_RESPONSE_OK);
}

static void
on_nickname_clicked (GtkButton *widget, gpointer user_data)
{
	GtkWidget *dialog;
	GtkWidget *entry, *away;
	gint result;

	if (gui.current_session == NULL) {
		return;
	}
	current_sess = gui.current_session;

	dialog = glade_xml_get_widget (gui.xml, "nickname dialog");
	entry = glade_xml_get_widget (gui.xml, "nickname dialog entry");
	away = glade_xml_get_widget (gui.xml, "nickname dialog away");

	gtk_entry_set_text (GTK_ENTRY (entry), current_sess->server->nick);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (away), current_sess->server->is_away);
	g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (nickname_dialog_entry_activated), dialog);

	result = gtk_dialog_run (GTK_DIALOG (dialog));
	if (result == GTK_RESPONSE_OK) {
		GtkWidget *check_all;
		gboolean all;
		gchar *text, *buf;

		check_all = glade_xml_get_widget (gui.xml, "nickname dialog all");
		all = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check_all));

		/* Nick */
		text = (gchar *) gtk_entry_get_text (GTK_ENTRY (entry));
		if (all) {
			buf = g_strdup_printf ("allserv nick %s", text);
		} else {
			buf = g_strdup_printf ("nick %s", text);
		}
		handle_command (current_sess, buf, FALSE);
		g_free (buf);

		/* Away */
		if (current_sess->server->is_away != gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (away))) {
			if (all) {
				handle_command (current_sess, "allserv away", FALSE);
			} else {
				handle_command (current_sess, "away", FALSE);	
			}
		}
	}
	gtk_widget_hide (dialog);
}

static gboolean
on_resize (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	GConfClient *client;

	client = gconf_client_get_default ();
	gconf_client_set_int (client, "/apps/xchat/main_window/width",  event->width,  NULL);
	gconf_client_set_int (client, "/apps/xchat/main_window/height", event->height, NULL);
	g_object_unref (client);
	return FALSE;
}

static gboolean
on_hpane_move (GtkPaned *widget, GParamSpec *param_spec, gpointer data)
{
	GConfClient *client;
	int pos;

	client = gconf_client_get_default ();
	pos = gtk_paned_get_position (widget);
	gconf_client_set_int (client, "/apps/xchat/main_window/hpane", pos, NULL);
	g_object_unref (client);
	return FALSE;
}

static void
on_discussion_topic_change_activate (GtkButton *widget, gpointer data)
{
	topic_label_change_current (TOPIC_LABEL (gui.topic_label));
}

void
set_nickname (struct server *serv, char *newnick)
{
	GtkLabel *label;

	if (gui.current_session == NULL) {
		return;
	}

	label = GTK_LABEL (GTK_BIN (gui.nick_button)->child);

	if (serv == NULL) {
		gtk_label_set_text (label, "");
		return;
	}

	if (serv == gui.current_session->server) {
		if (newnick == NULL) {
			gtk_label_set_text (label, serv->nick);
		} else {
			gtk_label_set_text (label, newnick);
		}
		set_nickname_color (serv);
	}
}

void
set_nickname_color (struct server *serv)
{
	if (gui.current_session == NULL) {
		return;
	}

	if (serv == gui.current_session->server) {
		GtkLabel *label;
		PangoAttribute *attr;
		PangoAttrList *l;
		GtkStyle *style;
		GdkColor *color;

		l = pango_attr_list_new ();
		label = GTK_LABEL (GTK_BIN (gui.nick_button)->child);

		style = gtk_widget_get_style (GTK_WIDGET (label));

		if (serv->is_away) {
			color = &(style->fg[GTK_STATE_INSENSITIVE]);
		} else {
			color = &(style->fg[GTK_STATE_NORMAL]);
		}
		attr = pango_attr_foreground_new (color->red, color->green, color->blue);

		attr->start_index = 0;
		attr->end_index = G_MAXUINT;
		pango_attr_list_insert (l, attr);
		gtk_label_set_attributes (label, l);

		pango_attr_list_unref (l);
	}
}

static gboolean
on_main_window_focus_in (GtkWidget * widget, GdkEventFocus * event, gpointer data)
{
	conversation_panel_check_marker_visibility (CONVERSATION_PANEL (gui.conversation_panel));
	gtk_window_set_urgency_hint (GTK_WINDOW (widget), FALSE);
	return FALSE;
}

static gboolean
on_main_window_configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	conversation_panel_queue_tdraw (CONVERSATION_PANEL (gui.conversation_panel));
	return FALSE;
}

static void
nickname_style_set (GtkWidget *button, GtkStyle *previous_style, gpointer data)
{
	if (gui.current_session == NULL) {
		return;
	}

	set_nickname_color (gui.current_session->server);
}