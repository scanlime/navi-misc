/*
 * main_window.c - main GUI window functions
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
#include "error_dialog.h"

#ifdef HAVE_GTKSPELL
#include <gtkspell/gtkspell.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gtk/gtkstock.h>
#include <gtk/gtkaction.h>
#include <gtk/gtkactiongroup.h>
#include <gtk/gtkuimanager.h>

static void *last_search_position = NULL;

static void on_main_window_close (GtkWidget *widget, GdkEvent *event, gpointer data);
static void on_discussion_jump_activate (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void on_discussion_plus_activate (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
static void on_discussion_minus_activate (GtkAccelGroup *accelgroup, GObject *arg1, guint arg2, GdkModifierType arg3, gpointer data);
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
static void on_insert_color_code_activate (GtkAction *action, gpointer data);
static void on_network_information_activate (GtkAction *action, gpointer data);
static void on_network_reconnect_activate (GtkAction *action, gpointer data);
static void on_network_disconnect_activate (GtkAction *action, gpointer data);
static void on_network_channels_activate (GtkAction *action, gpointer data);
static void on_network_users_activate (GtkAction *action, gpointer data);
static void on_discussion_save_activate (GtkAction *action, gpointer data);
static void on_discussion_leave_activate (GtkAction *action, gpointer data);
static void on_discussion_close_activate (GtkAction *action, gpointer data);
static void on_discussion_find_activate (GtkAction *action, gpointer data);
static void on_discussion_find_next_activate (GtkAction *action, gpointer data);
static void on_discussion_clear_window_activate (GtkAction *action, gpointer data);
static void on_discussion_bans_activate (GtkAction *action, gpointer data);
static void on_discussion_topic_change_activate (GtkButton *widget, gpointer data);
static void on_go_previous_network_activate (GtkAction *action, gpointer data);
static void on_go_next_network_activate (GtkAction *action, gpointer data);
static void on_go_previous_discussion_activate (GtkAction *action, gpointer data);
static void on_go_next_discussion_activate (GtkAction *action, gpointer data);
static void on_help_about_activate (GtkAction *action, gpointer data);

static void on_add_widget (GtkUIManager *manager, GtkWidget *menu, GtkWidget *menu_vbox);

#if (GTK_CHECK_VERSION(2,5,0))
static void on_expand_topic (GtkExpander *expander, gpointer data);
#endif

static void on_text_entry_activate (GtkWidget *widget, gpointer data);
static gboolean on_text_entry_key (GtkWidget *widget, GdkEventKey *key, gpointer data);

static gboolean on_resize (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
static gboolean on_vpane_move (GtkPaned *widget, GParamSpec *param_spec, gpointer data);
static gboolean on_hpane_move (GtkPaned *widget, GParamSpec *param_spec, gpointer data);

static void entry_context (GtkEntry *entry, GtkMenu *menu, gpointer user_data);

static GtkActionEntry action_entries [] = {

	/* Toplevel */
	{ "IRC", NULL, _("_IRC") },
	{ "Edit", NULL, _("_Edit") },
	{ "Insert", NULL, _("In_sert") },
	{ "Network", NULL, _("_Network") },
	{ "Discussion", NULL, _("_Discussion") },
	{ "Go", NULL, _("_Go") },
	{ "Help", NULL, _("_Help") },
	{ "PopupAction", NULL, _("") },

	/* IRC menu */
#if (GTK_CHECK_VERSION(2,5,0))
	{ "IRCConnect", GTK_STOCK_CONNECT, _("_Connect"), "<control>N", NULL, G_CALLBACK (on_irc_connect_activate) },
#else
	{ "IRCConnect", NULL, _("_Connect"), "<control>N", NULL, G_CALLBACK (on_irc_connect_activate) },
#endif
	{ "IRCDownloads", NULL, _("_File Transfers"), "<alt>F", NULL, G_CALLBACK (on_irc_downloads_activate) },
	{ "IRCQuit", GTK_STOCK_QUIT, _("_Quit"), "<control>Q", NULL, G_CALLBACK (on_irc_quit_activate) },

	/* Edit menu */
	{ "EditCut", GTK_STOCK_CUT, _("Cu_t"), "<control>X", NULL, G_CALLBACK (on_edit_cut_activate) },
	{ "EditCopy", GTK_STOCK_COPY, _("_Copy"), "<control>C", NULL, G_CALLBACK (on_edit_copy_activate) },
	{ "EditPaste", GTK_STOCK_PASTE, _("_Paste"), "<control>V", NULL, G_CALLBACK (on_edit_paste_activate) },
	{ "EditClear", GTK_STOCK_CLEAR, _("C_lear"), "", NULL, G_CALLBACK (on_edit_clear_activate) },
	{ "EditPreferences", GTK_STOCK_PREFERENCES, _("Prefere_nces"), "", NULL, G_CALLBACK (on_edit_preferences_activate) },

	/* Insert menu */
	{ "InsertColorCode", GTK_STOCK_SELECT_COLOR, _("Color Code..."), "", NULL, G_CALLBACK (on_insert_color_code_activate) },

	/* Network menu */
	{ "NetworkInformation", GTK_STOCK_DIALOG_INFO, _("_Information"), "", NULL, G_CALLBACK (on_network_information_activate) },
	{ "NetworkReconnect", GTK_STOCK_REFRESH, _("_Reconnect"), "<control>R", NULL, G_CALLBACK (on_network_reconnect_activate) },
	{ "NetworkDisconnect", GTK_STOCK_STOP, _("_Disconnect"), "", NULL, G_CALLBACK (on_network_disconnect_activate) },
	{ "NetworkChannels", GTK_STOCK_INDEX, _("_Channels"), "<alt>C", NULL, G_CALLBACK (on_network_channels_activate) },
	{ "NetworkUsers", NULL, _("_Users"), "<alt>U", NULL, G_CALLBACK (on_network_users_activate) },

	/* Discussion menu */
	{ "DiscussionSave", GTK_STOCK_SAVE, _("_Save Transcript"), "<control>S", NULL, G_CALLBACK (on_discussion_save_activate) },
	{ "DiscussionLeave", GTK_STOCK_QUIT, _("_Leave"), "", NULL, G_CALLBACK (on_discussion_leave_activate) },
	{ "DiscussionClose", GTK_STOCK_CLOSE, _("Cl_ose"), "<shift><control>W", NULL, G_CALLBACK (on_discussion_close_activate) },
	{ "DiscussionFind", GTK_STOCK_FIND, _("_Find"), "<control>F", NULL, G_CALLBACK (on_discussion_find_activate) },
	{ "DiscussionFindNext", NULL, _("Find Ne_xt"), "<control>G", NULL, G_CALLBACK (on_discussion_find_next_activate) },
	{ "DiscussionClearWindow", GTK_STOCK_CLEAR, _("_Clear Window"), "<control>L", NULL, G_CALLBACK (on_discussion_clear_window_activate) },
	{ "DiscussionChangeTopic", GTK_STOCK_REFRESH, _("Change _Topic"), "<alt>T", NULL, G_CALLBACK (on_discussion_topic_change_activate) },
	{ "DiscussionBans", GTK_STOCK_DIALOG_WARNING, _("_Bans"), "<alt>B", NULL, G_CALLBACK (on_discussion_bans_activate) },

	/* Go menu */
	{ "GoPreviousNetwork", GTK_STOCK_GOTO_FIRST, _("Pre_vious Network"), "<control>Up", NULL, G_CALLBACK (on_go_previous_network_activate) },
	{ "GoNextNetwork", GTK_STOCK_GOTO_LAST, _("Nex_t Network"), "<control>Down", NULL, G_CALLBACK (on_go_next_network_activate) },
	{ "GoPreviousDiscussion", GTK_STOCK_GO_BACK, _("_Previous Discussion"), "<alt>Up", NULL, G_CALLBACK (on_go_previous_discussion_activate) },
	{ "GoNextDiscussion", GTK_STOCK_GO_FORWARD, _("_Next Discussion"), "<alt>Down", NULL, G_CALLBACK (on_go_next_discussion_activate) },

	/* Help menu */
#if (GTK_CHECK_VERSION(2,5,0))
	{ "HelpAbout", GTK_STOCK_ABOUT, _("_About"), "", NULL, G_CALLBACK (on_help_about_activate) },
#else
	{ "HelpAbout", NULL, _("_About"), "", NULL, G_CALLBACK (on_help_about_activate) },
#endif
};

/*
 * FIXME: for the love of god make this more efficient
 */
GtkActionEntry *
find_action_entry (gchar *name)
{
	guint i;

	for (i = 0; i < G_N_ELEMENTS (action_entries); i++)
		if (strcmp (name, action_entries[i].name) == 0)
			return &action_entries[i];

	return NULL;
}

GtkAction *
find_previous_action (gchar *name)
{
	GList *actions;
	GtkAction *result;

	actions = gtk_action_group_list_actions (gui.action_group);

	while (actions != NULL) {
		if (strcmp(gtk_action_group_get_name(actions -> data), name) == 0) {
			result = actions -> data;
			g_list_free (actions);
			return result;
		}
		actions = actions -> next;
	}

	return NULL;
}

static void
keybinding_key_changed (GConfClient *client, guint cnxn_id, GConfEntry *e, gpointer user_data)
{
	GConfValue *value;
	gchar *name;
	GtkAction *action, *oldaction;
	GtkActionEntry *entry;

	value = gconf_entry_get_value (e);
	name = g_strrstr (gconf_entry_get_key (e), "/");

	if (name == NULL) {
		/* this should never, ever be able to happen.. */
		g_warning ("eek, gconf snafu");
		return;
	}
	name = &name[1];

	if (e == NULL) {
		/* nothing's there any more, so let's leave it */
		g_warning ("some impertinent character keryoinked the %s keybinding", name);
		return;
	}

	entry = find_action_entry (name);
	if (entry == NULL) {
		g_warning ("eek, menu snafu");
		return;
	}

	g_message("name is %s",name);
	oldaction = gtk_action_group_get_action (gui.action_group,
						 gconf_entry_get_key (e));

	action = gtk_action_new (entry -> name, entry -> label,
				 entry -> tooltip, entry -> stock_id);

	if (oldaction == NULL) {
		g_warning ("couldn't find the old action");
		return;
	}
	gtk_action_group_remove_action (gui.action_group, oldaction);

	gtk_action_group_add_action_with_accel (gui.action_group, action,
						gconf_client_get_string (client, gconf_entry_get_key(e), NULL));

	if (entry -> callback != NULL)
		g_signal_connect (action, "activate", G_CALLBACK (entry->callback), gui.xml);

	g_message ("somebody changed a keybinding!");
}

/* Crackful (but flexible) menu accelerator handling
 *
 * FIXME: This whole routine is probably a ginormous memory leak
 */
void
setup_menu_item (GConfClient *client, GtkActionEntry *entry)
{
	GConfEntry *e;
	gchar *key_string;
	GtkAction *action;

	key_string = g_strdup_printf ("/apps/xchat/keybindings/%s", entry->name);

	gconf_client_notify_add (client,
		 		 key_string,
				 (GConfClientNotifyFunc) &keybinding_key_changed,
				 entry, NULL, NULL);

	e = gconf_client_get_entry (client, key_string, NULL, TRUE, NULL);

	if (e == NULL) {
		/* no GConf data, so let's use the hardcoded values */
		gtk_action_group_add_actions (gui.action_group, entry, 1, NULL);
		return;
	}

	/* Let's still use most of the original hardcoded entry.. */
	action = gtk_action_new (entry->name, entry->label, entry->tooltip, entry->stock_id);

	/* but.. not the accelerators.. */
	gtk_action_group_add_action_with_accel (gui.action_group, action,
						gconf_client_get_string (client, key_string, NULL));

	if (entry->callback != NULL)
		g_signal_connect (action, "activate", G_CALLBACK (entry->callback), gui.xml);
	g_free (key_string);
}

void
setup_menu ()
{
	GConfClient *client;
	guint i;

	client = gconf_client_get_default ();

	for (i = 0; i < G_N_ELEMENTS (action_entries); i++)
		setup_menu_item (client, &action_entries[i]);

	g_object_unref (client);
}

static void
close_find_button (GtkWidget *button, gpointer data)
{
	GtkWidget *widget;
	widget = glade_xml_get_widget (gui.xml, "find hbox");
	gtk_widget_hide (widget);
	widget = glade_xml_get_widget (gui.xml, "text entry");
	gtk_widget_grab_focus (widget);
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
	const gchar *text = gtk_entry_get_text (GTK_ENTRY (entry));
	last_search_position = gtk_xtext_search (GTK_XTEXT (gui.xtext), text, last_search_position);
}

static void
clear_find (GtkWidget *entry, gpointer data)
{
	last_search_position = NULL;
}

void
initialize_main_window ()
{
	GtkWidget *entry, *topicbox, *close, *menu_vbox, *widget;
	GError *error = NULL;

	gui.main_window = GNOME_APP (glade_xml_get_widget (gui.xml, "xchat-gnome"));
	g_signal_connect (G_OBJECT (gui.main_window), "delete-event",
			  G_CALLBACK (on_main_window_close), NULL);
	/* hook up the menus */
	gui.action_group = gtk_action_group_new ("MenuAction");
//	gtk_action_group_add_actions (gui.action_group, action_entries,
//				      G_N_ELEMENTS (action_entries), NULL);
	setup_menu ();
//	initialize_gconf_accels();

	gui.manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (gui.manager, gui.action_group, 0);
	g_object_unref (gui.action_group);

	menu_vbox = glade_xml_get_widget (gui.xml, "menu_vbox");
	g_signal_connect (gui.manager, "add-widget", G_CALLBACK (on_add_widget), menu_vbox);

	/* load the menus */
	gtk_ui_manager_add_ui_from_file (gui.manager, "xchat-gnome-ui.xml", &error);
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

	entry = glade_xml_get_widget (gui.xml, "text entry");
	g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (on_text_entry_activate), NULL);
	g_signal_connect_after (G_OBJECT (entry), "key_press_event", G_CALLBACK (on_text_entry_key), NULL);
	g_signal_connect (G_OBJECT (entry), "populate-popup", G_CALLBACK (entry_context), NULL);

	close = glade_xml_get_widget (gui.xml, "close discussion");
	g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (on_discussion_close_activate), NULL);
	topicbox = glade_xml_get_widget (gui.xml, "topic hbox");
	gui.topic_label = GTK_LABEL (gtk_label_new(""));
	gtk_widget_show (GTK_WIDGET (gui.topic_label));
	gtk_box_pack_start (GTK_BOX (topicbox), GTK_WIDGET (gui.topic_label), TRUE, TRUE, 0);
	gtk_box_reorder_child (GTK_BOX (topicbox), GTK_WIDGET (gui.topic_label), 1);
	gtk_label_set_selectable (gui.topic_label, TRUE);
#if (GTK_CHECK_VERSION(2,5,0))
	gui.topic_expander = GTK_EXPANDER (gtk_expander_new (NULL));
	gtk_widget_show (GTK_WIDGET (gui.topic_expander));
	gtk_box_pack_start (GTK_BOX (topicbox), GTK_WIDGET (gui.topic_expander), FALSE, TRUE, 0);
	gtk_box_reorder_child (GTK_BOX (topicbox), GTK_WIDGET (gui.topic_expander), 0);
	gtk_expander_set_expanded (GTK_EXPANDER (gui.topic_expander), FALSE);
	gtk_expander_set_use_markup (gui.topic_expander, TRUE);
	g_signal_connect (G_OBJECT (gui.topic_expander), "activate", G_CALLBACK (on_expand_topic), NULL);
	gtk_label_set_ellipsize (gui.topic_label, PANGO_ELLIPSIZE_END);
#else
	gtk_label_set_line_wrap (gui.topic_label, TRUE);
#endif

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

	/* setup find stuff */
	widget = glade_xml_get_widget (gui.xml, "find entry");
	g_signal_connect (G_OBJECT (widget), "activate", G_CALLBACK (find_next), NULL);
	g_signal_connect (G_OBJECT (widget), "key-press-event", G_CALLBACK (close_find_key), NULL);
	g_signal_connect (G_OBJECT (widget), "changed", G_CALLBACK (clear_find), NULL);
	widget = glade_xml_get_widget (gui.xml, "find close button");
	g_signal_connect (G_OBJECT (widget), "clicked", G_CALLBACK (close_find_button), NULL);
}

void
run_main_window ()
{
	GtkWidget *pane, *widget;
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

	gtk_widget_show (GTK_WIDGET (gui.main_window));

	/* Temporarily disable menu items */
	widget = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/InsertMenu/InsertColorCodeItem");
	gtk_widget_set_sensitive (widget, FALSE);
	widget = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/NetworkMenu/NetworkInformationItem");
	gtk_widget_set_sensitive (widget, FALSE);
	widget = gtk_ui_manager_get_widget (gui.manager, "/ui/menubar/DiscussionMenu/DiscussionSaveItem");
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
on_irc_quit_activate (GtkAction *action, gpointer data)
{
	hide_transfers_window ();
	gtk_widget_hide (GTK_WIDGET (gui.main_window));
	gui.quit = TRUE;
	xchat_exit ();
}

static void
on_edit_cut_activate (GtkAction *action, gpointer data)
{
	GtkWidget *text_entry = glade_xml_get_widget(gui.xml, "text entry");
	gtk_editable_cut_clipboard(GTK_EDITABLE(text_entry));
}

static void
on_edit_copy_activate (GtkAction *action, gpointer data)
{
	GtkWidget *text_entry = glade_xml_get_widget(gui.xml, "text entry");
	gtk_editable_copy_clipboard(GTK_EDITABLE(text_entry));
}

static void
on_edit_paste_activate (GtkAction *action, gpointer data)
{
	GtkWidget *text_entry = glade_xml_get_widget(gui.xml, "text entry");
	gtk_editable_paste_clipboard(GTK_EDITABLE(text_entry));
}

static void
on_edit_clear_activate (GtkAction *action, gpointer data)
{
	GtkWidget *text_entry = glade_xml_get_widget(gui.xml, "text entry");
	gtk_editable_delete_selection(GTK_EDITABLE(text_entry));
}

static void
on_edit_preferences_activate (GtkAction *action, gpointer data)
{
	gtk_widget_show_all (GTK_WIDGET (gui.preferences_dialog));
}

static void
on_insert_color_code_activate (GtkAction *action, gpointer data)
{
	/* FIXME: implement */
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
	s->server->disconnect (s, TRUE, -1);
}

static void
on_irc_downloads_activate (GtkAction *action, gpointer data)
{
	show_transfers_window ();
}

static void
on_network_channels_activate (GtkAction *action, gpointer data)
{
	create_channel_list (gui.current_session);
}

static void
on_network_users_activate (GtkAction *action, gpointer data)
{
	/* FIXME: implement */
}

static void
on_discussion_save_activate (GtkAction *action, gpointer data)
{
	/* FIXME: implement */
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
	navigation_tree_remove (gui.server_tree, s);
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
on_discussion_find_next_activate (GtkAction *action, gpointer data)
{
	/* FIXME: implement */
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
	navigation_tree_select_prev_channel (gui.server_tree, TRUE);
}

static void
on_go_next_discussion_activate (GtkAction *action, gpointer data)
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
on_help_about_activate (GtkAction *action, gpointer data)
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
history_key_down (GtkEntry *entry)
{
	char *new_line;
	new_line = history_down (&gui.current_session->history);
	if (new_line) {
		gtk_entry_set_text (entry, new_line);
		gtk_editable_set_position (GTK_EDITABLE (entry), -1);
	}
}

static void
history_key_up (GtkEntry *entry)
{
	char *new_line;
	new_line = history_up (&gui.current_session->history, (char *)entry->text);
	if (new_line) {
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

	if (g_list_length (options) == 0) {
		/* no matches */
		g_free (text);
		g_free (prefix);
		return TRUE;
	}

	if (g_list_length (options) == 1) {
		int pos;

		/* one match */
		if (length - cursor == 0) {
			/* at the end of the entry, just insert */

			if (start != 0) {
				text[start] = '\0';
				npt = g_strdup_printf ("%s%s", text, (char *) options->data);
				pos = strlen ((char *) options->data) + start;
			} else {
				npt = g_strdup_printf ("%s: ", (char *) options->data);
				pos = strlen ((char *) options->data) + 2;
			}
		} else {
			/* somewhere in the middle of the entry */

			if (start != 0) {
				text[start] = '\0';
				npt = g_strdup_printf ("%s%s%s", text, (char *) options->data, &text[cursor]);
				pos = strlen ((char *) options->data) + start;
			} else {
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
	} else {
		/* more than one match - print a list of options
		 * to the window and update the prefix
		 */
		list = options;
		printtext = g_strdup ((char *) list->data);
		for (list = list->next; list; list = list->next) {
			npt = g_strdup_printf ("%s %s", printtext, (char *) list->data);
			g_free (printtext);
			printtext = npt;
		}
		tgui = (session_gui *) gui.current_session->gui;
		text_gui_print (tgui->buffer, printtext, TRUE);
		g_free (printtext);
		if (strcasecmp (prefix, new_prefix) != 0) {
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

	/* If we're directly after a space, we have nothing to tab complete */
	if (text[cursor_pos - 1] == ' ')
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
		if (start == 0 && text[0] == '/') {
		    /* TODO: Something? */
		}

		/* check if we can match a nickname */
		if (start == 0 || text[start] == ' ') {
			return tab_complete_nickname (entry, start == 0 ? start : start + 1);
		}
	}
	return TRUE;
}

static gboolean
on_text_entry_key (GtkWidget *widget, GdkEventKey *key, gpointer data)
{
	if (key->keyval == GDK_Down) {
		history_key_down (GTK_ENTRY (widget));
		return TRUE;
	}
	if (key->keyval == GDK_Up) {
		history_key_up (GTK_ENTRY (widget));
		return TRUE;
	}
	if (key->keyval == GDK_Tab) {
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
on_discussion_topic_change_activate (GtkButton *widget, gpointer data)
{
	GladeXML *xml = NULL;
	GtkWidget *dialog;
	GtkWidget *entry;
	GtkWidget *text_entry;
	gint response;
	GtkTextBuffer *buffer;
	gchar *title;

	if (g_file_test ("topic-change.glade", G_FILE_TEST_EXISTS))
		xml = glade_xml_new ("topic-change.glade", NULL, NULL);
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
	text_entry = glade_xml_get_widget (gui.xml, "text entry");
	gtk_widget_grab_focus (text_entry);
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
	if (gtk_expander_get_expanded (gui.topic_expander)) {
		gtk_label_set_ellipsize(gui.topic_label, PANGO_ELLIPSIZE_END);
		gtk_label_set_line_wrap (gui.topic_label, FALSE);
	} else {
		gtk_label_set_ellipsize(gui.topic_label, PANGO_ELLIPSIZE_NONE);
		gtk_label_set_line_wrap (gui.topic_label, TRUE);
	}
}
#endif
