/*
 * userlist-gui.c - helpers for the userlist view
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

#include <gnome.h>
#include "userlist-gui.h"
#include "pixmaps.h"
#include "textgui.h"
#include "palette.h"
#include "../common/xchat.h"
#include "../common/userlist.h"
#include "../common/outbound.h"

gboolean userlist_click (GtkWidget *view, GdkEventButton *event, gpointer data);
void userlist_context (GtkWidget *treeview, struct User *user);
static gint user_cmd (gchar *cmd, gchar *nick);

/* action callbacks */

static void user_send_file_activate   (GtkAction *action, gpointer data);
static void user_open_dialog_activate (GtkAction *action, gpointer data);
static void user_kick_activate        (GtkAction *action, gpointer data);
static void user_ban_activate         (GtkAction *action, gpointer data);
static void user_ignore_activate      (GtkAction *action, gpointer data);

static GtkActionEntry popup_action_entries [] = {
	{ "UserlistSendFile",   NULL, _("_Send File..."), "",   NULL, G_CALLBACK (user_send_file_activate) },
	{ "UserlistOpenDialog", NULL, _("Private _Chat"), "",   NULL, G_CALLBACK (user_open_dialog_activate) },
	{ "UserlistKick",       NULL, _("_Kick"),         "",   NULL, G_CALLBACK (user_kick_activate) },
	{ "UserlistBan",        NULL, _("_Ban"),          "",   NULL, G_CALLBACK (user_ban_activate) },
	{ "UserlistIgnore",     NULL, _("Ignore"),        "",   NULL, G_CALLBACK (user_ignore_activate) },
};

struct User *current_user;

void
initialize_userlist ()
{
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *icon_column, *text_column;
	GtkTreeSelection *select;

	gui.userlist        = glade_xml_get_widget (gui.xml, "userlist");
	gui.userlist_window = glade_xml_get_widget (gui.xml, "userlist_window");

	icon_renderer = gtk_cell_renderer_pixbuf_new ();
	icon_column = gtk_tree_view_column_new_with_attributes ("icon", icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui.userlist), icon_column);
	text_renderer = gtk_cell_renderer_text_new ();
	text_column = gtk_tree_view_column_new_with_attributes ("name", text_renderer, "text", 1, "foreground-gdk", 3, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui.userlist), text_column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui.userlist));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	g_signal_connect (G_OBJECT (gui.userlist), "button_press_event", G_CALLBACK (userlist_click), NULL);

	gtk_action_group_add_actions (gui.action_group, popup_action_entries, G_N_ELEMENTS (popup_action_entries), NULL);
}

struct User*
userlist_get_selected ()
{
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	struct User *u;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui.userlist));
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 2, &u, -1);
		return u;
	}
	return NULL;
}

gboolean
userlist_click (GtkWidget *view, GdkEventButton *event, gpointer data)
{
	GtkTreePath *path;
	GtkTreeSelection *select;
	struct User *user;
	if (!event)
		return FALSE;

	if (event->type == GDK_2BUTTON_PRESS) {
		if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (view), event->x, event->y, &path, 0, 0, 0)) {
			user = userlist_get_selected ();
			if (user != NULL)
				user_cmd ("query", user->nick);
			userlist_gui_hide ();
			return TRUE;
		}
	}

	if (event->button == 3) {
		if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (view), event->x, event->y, &path, 0, 0, 0)) {
			select = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
			gtk_tree_selection_unselect_all (select);
			gtk_tree_selection_select_path (select, path);
			gtk_tree_path_free (path);
		}
		user = userlist_get_selected ();
		if (user != NULL)
			userlist_context (view, user);
		return TRUE;
	}
	return FALSE;
}

void
userlist_context (GtkWidget *treeview, struct User *user)
{
	GtkWidget *menu;

	menu = gtk_ui_manager_get_widget (gui.manager, "/UserlistPopup");
	g_return_if_fail (menu != NULL);

	current_user = user;

	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 2, gtk_get_current_event_time ());
}

static void
user_send_file_activate (GtkAction *action, gpointer data)
{
	dcc_send_file (current_user);
}

static gint
user_cmd (gchar *cmd, gchar *nick)
{
	gint ret;
	gchar *tmp;

	tmp = g_strjoin (" ", cmd, nick, NULL);
	ret = handle_command (gui.current_session, tmp, 1);

	g_free(tmp);
	return ret;
}

static void
user_open_dialog_activate (GtkAction *action, gpointer data)
{
	user_cmd ("query", current_user->nick);
}

static void
user_kick_activate (GtkAction *action, gpointer data)
{
	user_cmd ("kick", current_user->nick);
}

static void
user_ban_activate (GtkAction *action, gpointer data)
{
	user_cmd ("ban", current_user->nick);
}

static void
user_ignore_activate (GtkAction *action, gpointer data)
{
	gchar *command;

	command = g_strdup_printf ("ignore %s!*@* ALL", current_user->nick);
	handle_command (gui.current_session, command, 1);
	g_free (command);
}

void
userlist_gui_show ()
{
	GdkScreen *screen;
	gint width, height, desired_height;
	GtkAdjustment *adjustment;
	GdkDisplay *display;
	GdkScreen *mouse_screen;
	gint mouse_x, mouse_y;

	gtk_widget_show (gui.userlist_window);

	screen = gtk_window_get_screen (GTK_WINDOW (gui.userlist_window));
	width  = gdk_screen_get_width  (screen);
	height = gdk_screen_get_height (screen);

	adjustment = gtk_tree_view_get_vadjustment (GTK_TREE_VIEW (gui.userlist));

	/* Buffer of 20 pixels.  Would be nice to know exactly how much space
	 * the rest of the window's UI goop used up, but oh well.
	 */
	desired_height = (gint) adjustment->upper + 20;
	if (desired_height > height)
		desired_height = height;

	display = gdk_display_get_default ();
	gdk_display_get_pointer (display, &mouse_screen, &mouse_x, &mouse_y, NULL);
	if (mouse_screen == screen) {
		/* Mouse is in a reasonable location, use it to position the window */
		gint screen_x, screen_y;

		screen_x = mouse_x - 100;
		screen_y = mouse_y - (desired_height / 2);

		if (screen_x < 0)
			screen_x = 0;
		if (screen_x + 250 > width)
			screen_x = width - 250;
		if (screen_y < 0)
			screen_y = 0;
		if (screen_y + desired_height > height)
			screen_y = height - desired_height;
		gtk_window_move (GTK_WINDOW (gui.userlist_window), screen_x, screen_y);
	}

	gtk_window_resize (GTK_WINDOW (gui.userlist_window), 250, desired_height);
	gtk_widget_grab_focus (gui.userlist);
}

void
userlist_gui_hide ()
{
	gtk_widget_hide (gui.userlist_window);
}
