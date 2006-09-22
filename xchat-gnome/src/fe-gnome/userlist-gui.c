/*
 * userlist-gui.c - helpers for the userlist view
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
#include <gnome.h>
#include <string.h>
#ifdef HAVE_LIBSEXY
#include <libsexy/sexy-tree-view.h>
#endif
#include "userlist-gui.h"
#include "pixmaps.h"
#include "palette.h"
#include "../common/xchat.h"
#include "../common/userlist.h"
#include "../common/outbound.h"
#include "../common/util.h"
#include "../common/server.h"

       gboolean userlist_click              (GtkWidget *view, GdkEventButton *event, gpointer data);
       void     userlist_context            (GtkWidget *treeview, struct User *user);
static gint     user_cmd                    (gchar *cmd, gchar *nick);
static void     userlist_grab               (void);
static gboolean userlist_window_event       (GtkWidget *window, GdkEvent *event, gpointer data);
static gboolean userlist_window_grab_broken (GtkWidget *window, GdkEventGrabBroken *event, gpointer data);
static void     userlist_popup_deactivate   (GtkMenuShell *menu, gpointer data);
static gboolean userlist_button_release     (GtkWidget *widget, GdkEventButton *button, gpointer data);
struct User*    userlist_get_selected       (void);
#ifdef HAVE_LIBSEXY
GtkWidget* get_user_vbox_infos	    	    (struct User *user);
static GtkWidget* get_user_tooltip	    (SexyTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data);
#endif

/* action callbacks */
static void user_send_file_activate   (GtkAction *action, gpointer data);
static void user_open_dialog_activate (GtkAction *action, gpointer data);
static void user_kick_activate        (GtkAction *action, gpointer data);
static void user_ban_activate         (GtkAction *action, gpointer data);
static void user_ignore_activate      (GtkAction *action, gpointer data);
static void user_op_activate          (GtkAction *action, gpointer data);

static GtkActionEntry popup_action_entries[] = {
	{ "UserlistSendFile",   NULL, N_("_Send File..."), "",   NULL, G_CALLBACK (user_send_file_activate) },
	{ "UserlistOpenDialog", NULL, N_("Private _Chat"), "",   NULL, G_CALLBACK (user_open_dialog_activate) },
	{ "UserlistKick",       NULL, N_("_Kick"),         "",   NULL, G_CALLBACK (user_kick_activate) },
	{ "UserlistBan",        NULL, N_("_Ban"),          "",   NULL, G_CALLBACK (user_ban_activate) },
	{ "UserlistIgnore",     NULL, N_("Ignore"),        "",   NULL, G_CALLBACK (user_ignore_activate) },
	{ "UserlistOp",	        NULL, N_("_Op"),           "",   NULL, G_CALLBACK (user_op_activate) },
};

struct User     *current_user;
static gboolean  have_grab = FALSE;
static gint      grab_menu_handler = 0;

void
initialize_userlist (void)
{
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *icon_column, *text_column;
	GtkTreeSelection *select;
	GtkWidget *swin;

#ifdef HAVE_LIBSEXY
	gui.userlist = sexy_tree_view_new ();
#else
	gui.userlist = gtk_tree_view_new ();
#endif
	gtk_widget_show (gui.userlist);
	gui.userlist_window = glade_xml_get_widget (gui.xml, "userlist_window");

	swin = glade_xml_get_widget (gui.xml, "scrolledwindow_userlist");
	gtk_container_add (GTK_CONTAINER (swin), gui.userlist);

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (gui.userlist), FALSE);
	gtk_tree_view_set_hover_selection (GTK_TREE_VIEW (gui.userlist), TRUE);

	icon_renderer = gtk_cell_renderer_pixbuf_new ();
	icon_column = gtk_tree_view_column_new_with_attributes ("icon", icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui.userlist), icon_column);
	text_renderer = gtk_cell_renderer_text_new ();
	text_column = gtk_tree_view_column_new_with_attributes ("name", text_renderer, "text", 1, "foreground-gdk", 3, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (gui.userlist), text_column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (gui.userlist));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	g_signal_connect (G_OBJECT (gui.userlist), "button_press_event", G_CALLBACK (userlist_click), NULL);
	g_signal_connect (G_OBJECT (gui.userlist_window), "button_release_event", G_CALLBACK (userlist_button_release), NULL);
	g_signal_connect (G_OBJECT (gui.userlist_window), "grab_broken_event", G_CALLBACK (userlist_window_grab_broken), NULL);
	g_signal_connect (G_OBJECT (gui.userlist_window), "event", G_CALLBACK (userlist_window_event), NULL);
#ifdef HAVE_LIBSEXY
	g_signal_connect(G_OBJECT (gui.userlist), "get-tooltip", G_CALLBACK (get_user_tooltip), NULL);
#endif

	gtk_action_group_add_actions (gui.action_group, popup_action_entries, G_N_ELEMENTS (popup_action_entries), NULL);
}

struct User*
userlist_get_selected (void)
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

	if (event->button == 1) {
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

	if (grab_menu_handler == 0)
		grab_menu_handler = g_signal_connect (G_OBJECT (menu), "deactivate", G_CALLBACK (userlist_popup_deactivate), NULL);

	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 2, gtk_get_current_event_time ());
}

static void
user_send_file_activate (GtkAction *action, gpointer data)
{
	userlist_gui_hide ();

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

	userlist_gui_hide ();
}

static void
user_kick_activate (GtkAction *action, gpointer data)
{
	user_cmd ("kick", current_user->nick);

	userlist_gui_hide ();
}

static void
user_ban_activate (GtkAction *action, gpointer data)
{
	user_cmd ("ban", current_user->nick);

	userlist_gui_hide ();
}

static void
user_ignore_activate (GtkAction *action, gpointer data)
{
	gchar *command;

	command = g_strdup_printf ("ignore %s!*@* ALL", current_user->nick);
	handle_command (gui.current_session, command, 1);
	g_free (command);

	userlist_gui_hide ();
}

static void
user_op_activate (GtkAction *action, gpointer data)
{
	user_cmd ("op", current_user->nick);

	userlist_gui_hide ();
}

void
userlist_gui_show (void)
{
	gint            desired_height;
	gint            window_x, window_y;
	gint            toggle_x, toggle_y;
	gint            monitor;
	GdkRectangle    monitor_rect;
	GdkScreen      *screen;
	GtkRequisition  request;

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gui.userlist_toggle), TRUE);

	if (!GTK_WIDGET_REALIZED (gui.userlist_window)) {
		gtk_widget_realize (gui.userlist_window);
	}
	gtk_widget_size_request (gui.userlist, &request);

	gdk_window_get_origin (gui.userlist_toggle->window, &toggle_x, &toggle_y);
	screen = gtk_widget_get_screen (gui.userlist_toggle);
	monitor = gdk_screen_get_monitor_at_point (screen, toggle_x, toggle_y);
	gdk_screen_get_monitor_geometry (screen, monitor, &monitor_rect);

	if (gtk_widget_get_direction (gui.userlist_toggle) == GTK_TEXT_DIR_RTL) {
		toggle_x += gui.userlist_toggle->allocation.x +
		            gui.userlist_toggle->allocation.width - request.width;
	} else {
		toggle_x += gui.userlist_toggle->allocation.width;
	}
	toggle_y += gui.userlist_toggle->allocation.y +
	            gui.userlist_toggle->allocation.height;

	/* Buffer of 20 pixels.  Would be nice to know exactly how much space
	 * the rest of the window's UI goop used up, but oh well.
	 */
	desired_height = request.height + 20;
	if (desired_height > monitor_rect.height)
		desired_height = monitor_rect.height;

	window_x = toggle_x + 10;
	window_y = toggle_y - (desired_height / 2);

	if (window_x < monitor_rect.x)
		window_x = monitor_rect.x;
	if (window_x + 250 > monitor_rect.width)
		window_x = monitor_rect.width - 250;
	if (window_y < monitor_rect.y)
		window_y = monitor_rect.y;
	if (window_y + desired_height > monitor_rect.height)
		window_y = monitor_rect.height - desired_height;
	gtk_window_move (GTK_WINDOW (gui.userlist_window), window_x, window_y);

	gtk_window_resize (GTK_WINDOW (gui.userlist_window), 250, desired_height);
	gtk_widget_show (gui.userlist_window);
	gtk_window_set_focus (GTK_WINDOW (gui.userlist_window), gui.userlist);
	userlist_grab ();
}

void
userlist_gui_hide (void)
{
	gint position;

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gui.userlist_toggle), FALSE);
	if (have_grab) {
		gtk_grab_remove (gui.userlist_window);
		gdk_pointer_ungrab (GDK_CURRENT_TIME);
		gdk_keyboard_ungrab (GDK_CURRENT_TIME);
		have_grab = FALSE;
	}
	gtk_widget_hide (gui.userlist_window);

	position = gtk_editable_get_position (GTK_EDITABLE (gui.text_entry));
	gtk_widget_grab_focus (gui.text_entry);
	gtk_editable_set_position (GTK_EDITABLE (gui.text_entry), position);
}

static gboolean
userlist_window_event (GtkWidget *window, GdkEvent *event, gpointer data)
{
	switch (event->type) {
	case GDK_KEY_PRESS:
		if (((GdkEventKey *)event)->keyval == GDK_Escape) {
			userlist_gui_hide ();
			break;
		}
	default:
		break;
	}
	return FALSE;
}

static gboolean
userlist_window_grab_broken (GtkWidget *window, GdkEventGrabBroken *event, gpointer data)
{
	if (have_grab && event->grab_window == NULL)
		userlist_gui_hide ();
	return TRUE;
}

static void
userlist_grab (void)
{
	if (have_grab)
		return;
	have_grab = (gdk_pointer_grab (gui.userlist_window->window, TRUE,
	                               GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK |
				       GDK_BUTTON_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK |
				       GDK_LEAVE_NOTIFY_MASK,
	                               NULL, NULL, GDK_CURRENT_TIME) == GDK_GRAB_SUCCESS);
	if (have_grab) {
		have_grab = (gdk_keyboard_grab (gui.userlist_window->window, TRUE, GDK_CURRENT_TIME) == GDK_GRAB_SUCCESS);
		if (have_grab == FALSE) {
			/* something bad happened */
			gdk_pointer_ungrab (GDK_CURRENT_TIME);
			userlist_gui_hide ();
			return;
		}
		gtk_grab_add (gui.userlist_window);
	}
}

static void
userlist_popup_deactivate (GtkMenuShell *menu, gpointer data)
{
	have_grab = FALSE;
	userlist_grab ();
}

static gboolean
userlist_button_release (GtkWidget *widget, GdkEventButton *button, gpointer data)
{
	gint x, y, width, height;

	gdk_window_get_root_origin (gui.userlist_window->window, &x, &y);
	gdk_drawable_get_size      (gui.userlist_window->window, &width, &height);

	/* If the event happened on top of the userlist window, we don't want to
	 * close it */
	if ((button->x_root > x) && (button->x_root < x + width) &&
	    (button->y_root > y) && (button->y_root < y + height)) {
		gtk_widget_event (gui.userlist, (GdkEvent *) button);
		return TRUE;
	}

	userlist_gui_hide ();
	return TRUE;
}

#ifdef HAVE_LIBSEXY
GtkWidget*
get_user_vbox_infos (struct User *user)
{
	GtkWidget *vbox, *label;
	gchar *text, *tmp, *country_txt;

	vbox = gtk_vbox_new (FALSE, 0);

	text = g_strdup_printf ("<span size=\"large\" weight=\"bold\">%s</span>", user->nick);

	if (user->realname && strlen (user->realname) > 0) {
		tmp = text;
		text = g_strdup_printf (_("%s\n<span weight=\"bold\">Name:</span> %s"), text, user->realname);
		g_free (tmp);
	}

	country_txt = country (user->hostname);
	if (country_txt && strcmp (country_txt, _("Unknown")) != 0) {
		tmp = text;
		text = g_strdup_printf (_("%s\n<span weight=\"bold\">Country:</span> %s"), text, country_txt);
		g_free (tmp);
	}

	if (user->lasttalk) {
		gint last;

		last = (gint) (time (NULL) - user->lasttalk) / 60;
		if (last >= 1) {
			tmp = text;
			text = g_strdup_printf (ngettext("%s\n<span weight=\"bold\">Last message:</span> %d minute ago", "%s\n<span weight=\"bold\">Last message:</span> %d minutes ago", last), text, last);
			g_free (tmp);
		}
	}

	if (user->away) {
		struct away_msg *away_msg;

		away_msg = server_away_find_message (gui.current_session->server, user->nick);
		if (away_msg) {
			tmp = text;
			text =  g_strdup_printf (_("%s\n<span weight=\"bold\">Away message:</span> %s"), text, away_msg->message);
			g_free (tmp);
		}
	}

	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), text);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 0);
	g_free (text);

	gtk_widget_show_all (vbox);

	return vbox;
}

static GtkWidget*
get_user_tooltip (SexyTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data)
{
	struct User *user;

	if (FALSE == GTK_WIDGET_VISIBLE (gui.userlist_window)) {
		return NULL;
	}

	user = userlist_get_selected ();

	if (user) {
		return get_user_vbox_infos (user);
	}
}
#endif
