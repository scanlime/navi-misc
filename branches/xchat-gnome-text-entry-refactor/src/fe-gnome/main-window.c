/*
 * main-window.c - main GUI window functions
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

#include <config.h>
#include <glib/gi18n.h>
#include <string.h>
#include <libgnome/libgnome.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#ifdef HAVE_LIBSEXY
#include <libsexy/sexy-url-label.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "about.h"
#include "channel-list.h"
#include "connect-dialog.h"
#include "gui.h"
#include "main-window.h"
#include "navigation-tree.h"
#include "palette.h"
#include "preferences-dialog.h"
#include "preferences.h"
#include "text-entry.h"
#include "textgui.h"
#include "userlist-gui.h"
#include "util.h"
#include "../common/xchatc.h"
#include "../common/outbound.h"
#include "../common/fe.h"

static textentry *last_search_position = NULL;

static void on_main_window_close (GtkWidget *widget, GdkEvent *event, gpointer data);
static void on_discussion_jump_activate (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void on_pgup (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void on_pgdn (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);

/* action callbacks */
static void on_irc_connect_activate (GtkAction *action, gpointer data);
static void on_irc_downloads_activate (GtkAction *action, gpointer data);
static void on_irc_quit_activate (GtkAction *action, gpointer data);
static void on_edit_cut_activate (GtkAction *action, gpointer data);
static void on_edit_copy_activate (GtkAction *action, gpointer data);
static void on_edit_paste_activate (GtkAction *action, gpointer data);
static void on_edit_clear_activate (GtkAction *action, gpointer data);
static void on_edit_preferences_activate (GtkAction *action, gpointer data);
static void on_network_information_activate (GtkAction *action, gpointer data);
static void on_network_reconnect_activate (GtkAction *action, gpointer data);
static void on_network_disconnect_activate (GtkAction *action, gpointer data);
static void on_network_close_activate (GtkAction *action, gpointer data);
static void on_network_channels_activate (GtkAction *action, gpointer data);
static void on_discussion_save_activate (GtkAction *action, gpointer data);
static void on_discussion_leave_activate (GtkAction *action, gpointer data);
static void on_discussion_close_activate (GtkAction *action, gpointer data);
static void on_discussion_find_activate (GtkAction *action, gpointer data);
static void on_discussion_clear_window_activate (GtkAction *action, gpointer data);
static void on_discussion_bans_activate (GtkAction *action, gpointer data);
static void on_discussion_topic_change_activate (GtkButton *widget, gpointer data);
static void on_discussion_users_activate (GtkAction *action, gpointer data);
static void on_go_previous_network_activate (GtkAction *action, gpointer data);
static void on_go_next_network_activate (GtkAction *action, gpointer data);
static void on_go_previous_discussion_activate (GtkAction *action, gpointer data);
static void on_go_next_discussion_activate (GtkAction *action, gpointer data);
static void on_help_contents_activate (GtkAction *action, gpointer data);
static void on_help_about_activate (GtkAction *action, gpointer data);
static void on_nickname_clicked (GtkButton *widget, gpointer user_data);
static void on_users_toggled (GtkToggleButton *widget, gpointer user_data);

static void on_add_widget (GtkUIManager *manager, GtkWidget *menu, GtkWidget *menu_vbox);

static void on_expand_topic (GtkExpander *expander, gpointer data);

static gboolean on_resize (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
static gboolean on_hpane_move (GtkPaned *widget, GParamSpec *param_spec, gpointer data);

void setup_menu_item (GConfClient *client, GtkActionEntry *entry);

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
	{ "IRCConnect",   GTK_STOCK_CONNECT, N_("_Connect"),        "<control>N", NULL, G_CALLBACK (on_irc_connect_activate) },
	{ "IRCDownloads", NULL,              N_("_File Transfers"), "<alt>F",     NULL, G_CALLBACK (on_irc_downloads_activate) },
	{ "IRCQuit",      GTK_STOCK_QUIT,    N_("_Quit"),           "<control>Q", NULL, G_CALLBACK (on_irc_quit_activate) },

	/* Edit menu */
	{ "EditCut",         GTK_STOCK_CUT,         N_("Cu_t"),         "<control>X", NULL, G_CALLBACK (on_edit_cut_activate) },
	{ "EditCopy",        GTK_STOCK_COPY,        N_("_Copy"),        "<control>C", NULL, G_CALLBACK (on_edit_copy_activate) },
	{ "EditPaste",       GTK_STOCK_PASTE,       N_("_Paste"),       "<control>V", NULL, G_CALLBACK (on_edit_paste_activate) },
	{ "EditClear",       GTK_STOCK_CLEAR,       N_("C_lear"),       "",           NULL, G_CALLBACK (on_edit_clear_activate) },
	{ "EditPreferences", GTK_STOCK_PREFERENCES, N_("Prefere_nces"), "",           NULL, G_CALLBACK (on_edit_preferences_activate) },

	/* Network menu */
	{ "NetworkInformation", GTK_STOCK_DIALOG_INFO, N_("_Information"), "",                  NULL, G_CALLBACK (on_network_information_activate) },
	{ "NetworkReconnect",   GTK_STOCK_REFRESH,     N_("_Reconnect"),   "<control>R",        NULL, G_CALLBACK (on_network_reconnect_activate) },
	{ "NetworkDisconnect",  GTK_STOCK_STOP,        N_("_Disconnect"),  "",                  NULL, G_CALLBACK (on_network_disconnect_activate) },
	{ "NetworkClose",       GTK_STOCK_CLOSE,       N_("_Close"),       "<shift><control>W", NULL, G_CALLBACK (on_network_close_activate) },
	{ "NetworkChannels",    GTK_STOCK_INDEX,       N_("_Channels..."), "<alt>C",            NULL, G_CALLBACK (on_network_channels_activate) },

	/* Discussion menu */
	{ "DiscussionSave",        GTK_STOCK_SAVE,           N_("_Save Transcript"), "<control>S", NULL, G_CALLBACK (on_discussion_save_activate) },
	{ "DiscussionLeave",       GTK_STOCK_QUIT,           N_("_Leave"),           "",           NULL, G_CALLBACK (on_discussion_leave_activate) },
	{ "DiscussionClose",       GTK_STOCK_CLOSE,          N_("Cl_ose"),           "<control>W", NULL, G_CALLBACK (on_discussion_close_activate) },
	{ "DiscussionFind",        GTK_STOCK_FIND,           N_("_Find"),            "<control>F", NULL, G_CALLBACK (on_discussion_find_activate) },
	{ "DiscussionClearWindow", GTK_STOCK_CLEAR,          N_("_Clear Window"),    "<control>L", NULL, G_CALLBACK (on_discussion_clear_window_activate) },
	{ "DiscussionChangeTopic", GTK_STOCK_REFRESH,        N_("Change _Topic"),    "<alt>T",     NULL, G_CALLBACK (on_discussion_topic_change_activate) },
	{ "DiscussionBans",        GTK_STOCK_DIALOG_WARNING, N_("_Bans..."),         "<alt>B",     NULL, G_CALLBACK (on_discussion_bans_activate) },
	{ "DiscussionUsers",       NULL,                     N_("_Users"),           "<control>U", NULL, G_CALLBACK (on_discussion_users_activate) },

	/* Go menu */
	{ "GoPreviousNetwork",    GTK_STOCK_GOTO_FIRST, N_("Pre_vious Network"),    "<control>Up",   NULL, G_CALLBACK (on_go_previous_network_activate) },
	{ "GoNextNetwork",        GTK_STOCK_GOTO_LAST,  N_("Nex_t Network"),        "<control>Down", NULL, G_CALLBACK (on_go_next_network_activate) },
	{ "GoPreviousDiscussion", GTK_STOCK_GO_BACK,    N_("_Previous Discussion"), "<alt>Up",       NULL, G_CALLBACK (on_go_previous_discussion_activate) },
	{ "GoNextDiscussion",     GTK_STOCK_GO_FORWARD, N_("_Next Discussion"),     "<alt>Down",     NULL, G_CALLBACK (on_go_next_discussion_activate) },

	/* Help menu */
	{ "HelpContents", GTK_STOCK_HELP,  N_("_Contents"), "",   NULL, G_CALLBACK (on_help_contents_activate) },
	{ "HelpAbout",    GTK_STOCK_ABOUT, N_("_About"),    "",   NULL, G_CALLBACK (on_help_about_activate) },
};

void
save_transcript (void)
{
	GtkWidget *file_chooser;

	file_chooser = gtk_file_chooser_dialog_new (_("Save Transcript"),
	                                            GTK_WINDOW (gui.main_window),
	                                            GTK_FILE_CHOOSER_ACTION_SAVE,
	                                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                                            GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
	                                            NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (file_chooser), TRUE);
	gtk_dialog_set_default_response (GTK_DIALOG (file_chooser), GTK_RESPONSE_ACCEPT);

	if (gtk_dialog_run (GTK_DIALOG (file_chooser)) == GTK_RESPONSE_ACCEPT) {
		gchar *filename;
		GIOChannel *file;
		GError *error = NULL;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_chooser));
		file = g_io_channel_new_file (filename, "w", &error);
		if (error) {
			gchar *header = g_strdup_printf (_("Error saving %s"), filename);
			error_dialog (header, error->message);
			g_free (header);
			g_error_free (error);
		} else {
			gint fd = g_io_channel_unix_get_fd (file);
			gtk_xtext_save (gui.xtext, fd);
			g_io_channel_shutdown (file, TRUE, &error);

			if (error) {
				gchar *header = g_strdup_printf (_("Error saving %s"), filename);
				error_dialog (header, error->message);
				g_free (header);
				g_error_free (error);
			}
		}
		g_free (filename);
	}
	gtk_widget_destroy (file_chooser);
}

void
close_find_button (GtkWidget *button, gpointer data)
{
	GtkWidget *widget;
	widget = glade_xml_get_widget (gui.xml, "find hbox");
	gtk_widget_hide (widget);
	gtk_widget_grab_focus (gui.text_entry);

	widget = glade_xml_get_widget (gui.xml, "find_status_label");
	gtk_label_set_text (GTK_LABEL (widget), "");

	gtk_xtext_selection_clear_full (gui.xtext->buffer);
	gtk_xtext_refresh (gui.xtext, TRUE);
}

static gboolean
close_find_key (GtkWidget *entry, GdkEventKey *event, gpointer data)
{
	if (event->keyval != GDK_Escape)
		return FALSE;
	close_find_button (NULL, NULL);
	return FALSE;
}

static void
find_next (GtkWidget *entry, gpointer data)
{
	GtkWidget *info;
	const guchar *text = gtk_entry_get_text (GTK_ENTRY (entry));
	textentry *position;
	gboolean reverse = (gboolean) GPOINTER_TO_UINT(data);

	position = gtk_xtext_search (GTK_XTEXT (gui.xtext), text, last_search_position, FALSE, reverse);

	info = glade_xml_get_widget (gui.xml, "find_status_label");
	if (position == NULL && (last_search_position != NULL)) {
		if (reverse) {
			position = gtk_xtext_search (GTK_XTEXT (gui.xtext), text, NULL, FALSE, reverse);
			gtk_label_set_markup (GTK_LABEL (info), "<span foreground=\"grey\">Reached beginning, continuing from bottom</span>");
		} else {
			position = gtk_xtext_search (GTK_XTEXT (gui.xtext), text, NULL, FALSE, reverse);
			gtk_label_set_markup (GTK_LABEL (info), "<span foreground=\"grey\">Reached end, continuing from top</span>");
		}
	} else if (position == NULL) {
		gtk_label_set_markup (GTK_LABEL (info), "<span foreground=\"grey\">Search string not found</span>");
	} else {
		gtk_label_set_text (GTK_LABEL (info), "");
	}
	last_search_position = position;
}

static void
find_button_next (GtkButton *button, GtkWidget *entry)
{
	find_next (entry, GUINT_TO_POINTER(FALSE));
}

static void
find_button_prev (GtkButton *button, GtkWidget *entry)
{
	find_next (entry, GUINT_TO_POINTER(TRUE));
}

static void
clear_find (GtkWidget *entry, gpointer data)
{
	last_search_position = NULL;
}

#ifdef HAVE_LIBSEXY
static void
url_activated (GtkWidget *url_label, const char *url, gpointer data)
{
	fe_open_url (url);
}
#endif

void
initialize_main_window (void)
{
	GtkWidget *entrybox, *topicbox, *close, *menu_vbox, *widget, *widget2;
	GtkSizeGroup *group;
	GError *error = NULL;

	gui.main_window = GNOME_APP (glade_xml_get_widget (gui.xml, "xchat-gnome"));
	g_signal_connect (G_OBJECT (gui.main_window), "delete-event",
	                  G_CALLBACK (on_main_window_close), NULL);

	/* hook up the menus */
	gui.action_group = gtk_action_group_new ("MenuAction");
	gtk_action_group_set_translation_domain (gui.action_group, NULL);
	gtk_action_group_add_actions (gui.action_group, action_entries,
	                              G_N_ELEMENTS (action_entries), NULL);
//	setup_menu ();
//	initialize_gconf_accels();

	gui.manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (gui.manager, gui.action_group, 0);

	menu_vbox = glade_xml_get_widget (gui.xml, "menu_vbox");
	g_signal_connect (gui.manager, "add-widget", G_CALLBACK (on_add_widget), menu_vbox);

	/* load the menus */
	gtk_ui_manager_add_ui_from_file (gui.manager, "../../data/xchat-gnome-ui.xml", &error);
	if (error != NULL) {
		g_clear_error (&error);
		gtk_ui_manager_add_ui_from_file (gui.manager, XCHATSHAREDIR "/xchat-gnome-ui.xml", &error);
		if (error != NULL)
		{
			g_clear_error (&error);
			g_warning ("Couldn't load the menus!\n");
		}
	}

	/* hook up accelerators */
	gtk_window_add_accel_group (GTK_WINDOW (gui.main_window), gtk_ui_manager_get_accel_group (gui.manager));

	entrybox = glade_xml_get_widget (gui.xml, "entry hbox");
	gui.text_entry = text_entry_new ();
	gtk_box_pack_start (GTK_BOX (entrybox), gui.text_entry, TRUE, TRUE, 0);
	gtk_widget_show (gui.text_entry);

	close = glade_xml_get_widget (gui.xml, "close discussion");
	g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (on_discussion_close_activate), NULL);
	topicbox = glade_xml_get_widget (gui.xml, "topic hbox");

#ifdef HAVE_LIBSEXY
	gui.topic_label = sexy_url_label_new ();
	g_signal_connect (gui.topic_label, "url_activated", G_CALLBACK (url_activated), NULL);
#else
	gui.topic_label = gtk_label_new("");
#endif
	gtk_widget_show (gui.topic_label);

	gtk_box_pack_start (GTK_BOX (topicbox), gui.topic_label, TRUE, TRUE, 0);
	gtk_box_reorder_child (GTK_BOX (topicbox), gui.topic_label, 1);
	gtk_label_set_selectable (GTK_LABEL (gui.topic_label), TRUE);

	gui.topic_expander = GTK_EXPANDER (gtk_expander_new (NULL));
	gtk_widget_show (GTK_WIDGET (gui.topic_expander));
	gtk_box_pack_start (GTK_BOX (topicbox), GTK_WIDGET (gui.topic_expander), FALSE, TRUE, 0);
	gtk_box_reorder_child (GTK_BOX (topicbox), GTK_WIDGET (gui.topic_expander), 0);
	gtk_expander_set_expanded (GTK_EXPANDER (gui.topic_expander), FALSE);
	gtk_expander_set_use_markup (gui.topic_expander, TRUE);
	g_signal_connect (G_OBJECT (gui.topic_expander), "activate", G_CALLBACK (on_expand_topic), NULL);
	gtk_label_set_ellipsize (GTK_LABEL (gui.topic_label), PANGO_ELLIPSIZE_END);

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
		for (i = 0; i < 9; i++) {
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
		closure = g_cclosure_new (G_CALLBACK (on_go_next_discussion_activate), NULL, NULL);
		gtk_accel_group_connect (discussion_accel, gdk_keyval_from_name ("equal"), GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);
		g_closure_unref (closure);

		/* alt-- */
		closure = g_cclosure_new (G_CALLBACK (on_go_previous_discussion_activate), NULL, NULL);
		gtk_accel_group_connect (discussion_accel, gdk_keyval_from_name ("minus"), GDK_MOD1_MASK, GTK_ACCEL_VISIBLE, closure);
		g_closure_unref (closure);

		/* We've had a couple of calls to hook up Ctrl-Alt-PgUp and
		 * Ctrl-Alt-PgDown to discussion navigation. As far as I can
		 * tell this is not HIG compliant, but for the time being we'll
		 * put it in. It's easy enough to delete it later.
		 */
		closure = g_cclosure_new (G_CALLBACK (on_go_next_discussion_activate), NULL, NULL);
		gtk_accel_group_connect (discussion_accel, GDK_Page_Down,
				GDK_MOD1_MASK | GDK_CONTROL_MASK , GTK_ACCEL_VISIBLE, closure);

		g_closure_unref (closure);

		closure = g_cclosure_new (G_CALLBACK (on_go_previous_discussion_activate), NULL, NULL);
		gtk_accel_group_connect (discussion_accel, GDK_Page_Up,
				GDK_MOD1_MASK | GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, closure);

		g_closure_unref (closure);

		/* Add the accelgroup to the main window. */
		gtk_window_add_accel_group (GTK_WINDOW (gui.main_window), discussion_accel);
	}

	/* setup find stuff */
	widget = glade_xml_get_widget (gui.xml, "find entry");
	g_signal_connect (G_OBJECT (widget), "activate", G_CALLBACK (find_next), GUINT_TO_POINTER(FALSE));
	g_signal_connect (G_OBJECT (widget), "key-press-event", G_CALLBACK (close_find_key), NULL);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (clear_find), NULL);
	widget2 = glade_xml_get_widget (gui.xml, "find_next_button");
	g_signal_connect (G_OBJECT (widget2), "clicked", G_CALLBACK (find_button_next), widget);
	widget2 = glade_xml_get_widget (gui.xml, "find_prev_button");
	g_signal_connect (G_OBJECT (widget2), "clicked", G_CALLBACK (find_button_prev), widget);
	widget = glade_xml_get_widget (gui.xml, "find close button");
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (close_find_button), NULL);

	/* Size group between users button and entry field */
	group = gtk_size_group_new (GTK_SIZE_GROUP_VERTICAL);
	gui.userlist_toggle = glade_xml_get_widget (gui.xml, "userlist_toggle");
	g_signal_connect (G_OBJECT (gui.userlist_toggle), "toggled", G_CALLBACK (on_users_toggled), NULL);
	if (g_file_test ("../../data/users.png", G_FILE_TEST_EXISTS))
		gtk_button_set_image (GTK_BUTTON (gui.userlist_toggle), gtk_image_new_from_file ("../../data/users.png"));
	else
		gtk_button_set_image (GTK_BUTTON (gui.userlist_toggle), gtk_image_new_from_file (XCHATSHAREDIR "/users.png"));
	gtk_size_group_add_widget (group, gui.userlist_toggle);
	widget = glade_xml_get_widget (gui.xml, "entry hbox");
	gtk_size_group_add_widget (group, widget);
	g_object_unref (group);

	/* connect nickname button */
	widget = glade_xml_get_widget (gui.xml, "nickname");
	gtk_button_set_use_underline (GTK_BUTTON (widget), FALSE);
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (on_nickname_clicked), NULL);
}

void
run_main_window ()
{
	GtkWidget *pane, *widget;
	GConfClient *client;
	int width, height;
	int h;

	client = gconf_client_get_default ();
	width  = gconf_client_get_int (client, "/apps/xchat/main_window/width",  NULL);
	height = gconf_client_get_int (client, "/apps/xchat/main_window/height", NULL);
	if(width == 0 || height == 0)
		gtk_window_set_default_size (GTK_WINDOW (gui.main_window), 800, 550);
	else
		gtk_window_set_default_size (GTK_WINDOW (gui.main_window), width, height);
	h = gconf_client_get_int (client, "/apps/xchat/main_window/hpane", NULL);
	if(h != 0) {
		GtkWidget *hpane = glade_xml_get_widget (gui.xml, "HPane");
		gtk_paned_set_position (GTK_PANED (hpane), h);
	}
	g_signal_connect (G_OBJECT (gui.main_window), "configure-event", G_CALLBACK (on_resize), NULL);
	pane = glade_xml_get_widget (gui.xml, "HPane");
	g_signal_connect (G_OBJECT (pane), "notify::position", G_CALLBACK (on_hpane_move), NULL);
	g_object_unref (client);

	gtk_widget_show (GTK_WIDGET (gui.main_window));

	/* Temporarily disable menu items */
	widget = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/NetworkMenu/NetworkInformationItem");
	gtk_widget_set_sensitive (widget, FALSE);
	widget = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/DiscussionMenu/DiscussionBansItem");
	gtk_widget_set_sensitive (widget, FALSE);
}

void
rename_main_window (gchar *server, gchar *channel)
{
	gchar *new_title;

	if (server == NULL) {
		gtk_window_set_title (GTK_WINDOW (gui.main_window), channel);
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

static void
on_main_window_close (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gui.quit = TRUE;
	gtk_widget_hide (GTK_WIDGET (gui.dcc));
	userlist_gui_hide ();
	xchat_exit ();
}

static void
on_irc_quit_activate (GtkAction *action, gpointer data)
{
	gtk_widget_hide (GTK_WIDGET (gui.main_window));
	gtk_widget_hide (GTK_WIDGET (gui.dcc));
	userlist_gui_hide ();
	gui.quit = TRUE;
	xchat_exit ();
}

static void
on_edit_cut_activate (GtkAction *action, gpointer data)
{
	gtk_editable_cut_clipboard(GTK_EDITABLE(gui.text_entry));
}

static void
on_edit_copy_activate (GtkAction *action, gpointer data)
{
	gtk_editable_copy_clipboard(GTK_EDITABLE(gui.text_entry));
}

static void
on_edit_paste_activate (GtkAction *action, gpointer data)
{
	gtk_editable_paste_clipboard(GTK_EDITABLE(gui.text_entry));
}

static void
on_edit_clear_activate (GtkAction *action, gpointer data)
{
	gtk_editable_delete_selection(GTK_EDITABLE(gui.text_entry));
}

static void
on_edit_preferences_activate (GtkAction *action, gpointer data)
{
	PreferencesDialog *dialog;

	dialog = preferences_dialog_new ();
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
on_network_information_activate (GtkAction *action, gpointer data)
{
	/* FIXME: implement */
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
	if (s)
		s->server->disconnect (s, TRUE, -1);
}

static void
on_network_close_activate (GtkAction *actoin, gpointer data)
{
	GtkTreeIter parent, *iter;
	session *sess = gui.current_session;
	if (sess == NULL)
		return;

	iter = navigation_model_get_unsorted_iter (gui.tree_model, sess);

	sess->server->disconnect (sess, TRUE, -1);

	if (gtk_tree_model_iter_parent (GTK_TREE_MODEL (gui.tree_model->store), &parent, iter)) {
		gtk_tree_store_remove (gui.tree_model->store, &parent);
	} else {
		gtk_tree_store_remove (gui.tree_model->store, iter);
	}

	gtk_tree_iter_free (iter);
}

static void
on_irc_downloads_activate (GtkAction *action, gpointer data)
{
	gtk_window_present (GTK_WINDOW (gui.dcc));
}

static void
on_network_channels_activate (GtkAction *action, gpointer data)
{
	create_channel_list (gui.current_session);
}

static void
on_discussion_users_activate (GtkAction *action, gpointer data)
{
	userlist_gui_show ();
}

static void
on_discussion_save_activate (GtkAction *action, gpointer data)
{
	save_transcript ();
}

static void
on_discussion_leave_activate (GtkAction *action, gpointer data)
{
	session *s = gui.current_session;
	if (s->type == SESS_CHANNEL) {
		gchar *text;
		GConfClient *client;

		client = gconf_client_get_default ();
		text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
		if (text == NULL)
			text = g_strdup ("Ex-Chat");
		s->server->p_part (s->server, s->channel, text);
		g_object_unref (client);
		g_free (text);
	}
}

static void
on_discussion_close_activate (GtkAction *action, gpointer data)
{
	session *s = gui.current_session;
	if (s == NULL)
		return;
/*	navigation_tree_select_next_channel (gui.server_tree, TRUE);*/
	if (s->type == SESS_CHANNEL) {
		gchar *text;
		GConfClient *client;

		client = gconf_client_get_default ();
		text = gconf_client_get_string (client, "/apps/xchat/irc/partmsg", NULL);
		if (text == NULL)
			text = g_strdup ("Ex-Chat");
		s->server->p_part (s->server, s->channel, text);
		g_object_unref (client);
		g_free (text);
	}
	fe_close_window (s);
	text_gui_remove_text_buffer (s);
}

static void
on_discussion_find_activate (GtkAction *action, gpointer data)
{
	GtkWidget *widget;

	widget = glade_xml_get_widget (gui.xml, "find hbox");
	gtk_widget_show (widget);
	widget = glade_xml_get_widget (gui.xml, "find entry");
	gtk_widget_grab_focus (widget);
}

static void
on_discussion_clear_window_activate (GtkAction *action, gpointer data)
{
	session *s = gui.current_session;
	clear_buffer (s);
}

static void
on_discussion_bans_activate (GtkAction *action, gpointer data)
{
	/* FIXME: implement */
}

static void
on_go_previous_network_activate (GtkAction *action, gpointer data)
{
	navigation_tree_select_prev_network (gui.server_tree);
}

static void
on_go_next_network_activate(GtkAction *action, gpointer data)
{
	navigation_tree_select_next_network (gui.server_tree);
}

static void
on_go_previous_discussion_activate (GtkAction *action, gpointer data)
{
	navigation_tree_select_prev_channel (gui.server_tree);
}

static void
on_go_next_discussion_activate (GtkAction *action, gpointer data)
{
	navigation_tree_select_next_channel (gui.server_tree);
}

static void
on_discussion_jump_activate (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data)
{
	navigation_tree_select_nth_channel (gui.server_tree, GPOINTER_TO_INT (data));
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
on_help_contents_activate (GtkAction *action, gpointer data)
{
	GError *error = NULL;

	gnome_help_display_with_doc_id (NULL, NULL, "xchat-gnome.xml", NULL, &error);
	if (error) {
		error_dialog ("Error showing help", error->message);
		g_error_free (error);
	}
}

static void
on_help_about_activate (GtkAction *action, gpointer data)
{
	show_about_dialog ();
}

static void
nickname_get_str_response (GtkDialog *dialog, gint arg1, gpointer entry)
{
	gchar *text, *buf;

	text = (gchar *) gtk_entry_get_text (GTK_ENTRY (entry));

	if (arg1 == GTK_RESPONSE_ACCEPT || arg1 == GTK_RESPONSE_OK) {
		buf = g_strdup_printf ("nick %s", text);
		handle_command (current_sess, buf, FALSE);
		g_free (buf);
	}
	gtk_widget_hide_all (GTK_WIDGET (dialog));
}

static void
nickname_str_enter (GtkWidget *entry, GtkWidget *dialog)
{
	gtk_dialog_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);
}

static void
on_nickname_clicked (GtkButton *widget, gpointer user_data)
{
	GtkWidget *dialog;
	GtkWidget *entry;

	if (!current_sess)
		return;

	dialog = glade_xml_get_widget (gui.xml, "nickname dialog");
	entry = glade_xml_get_widget (gui.xml, "nickname dialog entry");

	gtk_entry_set_text (GTK_ENTRY (entry), current_sess->server->nick);
	g_signal_connect (G_OBJECT (entry), "activate",
						 	G_CALLBACK (nickname_str_enter),
						 	dialog);
	g_signal_connect (G_OBJECT (dialog), "response",
						   G_CALLBACK (nickname_get_str_response),
						   entry);

	gtk_widget_show_all (dialog);
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
	GladeXML *xml = NULL;
	GtkWidget *dialog;
	GtkWidget *entry;
	gint response;
	GtkTextBuffer *buffer;
	gchar *title;

	if (gui.current_session == NULL)
		return;

	if (g_file_test ("../../topic-change.glade", G_FILE_TEST_EXISTS))
		xml = glade_xml_new ("../../topic-change.glade", NULL, NULL);
	if (!xml)
		xml = glade_xml_new (XCHATSHAREDIR "/topic-change.glade", NULL, NULL);
	if (!xml) {
		error_dialog (_("Could not load topic-change.glade!"), _("Your installation is broken"));
		return;
	}

	dialog = glade_xml_get_widget (xml, "topic change");
	entry = glade_xml_get_widget (xml, "topic entry box");

	title = g_strdup_printf ("Changing topic for %s", gui.current_session->channel);
	gtk_window_set_title (GTK_WINDOW (dialog), title);
	g_free (title);

	buffer = gtk_text_buffer_new (NULL);
	gtk_text_view_set_buffer (GTK_TEXT_VIEW (entry), buffer);
	gtk_text_buffer_set_text (buffer, gui.current_session->topic, -1);

	response = gtk_dialog_run (GTK_DIALOG (dialog));

	if (response == GTK_RESPONSE_OK) {
		GtkTextIter start;
		GtkTextIter end;
		gchar *newtopic;

		gtk_text_buffer_get_start_iter (buffer, &start);
		gtk_text_buffer_get_end_iter (buffer, &end);
		gtk_widget_hide (dialog);
		gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (entry), GTK_WRAP_NONE);
		newtopic = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
		gui.current_session->server->p_topic(gui.current_session->server, gui.current_session->channel, newtopic);
		g_free (newtopic);
	}

	gtk_widget_destroy (dialog);
	g_object_unref (xml);

	/* send focus back to the text entry */
	gtk_widget_grab_focus (gui.text_entry);
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

static void
on_expand_topic (GtkExpander *expander, gpointer data)
{
	if (gtk_expander_get_expanded (gui.topic_expander)) {
		gtk_label_set_ellipsize(GTK_LABEL (gui.topic_label), PANGO_ELLIPSIZE_END);
		gtk_label_set_line_wrap (GTK_LABEL (gui.topic_label), FALSE);
	} else {
		gtk_label_set_ellipsize(GTK_LABEL (gui.topic_label), PANGO_ELLIPSIZE_NONE);
		gtk_label_set_line_wrap (GTK_LABEL (gui.topic_label), TRUE);
	}
}

static void
on_users_toggled (GtkToggleButton *widget, gpointer user_data)
{
	gboolean toggled;

	toggled = gtk_toggle_button_get_active (widget);

	if (toggled)
		userlist_gui_show ();
	else
		userlist_gui_hide ();
}
